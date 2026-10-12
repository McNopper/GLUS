/*
 * GLUS - Modern OpenGL, OpenGL ES and OpenVG Utilities. Copyright (C) since 2010 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

// GBM/DRM windowing backend: the EGL native display is a gbm device on a DRM
// node, the native window a gbm surface, and every swap is presented with a
// KMS page flip on the connector's crtc. No compositor is involved - the
// application owns the display - which is why selecting it needs no GPU:
// every EGL stack with GBM support understands the gbm types.

#include "GL/glus.h"

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>

#include "GLUS/glus_os_linux_evdev.h"

#include <string.h>

#include <time.h>

//

static int g_drmFileDescriptor = -1;

static struct gbm_device* g_gbmDevice = NULL;

static struct gbm_surface* g_gbmSurface = NULL;

static uint32_t g_connectorId = 0;

static uint32_t g_crtcId = 0;

static drmModeModeInfo g_mode;

static GLUSint g_width = -1;

static GLUSint g_height = -1;

static int g_modeSet = 0;

// Requested display rotation in degrees, set through the CMake Rotation
// variable. 90 and 270 swap width and height of the native window.
#ifndef GLUS_GBM_ROTATION
#define GLUS_GBM_ROTATION 0
#endif

// The buffer handed to the page flip of the current frame, and the one of the
// frame before, which is released back to the gbm surface once its
// replacement is queued.
static struct gbm_bo* g_currentBo = NULL;

static struct gbm_bo* g_previousBo = NULL;

static void glusOsBoDestroy(struct gbm_bo* bo, void* data)
{
    uint32_t fbId = (uint32_t)(uintptr_t)data;

    (void)bo;

    if (fbId != 0)
    {
        drmModeRmFB(g_drmFileDescriptor, fbId);
    }
}

// Each scanout buffer needs a framebuffer object once; the id is kept as the
// buffer's user data and removed with it.

static uint32_t glusOsFramebufferId(struct gbm_bo* bo)
{
    uint32_t fbId = (uint32_t)(uintptr_t)gbm_bo_get_user_data(bo);

    uint32_t width;

    uint32_t height;

    uint32_t stride;

    uint32_t handle;

    if (fbId != 0)
    {
        return fbId;
    }

    width = gbm_bo_get_width(bo);
    height = gbm_bo_get_height(bo);
    stride = gbm_bo_get_stride(bo);
    handle = gbm_bo_get_handle(bo).u32;

    if (drmModeAddFB(g_drmFileDescriptor, width, height, 24, 32, stride, handle, &fbId) != 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not add DRM framebuffer");

        return 0;
    }

    gbm_bo_set_user_data(bo, (void*)(uintptr_t)fbId, glusOsBoDestroy);

    return fbId;
}

// The first card with a connected connector is used; the connector's crtc is
// taken from its encoder, or from the first one driving it.

static int glusOsDrmSetup(uint32_t* connectorId, uint32_t* crtcId, drmModeModeInfo* mode)
{
    drmModeRes* resources = drmModeGetResources(g_drmFileDescriptor);

    int found = 0;

    int cardIndex;

    int connectorIndex;

    int encoderIndex;

    if (!resources)
    {
        return 0;
    }

    for (connectorIndex = 0; connectorIndex < resources->count_connectors && !found; connectorIndex++)
    {
        drmModeConnector* connector = drmModeGetConnector(g_drmFileDescriptor, resources->connectors[connectorIndex]);

        drmModeEncoder* encoder = NULL;

        if (!connector)
        {
            continue;
        }

        if (connector->connection != DRM_MODE_CONNECTED || connector->count_modes == 0)
        {
            drmModeFreeConnector(connector);

            continue;
        }

        if (connector->encoder_id != 0)
        {
            encoder = drmModeGetEncoder(g_drmFileDescriptor, connector->encoder_id);
        }

        if (!encoder)
        {
            for (encoderIndex = 0; encoderIndex < resources->count_encoders && !encoder; encoderIndex++)
            {
                drmModeEncoder* candidate = drmModeGetEncoder(g_drmFileDescriptor, resources->encoders[encoderIndex]);

                if (candidate && (candidate->possible_crtcs & (1 << 0)) != 0)
                {
                    encoder = candidate;
                }
                else if (candidate)
                {
                    drmModeFreeEncoder(candidate);
                }
            }
        }

        if (encoder)
        {
            *connectorId = connector->connector_id;

            *crtcId = encoder->crtc_id != 0 ? encoder->crtc_id : resources->crtcs[0];

            // The preferred mode is the panel's native one.
            *mode = connector->modes[0];

            for (cardIndex = 0; cardIndex < connector->count_modes; cardIndex++)
            {
                if (connector->modes[cardIndex].type & DRM_MODE_TYPE_PREFERRED)
                {
                    *mode = connector->modes[cardIndex];

                    break;
                }
            }

            found = 1;

            drmModeFreeEncoder(encoder);
        }

        drmModeFreeConnector(connector);
    }

    drmModeFreeResources(resources);

    return found;
}

// Applies the requested rotation through the DRM plane rotation property -
// the standard KMS mechanism. Displays whose controller cannot rotate (the
// LTDC of the STM32MP1, for example) do not expose the property; the rotation
// is then ignored with a log entry and the native orientation stays.

static int glusOsSetPlaneRotation(const int degrees)
{
    drmModePlaneRes* planeResources = drmModeGetPlaneResources(g_drmFileDescriptor);

    drmModeRes* resources = NULL;

    uint32_t rotationValue = 0;

    int crtcIndex;

    int planeIndex;

    int propertyIndex;

    int applied = 0;

    if (degrees == 90)
    {
        rotationValue = DRM_MODE_ROTATE_90;
    }
    else if (degrees == 180)
    {
        rotationValue = DRM_MODE_ROTATE_180;
    }
    else if (degrees == 270)
    {
        rotationValue = DRM_MODE_ROTATE_270;
    }

    if (!planeResources || rotationValue == 0)
    {
        if (planeResources)
        {
            drmModeFreePlaneResources(planeResources);
        }

        return 0;
    }

    resources = drmModeGetResources(g_drmFileDescriptor);

    if (resources)
    {
        for (crtcIndex = 0; crtcIndex < resources->count_crtcs; crtcIndex++)
        {
            if (resources->crtcs[crtcIndex] == g_crtcId)
            {
                break;
            }
        }

        for (planeIndex = 0; planeIndex < planeResources->count_planes && !applied; planeIndex++)
        {
            drmModePlane* plane = drmModeGetPlane(g_drmFileDescriptor, planeResources->planes[planeIndex]);

            drmModeObjectProperties* properties = NULL;

            if (!plane || (plane->possible_crtcs & (1 << crtcIndex)) == 0)
            {
                if (plane)
                {
                    drmModeFreePlane(plane);
                }

                continue;
            }

            properties = drmModeObjectGetProperties(g_drmFileDescriptor, plane->plane_id, DRM_MODE_OBJECT_PLANE);

            if (properties)
            {
                for (propertyIndex = 0; propertyIndex < properties->count_props; propertyIndex++)
                {
                    drmModePropertyRes* property = drmModeGetProperty(g_drmFileDescriptor, properties->props[propertyIndex]);

                    if (property && strcmp(property->name, "rotation") == 0)
                    {
                        if (drmModeObjectSetProperty(g_drmFileDescriptor, plane->plane_id, DRM_MODE_OBJECT_PLANE, property->prop_id, rotationValue) == 0)
                        {
                            applied = 1;
                        }
                    }

                    if (property)
                    {
                        drmModeFreeProperty(property);
                    }

                    if (applied)
                    {
                        break;
                    }
                }

                drmModeFreeObjectProperties(properties);
            }

            drmModeFreePlane(plane);
        }

        drmModeFreeResources(resources);
    }

    drmModeFreePlaneResources(planeResources);

    return applied;
}

GLUSvoid _glusOsPollEvents()
{
    glusOsEvdevPoll();

    // glusWindowLoop polls right after eglSwapBuffers: the new front buffer
    // exists now and is queued for the next page flip.
    if (g_gbmSurface != NULL)
    {
        struct gbm_bo* nextBo = gbm_surface_lock_front_buffer(g_gbmSurface);

        uint32_t fbId;

        if (!nextBo)
        {
            return;
        }

        fbId = glusOsFramebufferId(nextBo);

        if (fbId == 0)
        {
            gbm_surface_release_buffer(g_gbmSurface, nextBo);

            return;
        }

        if (!g_modeSet)
        {
            if (drmModeSetCrtc(g_drmFileDescriptor, g_crtcId, fbId, 0, 0, &g_connectorId, 1, &g_mode) == 0)
            {
                g_modeSet = 1;
            }
            else
            {
                glusLogPrint(GLUS_LOG_ERROR, "Could not set DRM crtc");

                gbm_surface_release_buffer(g_gbmSurface, nextBo);

                _glusWindowInternalClose();

                return;
            }
        }
        else if (g_currentBo != nextBo)
        {
            if (drmModePageFlip(g_drmFileDescriptor, g_crtcId, fbId, DRM_MODE_PAGE_FLIP_EVENT, NULL) != 0)
            {
                // A flip is still pending - this frame is dropped, the
                // buffer goes straight back to the surface.
                gbm_surface_release_buffer(g_gbmSurface, nextBo);

                return;
            }
        }

        if (g_previousBo != NULL && g_previousBo != nextBo)
        {
            gbm_surface_release_buffer(g_gbmSurface, g_previousBo);
        }

        g_previousBo = g_currentBo;

        g_currentBo = nextBo;
    }
}

EGLNativeDisplayType _glusOsGetNativeDisplayType()
{
    char deviceName[16];

    int deviceIndex;

    if (g_gbmDevice != NULL)
    {
        return g_gbmDevice;
    }

    for (deviceIndex = 0; deviceIndex < 16; deviceIndex++)
    {
        snprintf(deviceName, sizeof(deviceName), "/dev/dri/card%d", deviceIndex);

        g_drmFileDescriptor = open(deviceName, O_RDWR | O_CLOEXEC);

        if (g_drmFileDescriptor < 0)
        {
            continue;
        }

        if (glusOsDrmSetup(&g_connectorId, &g_crtcId, &g_mode))
        {
            break;
        }

        close(g_drmFileDescriptor);

        g_drmFileDescriptor = -1;
    }

    if (g_drmFileDescriptor < 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not find a DRM device with a connected display");

        return 0;
    }

    // Modesetting needs DRM master; a running display server keeps it.
    if (drmSetMaster(g_drmFileDescriptor) != 0)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not become DRM master - stop the running display server first");

        close(g_drmFileDescriptor);

        g_drmFileDescriptor = -1;

        return 0;
    }

    g_gbmDevice = gbm_create_device(g_drmFileDescriptor);

    if (!g_gbmDevice)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not create GBM device");

        drmDropMaster(g_drmFileDescriptor);

        close(g_drmFileDescriptor);

        g_drmFileDescriptor = -1;

        return 0;
    }

    return g_gbmDevice;
}

EGLNativeWindowType _glusOsCreateNativeWindowType(const char* title, const GLUSint width, const GLUSint height, const GLUSboolean fullscreen, const GLUSboolean noResize, EGLint eglNativeVisualID)
{
    (void)eglNativeVisualID;

    glusLogPrint(GLUS_LOG_INFO, "Parameters '%s', 'fullscreen' and 'noResize' are not used", title != NULL ? title : "");
    glusLogPrint(GLUS_LOG_INFO, "Key events are mapped to US keyboard");
    glusLogPrint(GLUS_LOG_INFO, "Touch display is used for mouse events and are limited");

    if (g_gbmDevice == NULL)
    {
        return 0;
    }

    // The KMS mode governs the real size; the requested one only selects
    // between the connector's modes when it matches one exactly.
    if (width > 0 && height > 0)
    {
        drmModeConnector* connector = drmModeGetConnector(g_drmFileDescriptor, g_connectorId);

        int modeIndex;

        if (connector)
        {
            for (modeIndex = 0; modeIndex < connector->count_modes; modeIndex++)
            {
                if (connector->modes[modeIndex].hdisplay == width && connector->modes[modeIndex].vdisplay == height)
                {
                    g_mode = connector->modes[modeIndex];

                    break;
                }
            }

            drmModeFreeConnector(connector);
        }
    }

    g_width = g_mode.hdisplay;

    g_height = g_mode.vdisplay;

#if GLUS_GBM_ROTATION != 0
    // The same orientation handling as the Android backend, which swaps the
    // window size for non-portrait configurations. The rotation is applied
    // through the DRM plane rotation property where the display pipeline
    // supports it; the window keeps its native orientation otherwise.
    if (glusOsSetPlaneRotation(GLUS_GBM_ROTATION))
    {
        if (GLUS_GBM_ROTATION == 90 || GLUS_GBM_ROTATION == 270)
        {
            const GLUSint swapped = g_width;

            g_width = g_height;

            g_height = swapped;
        }
    }
    else
    {
        glusLogPrint(GLUS_LOG_INFO, "Rotation=%d is not supported by the display pipeline - using the native orientation", GLUS_GBM_ROTATION);
    }
#endif

    glusOsEvdevSetDisplayGeometry(g_width, g_height);

    glusOsEvdevInit();

    g_gbmSurface = gbm_surface_create(g_gbmDevice, g_width, g_height, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    if (!g_gbmSurface)
    {
        glusLogPrint(GLUS_LOG_ERROR, "Could not create native window");

        glusOsEvdevTerminate();

        return 0;
    }

    return g_gbmSurface;
}

GLUSvoid _glusOsDestroyNativeWindowDisplay()
{
    glusOsEvdevTerminate();

    if (g_previousBo != NULL && g_gbmSurface != NULL)
    {
        gbm_surface_release_buffer(g_gbmSurface, g_previousBo);
    }

    g_previousBo = NULL;

    if (g_currentBo != NULL && g_gbmSurface != NULL)
    {
        gbm_surface_release_buffer(g_gbmSurface, g_currentBo);
    }

    g_currentBo = NULL;

    if (g_gbmSurface)
    {
        gbm_surface_destroy(g_gbmSurface);

        g_gbmSurface = NULL;
    }

    if (g_gbmDevice)
    {
        gbm_device_destroy(g_gbmDevice);

        g_gbmDevice = NULL;
    }

    if (g_drmFileDescriptor >= 0)
    {
        drmDropMaster(g_drmFileDescriptor);

        close(g_drmFileDescriptor);

        g_drmFileDescriptor = -1;
    }

    g_connectorId = 0;

    g_crtcId = 0;

    g_modeSet = 0;

    g_width = -1;

    g_height = -1;
}

double _glusOsGetRawTime()
{
    struct timespec currentTime;

    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000.0;
}

GLUSvoid _glusOsGetWindowSize(GLUSint* width, GLUSint* height)
{
    if (width)
    {
        *width = g_width;
    }

    if (height)
    {
        *height = g_height;
    }
}
