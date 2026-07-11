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

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GL/glus.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Matches the GLUS skinned PBR vertex shader. */
#define GLUS_GLTF_MAX_JOINTS 128

/* Upper bound on morph targets per primitive. The morph vertex shader uses a
 * uniform array of this size; loader clamps larger counts (rare). */
#define GLUS_GLTF_MAX_MORPH_TARGETS 32

typedef struct _GLUSgltfImageCacheEntry
{
    cgltf_image* image;
    GLuint       texture;
} GLUSgltfImageCacheEntry;

typedef struct _GLUSgltfLoadContext
{
    GLUSgltfScene*           scene;
    const GLUSchar*          basePath;
    GLUSboolean              sRGB;
    GLUSgltfImageCacheEntry* cache;
    GLUSint                  cacheCount;
    GLUSint                  cacheCap;
} GLUSgltfLoadContext;

static GLUSchar* gltfCopyString(const GLUSchar* s)
{
    GLUSint  n;
    GLUSchar* out;

    if (!s)
    {
        return NULL;
    }
    n   = (GLUSint)strlen(s) + 1;
    out = (GLUSchar*)malloc((size_t)n);
    if (out)
    {
        memcpy(out, s, (size_t)n);
    }
    return out;
}

static GLUSvoid gltfDeriveBasePath(const GLUSchar* filename, GLUSchar* out, GLUSint outSize)
{
    const GLUSchar* slash;
    const GLUSchar* backslash;
    const GLUSchar* sep;
    GLUSint          i;

    if (outSize > 0)
    {
        out[0] = '\0';
    }
    if (!filename)
    {
        return;
    }

    slash     = strrchr(filename, '/');
    backslash = strrchr(filename, '\\');
    sep       = (slash > backslash) ? slash : backslash;

    if (!sep)
    {
        return;
    }
    for (i = 0; i < outSize - 1; i++)
    {
        out[i] = filename[i];
        if (&filename[i] == sep)
        {
            out[i + 1] = '\0';
            return;
        }
    }
    if (outSize > 0)
    {
        out[outSize - 1] = '\0';
    }
}

static GLuint gltfCreateTexture1x1(GLUSubyte r, GLUSubyte g, GLUSubyte b, GLUSubyte a)
{
    GLuint  tex;
    GLubyte data[4];

    data[0] = r;
    data[1] = g;
    data[2] = b;
    data[3] = a;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return tex;
}

static GLUSint gltfB64Val(GLUSchar c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9')
    {
        return c - '0' + 52;
    }
    if (c == '+')
    {
        return 62;
    }
    if (c == '/')
    {
        return 63;
    }
    return -1;
}

static GLUSubyte* gltfBase64Decode(const GLUSchar* in, GLUSint len, GLUSint* outLen)
{
    GLUSubyte* out;
    GLUSint    o = 0;
    GLUSint    i;
    GLUSint    quad[4];
    GLUSint    qn = 0;

    out = (GLUSubyte*)malloc((size_t)len);
    if (!out)
    {
        return NULL;
    }
    for (i = 0; i < len; i++)
    {
        GLUSint v;
        if (in[i] == '=' || in[i] == '\n' || in[i] == '\r' || in[i] == ' ')
        {
            continue;
        }
        v = gltfB64Val(in[i]);
        if (v < 0)
        {
            continue;
        }
        quad[qn++] = v;
        if (qn == 4)
        {
            out[o++] = (GLUSubyte)((quad[0] << 2) | (quad[1] >> 4));
            out[o++] = (GLUSubyte)((quad[1] << 4) | (quad[2] >> 2));
            out[o++] = (GLUSubyte)((quad[2] << 6) | quad[3]);
            qn        = 0;
        }
    }
    if (qn == 2)
    {
        out[o++] = (GLUSubyte)((quad[0] << 2) | (quad[1] >> 4));
    }
    else if (qn == 3)
    {
        out[o++] = (GLUSubyte)((quad[0] << 2) | (quad[1] >> 4));
        out[o++] = (GLUSubyte)((quad[1] << 4) | (quad[2] >> 2));
    }
    *outLen = o;
    return out;
}

static GLUSubyte* gltfLoadImagePixels(cgltf_image* image, const GLUSchar* basePath, GLint* w, GLint* h)
{
    if (image->buffer_view)
    {
        const GLUSubyte* buf = (const GLUSubyte*)image->buffer_view->buffer->data + image->buffer_view->offset;
        return stbi_load_from_memory(buf, (int)image->buffer_view->size, w, h, NULL, 4);
    }
    if (image->uri)
    {
        if (strncmp(image->uri, "data:", 5) == 0)
        {
            const GLUSchar* comma = strchr(image->uri, ',');
            if (comma)
            {
                GLUSint   len = (GLUSint)strlen(comma + 1);
                GLUSint   outLen = 0;
                GLUSubyte* dec = gltfBase64Decode(comma + 1, len, &outLen);
                GLUSubyte* pix = NULL;
                if (dec)
                {
                    pix = stbi_load_from_memory(dec, outLen, w, h, NULL, 4);
                    free(dec);
                }
                return pix;
            }
            return NULL;
        }
        {
            GLUSchar path[4096];
            if (basePath && basePath[0])
            {
                snprintf(path, sizeof(path), "%s%s", basePath, image->uri);
            }
            else
            {
                strncpy(path, image->uri, sizeof(path) - 1);
                path[sizeof(path) - 1] = '\0';
            }
            return stbi_load(path, w, h, NULL, 4);
        }
    }
    return NULL;
}

static GLUSvoid gltfApplySampler(cgltf_sampler* sampler)
{
    if (!sampler)
    {
        return;
    }
    if (sampler->min_filter)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)sampler->min_filter);
    }
    if (sampler->mag_filter)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)sampler->mag_filter);
    }
    if (sampler->wrap_s)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)sampler->wrap_s);
    }
    if (sampler->wrap_t)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)sampler->wrap_t);
    }
}

static GLuint gltfLoadImageTexture(GLUSgltfLoadContext* ctx, cgltf_image* image, GLUSboolean sRGB)
{
    GLint     w, h;
    GLUSubyte* pix;
    GLuint    tex;
    GLint     internalFmt;
    GLint     i;

    if (!image)
    {
        return ctx->scene->defaultWhiteTexture;
    }
    for (i = 0; i < ctx->cacheCount; i++)
    {
        if (ctx->cache[i].image == image)
        {
            return ctx->cache[i].texture;
        }
    }

    pix = gltfLoadImagePixels(image, ctx->basePath, &w, &h);
    if (!pix)
    {
        glusLogPrint(GLUS_LOG_WARNING, "glTF: failed to load image '%s'", image->uri ? image->uri : "(embedded)");
        return ctx->scene->defaultWhiteTexture;
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    internalFmt = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    stbi_image_free(pix);

    if (ctx->cacheCount < ctx->cacheCap)
    {
        ctx->cache[ctx->cacheCount].image   = image;
        ctx->cache[ctx->cacheCount].texture = tex;
        ctx->cacheCount++;
    }
    return tex;
}

static GLint gltfTypeComponents(cgltf_type t)
{
    switch (t)
    {
    case cgltf_type_vec2:
        return 2;
    case cgltf_type_vec3:
        return 3;
    case cgltf_type_vec4:
        return 4;
    case cgltf_type_mat2:
        return 4;
    case cgltf_type_mat3:
        return 9;
    case cgltf_type_mat4:
        return 16;
    case cgltf_type_scalar:
    case cgltf_type_invalid:
    default:
        return 1;
    }
}

/* Read a whole accessor into a freshly allocated GLfloat array, sparse storage
 * included (cgltf_accessor_unpack_floats applies sparse; the per-element
 * cgltf_accessor_read_float does not). When the accessor's component count is
 * smaller than the requested one (e.g. a VEC3 colour uploaded as vec4), the
 * trailing components are padded (alpha -> 1.0 for vec4, else 0.0). */
static GLfloat* gltfReadAccessorFloats(cgltf_accessor* acc, GLint components)
{
    GLint     native;
    GLint     n;
    GLint     i;
    GLint     c;
    GLfloat*  out;
    GLfloat*  tmp;

    if (!acc)
    {
        return NULL;
    }
    native = gltfTypeComponents(acc->type);
    n      = (GLint)acc->count;
    out    = (GLfloat*)malloc((size_t)n * (size_t)components * sizeof(GLfloat));
    if (!out)
    {
        return NULL;
    }
    if (native == components)
    {
        cgltf_accessor_unpack_floats(acc, out, (cgltf_size)n * (cgltf_size)native);
        return out;
    }
    tmp = (GLfloat*)malloc((size_t)n * (size_t)native * sizeof(GLfloat));
    if (!tmp)
    {
        free(out);
        return NULL;
    }
    cgltf_accessor_unpack_floats(acc, tmp, (cgltf_size)n * (cgltf_size)native);
    for (i = 0; i < n; i++)
    {
        for (c = 0; c < native; c++)
        {
            out[i * components + c] = tmp[i * native + c];
        }
        for (c = native; c < components; c++)
        {
            out[i * components + c] = (components == 4 && c == 3) ? 1.0f : 0.0f;
        }
    }
    free(tmp);
    return out;
}

static GLuint gltfUploadFloatStream(cgltf_accessor* acc, GLint components)
{
    GLuint   vbo = 0;
    GLfloat* buf;

    buf = gltfReadAccessorFloats(acc, components);
    if (!buf)
    {
        return 0;
    }
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizei)((size_t)acc->count * (size_t)components * sizeof(GLfloat)), buf, GL_STATIC_DRAW);
    free(buf);
    return vbo;
}

static GLuint gltfUploadZeroStream(GLsizei count, GLint components)
{
    GLuint   vbo = 0;
    GLfloat* buf = (GLfloat*)calloc((size_t)count * (size_t)components, sizeof(GLfloat));
    if (!buf)
    {
        return 0;
    }
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizei)((size_t)count * (size_t)components * sizeof(GLfloat)), buf, GL_STATIC_DRAW);
    free(buf);
    return vbo;
}

static GLUSvoid gltfUploadIndices(cgltf_accessor* acc, GLuint* outIbo, GLsizei* outCount, GLenum* outType)
{
    GLsizei  n;
    GLint    i;
    GLuint*  ibuf;
    GLfloat* fbuf;

    if (!acc)
    {
        *outIbo   = 0;
        *outCount = 0;
        return;
    }
    n    = (GLsizei)acc->count;
    /* Read sparse-aware as floats, then cast to GLuint. */
    fbuf = gltfReadAccessorFloats(acc, 1);
    if (!fbuf)
    {
        return;
    }
    ibuf = (GLuint*)malloc((size_t)n * sizeof(GLuint));
    if (!ibuf)
    {
        free(fbuf);
        return;
    }
    for (i = 0; i < n; i++)
    {
        ibuf[i] = (GLuint)fbuf[i];
    }
    free(fbuf);

    glGenBuffers(1, outIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *outIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)((size_t)n * sizeof(GLuint)), ibuf, GL_STATIC_DRAW);
    free(ibuf);
    *outCount = n;
    *outType  = GL_UNSIGNED_INT;
}

static cgltf_accessor* gltfFindAttribute(cgltf_primitive* prim, cgltf_attribute_type type, GLint index)
{
    GLint i;

    for (i = 0; i < (GLint)prim->attributes_count; i++)
    {
        if (prim->attributes[i].type == type && (GLint)prim->attributes[i].index == index)
        {
            return prim->attributes[i].data;
        }
    }
    return NULL;
}

static cgltf_accessor* gltfFindTargetAttribute(cgltf_morph_target* tgt, cgltf_attribute_type type)
{
    GLint i;

    for (i = 0; i < (GLint)tgt->attributes_count; i++)
    {
        if (tgt->attributes[i].type == type && (GLint)tgt->attributes[i].index == 0)
        {
            return tgt->attributes[i].data;
        }
    }
    return NULL;
}

/* Upload per-target POSITION / NORMAL / TANGENT deltas into SSBOs packed as
 * [target][vertex]. Sparse-aware via gltfReadAccessorFloats. */
static GLUSvoid gltfUploadMorphDeltas(GLUSgltfPrimitive* gp, cgltf_primitive* prim, GLsizei vertCount, GLint mt)
{
    GLfloat* posBuf;
    GLfloat* norBuf = NULL;
    GLfloat* tanBuf = NULL;
    GLint    ti;
    GLint    hasN = 0;
    GLint    hasT = 0;
    size_t   posFloats = (size_t)mt * (size_t)vertCount * 3u;

    posBuf = (GLfloat*)calloc(posFloats, sizeof(GLfloat));
    if (mt > 0 && gltfFindTargetAttribute(&prim->targets[0], cgltf_attribute_type_normal))
    {
        hasN   = 1;
        norBuf = (GLfloat*)calloc(posFloats, sizeof(GLfloat));
    }
    if (mt > 0 && gltfFindTargetAttribute(&prim->targets[0], cgltf_attribute_type_tangent))
    {
        hasT   = 1;
        tanBuf = (GLfloat*)calloc((size_t)mt * (size_t)vertCount * 4u, sizeof(GLfloat));
    }

    for (ti = 0; ti < mt; ti++)
    {
        cgltf_morph_target* tgt = &prim->targets[ti];
        cgltf_accessor*     a;

        a = gltfFindTargetAttribute(tgt, cgltf_attribute_type_position);
        if (a)
        {
            GLfloat* tmp = gltfReadAccessorFloats(a, 3);
            if (tmp)
            {
                memcpy(posBuf + (size_t)ti * vertCount * 3, tmp, (size_t)vertCount * 3 * sizeof(GLfloat));
                free(tmp);
            }
        }
        if (hasN)
        {
            a = gltfFindTargetAttribute(tgt, cgltf_attribute_type_normal);
            if (a)
            {
                GLfloat* tmp = gltfReadAccessorFloats(a, 3);
                if (tmp)
                {
                    memcpy(norBuf + (size_t)ti * vertCount * 3, tmp, (size_t)vertCount * 3 * sizeof(GLfloat));
                    free(tmp);
                }
            }
        }
        if (hasT)
        {
            a = gltfFindTargetAttribute(tgt, cgltf_attribute_type_tangent);
            if (a)
            {
                GLfloat* tmp = gltfReadAccessorFloats(a, 4);
                if (tmp)
                {
                    memcpy(tanBuf + (size_t)ti * vertCount * 4, tmp, (size_t)vertCount * 4 * sizeof(GLfloat));
                    free(tmp);
                }
            }
        }
    }

    glGenBuffers(1, &gp->morphPositionSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gp->morphPositionSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizei)(posFloats * sizeof(GLfloat)), posBuf, GL_STATIC_DRAW);
    if (norBuf)
    {
        glGenBuffers(1, &gp->morphNormalSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, gp->morphNormalSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizei)(posFloats * sizeof(GLfloat)), norBuf, GL_STATIC_DRAW);
    }
    if (tanBuf)
    {
        glGenBuffers(1, &gp->morphTangentSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, gp->morphTangentSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizei)((size_t)mt * vertCount * 4 * sizeof(GLfloat)), tanBuf, GL_STATIC_DRAW);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    free(posBuf);
    free(norBuf);
    free(tanBuf);
}

static GLenum gltfPrimitiveMode(cgltf_primitive_type t)
{
    switch (t)
    {
    case cgltf_primitive_type_points:
        return GL_POINTS;
    case cgltf_primitive_type_lines:
        return GL_LINES;
    case cgltf_primitive_type_line_loop:
        return GL_LINE_LOOP;
    case cgltf_primitive_type_line_strip:
        return GL_LINE_STRIP;
    case cgltf_primitive_type_triangle_strip:
        return GL_TRIANGLE_STRIP;
    case cgltf_primitive_type_triangle_fan:
        return GL_TRIANGLE_FAN;
    case cgltf_primitive_type_triangles:
    default:
        return GL_TRIANGLES;
    }
}

static GLUSvoid gltfFillMaterial(GLUSgltfLoadContext* ctx, GLUSgltfMaterial* m, cgltf_material* mat, cgltf_accessor* accTan)
{
    m->baseColorTexture         = ctx->scene->defaultWhiteTexture;
    m->metallicRoughnessTexture = ctx->scene->defaultWhiteTexture;
    m->normalTexture            = ctx->scene->defaultNormalTexture;
    m->occlusionTexture         = ctx->scene->defaultWhiteTexture;
    m->emissiveTexture          = ctx->scene->defaultWhiteTexture;

    m->baseColorFactor[0] = 1.0f;
    m->baseColorFactor[1] = 1.0f;
    m->baseColorFactor[2] = 1.0f;
    m->baseColorFactor[3] = 1.0f;
    m->metallicFactor     = 1.0f;
    m->roughnessFactor    = 1.0f;
    m->emissiveFactor[0]  = 0.0f;
    m->emissiveFactor[1]  = 0.0f;
    m->emissiveFactor[2]  = 0.0f;
    m->occlusionStrength  = 1.0f;
    m->normalScale        = 1.0f;
    m->alphaCutoff        = 0.5f;
    m->alphaMode          = GLUS_GLTF_ALPHA_OPAQUE;
    m->doubleSided        = GLUS_FALSE;
    m->hasNormalMap       = GLUS_FALSE;

    m->baseColorTexCoordSet         = 0;
    m->metallicRoughnessTexCoordSet = 0;
    m->normalTexCoordSet            = 0;
    m->occlusionTexCoordSet         = 0;
    m->emissiveTexCoordSet          = 0;

    if (!mat)
    {
        return;
    }

    m->doubleSided = mat->double_sided ? GLUS_TRUE : GLUS_FALSE;
    m->alphaMode   = (GLint)mat->alpha_mode;
    m->alphaCutoff = mat->alpha_cutoff > 0.0f ? (GLfloat)mat->alpha_cutoff : 0.5f;

    if (mat->has_pbr_metallic_roughness)
    {
        cgltf_pbr_metallic_roughness* pbr = &mat->pbr_metallic_roughness;
        GLint                          c;
        for (c = 0; c < 4; c++)
        {
            m->baseColorFactor[c] = (GLfloat)pbr->base_color_factor[c];
        }
        m->metallicFactor  = (GLfloat)pbr->metallic_factor;
        m->roughnessFactor = (GLfloat)pbr->roughness_factor;

        if (pbr->base_color_texture.texture)
        {
            m->baseColorTexture = gltfLoadImageTexture(ctx, pbr->base_color_texture.texture->image, ctx->sRGB);
            glBindTexture(GL_TEXTURE_2D, m->baseColorTexture);
            gltfApplySampler(pbr->base_color_texture.texture->sampler);
            m->baseColorTexCoordSet = (GLint)pbr->base_color_texture.texcoord;
        }
        if (pbr->metallic_roughness_texture.texture)
        {
            m->metallicRoughnessTexture = gltfLoadImageTexture(ctx, pbr->metallic_roughness_texture.texture->image, GLUS_FALSE);
            glBindTexture(GL_TEXTURE_2D, m->metallicRoughnessTexture);
            gltfApplySampler(pbr->metallic_roughness_texture.texture->sampler);
            m->metallicRoughnessTexCoordSet = (GLint)pbr->metallic_roughness_texture.texcoord;
        }
    }

    if (mat->normal_texture.texture)
    {
        m->normalTexture = gltfLoadImageTexture(ctx, mat->normal_texture.texture->image, GLUS_FALSE);
        m->hasNormalMap  = accTan ? GLUS_TRUE : GLUS_FALSE;
        m->normalScale   = (GLfloat)mat->normal_texture.scale;
        glBindTexture(GL_TEXTURE_2D, m->normalTexture);
        gltfApplySampler(mat->normal_texture.texture->sampler);
        m->normalTexCoordSet = (GLint)mat->normal_texture.texcoord;
    }
    if (mat->occlusion_texture.texture)
    {
        m->occlusionTexture = gltfLoadImageTexture(ctx, mat->occlusion_texture.texture->image, GLUS_FALSE);
        m->occlusionStrength = (GLfloat)mat->occlusion_texture.scale;
        glBindTexture(GL_TEXTURE_2D, m->occlusionTexture);
        gltfApplySampler(mat->occlusion_texture.texture->sampler);
        m->occlusionTexCoordSet = (GLint)mat->occlusion_texture.texcoord;
    }
    if (mat->emissive_texture.texture)
    {
        m->emissiveTexture = gltfLoadImageTexture(ctx, mat->emissive_texture.texture->image, ctx->sRGB);
        glBindTexture(GL_TEXTURE_2D, m->emissiveTexture);
        gltfApplySampler(mat->emissive_texture.texture->sampler);
        m->emissiveTexCoordSet = (GLint)mat->emissive_texture.texcoord;
    }

    m->emissiveFactor[0] = (GLfloat)mat->emissive_factor[0];
    m->emissiveFactor[1] = (GLfloat)mat->emissive_factor[1];
    m->emissiveFactor[2] = (GLfloat)mat->emissive_factor[2];
}

static GLUSvoid gltfExpandPoint(GLUSgltfScene* scene, const GLfloat p[3])
{
    GLint a;
    for (a = 0; a < 3; a++)
    {
        if (p[a] < scene->sceneMin[a])
        {
            scene->sceneMin[a] = p[a];
        }
        if (p[a] > scene->sceneMax[a])
        {
            scene->sceneMax[a] = p[a];
        }
    }
}

static GLUSvoid gltfExpandBounds(GLUSgltfScene* scene, const cgltf_float minP[3], const cgltf_float maxP[3], const GLfloat modelMatrix[16])
{
    GLint cx, cy, cz;
    for (cx = 0; cx < 2; cx++)
    {
        for (cy = 0; cy < 2; cy++)
        {
            for (cz = 0; cz < 2; cz++)
            {
                GLfloat corner[4];
                GLfloat world[4];
                GLfloat wp[3];
                corner[0] = cx ? (GLfloat)maxP[0] : (GLfloat)minP[0];
                corner[1] = cy ? (GLfloat)maxP[1] : (GLfloat)minP[1];
                corner[2] = cz ? (GLfloat)maxP[2] : (GLfloat)minP[2];
                corner[3] = 1.0f;
                glusMatrix4x4MultiplyPoint4f(world, modelMatrix, corner);
                wp[0] = world[0];
                wp[1] = world[1];
                wp[2] = world[2];
                gltfExpandPoint(scene, wp);
            }
        }
    }
}

static GLUSvoid gltfProcessNodeMeshes(GLUSgltfLoadContext* ctx, GLint nodeIndex, GLint* cursor)
{
    GLUSgltfScene* scene = ctx->scene;
    cgltf_data*    data = scene->cgltfData;
    cgltf_node*    cnode = &data->nodes[nodeIndex];
    cgltf_mesh*    mesh = cnode->mesh;
    GLint          skinIdx = scene->nodes[nodeIndex].skinIndex;
    GLint          pi;

    for (pi = 0; pi < (GLint)mesh->primitives_count; pi++)
    {
        cgltf_primitive*  prim = &mesh->primitives[pi];
        GLUSgltfPrimitive* gp;
        cgltf_accessor    *accPos, *accNor, *accTan, *accUV0, *accUV1, *accColor, *accJoints, *accWeights;
        GLsizei           vertCount;
        GLfloat           tmp[16];
        GLint             ti;
        cgltf_float*      weights;
        cgltf_size        weightCount;

        accPos = gltfFindAttribute(prim, cgltf_attribute_type_position, 0);
        if (!accPos)
        {
            continue;
        }

        gp = &scene->primitives[(*cursor)++];
        memset(gp, 0, sizeof(*gp));
        gp->nodeIndex = nodeIndex;
        gp->skinIndex = skinIdx;
        gp->mode      = gltfPrimitiveMode(prim->type);
        gp->indexType = GL_UNSIGNED_INT;

        accNor     = gltfFindAttribute(prim, cgltf_attribute_type_normal, 0);
        accTan     = gltfFindAttribute(prim, cgltf_attribute_type_tangent, 0);
        accUV0     = gltfFindAttribute(prim, cgltf_attribute_type_texcoord, 0);
        accUV1     = gltfFindAttribute(prim, cgltf_attribute_type_texcoord, 1);
        accColor   = gltfFindAttribute(prim, cgltf_attribute_type_color, 0);
        accJoints  = gltfFindAttribute(prim, cgltf_attribute_type_joints, 0);
        accWeights = gltfFindAttribute(prim, cgltf_attribute_type_weights, 0);

        vertCount = (GLsizei)accPos->count;

        gp->vboPosition  = gltfUploadFloatStream(accPos, 3);
        gp->vboNormal    = accNor ? gltfUploadFloatStream(accNor, 3) : gltfUploadZeroStream(vertCount, 3);
        gp->vboTangent   = accTan ? gltfUploadFloatStream(accTan, 4) : gltfUploadZeroStream(vertCount, 4);
        gp->vboTexCoord0 = accUV0 ? gltfUploadFloatStream(accUV0, 2) : gltfUploadZeroStream(vertCount, 2);
        gp->vboTexCoord1 = accUV1 ? gltfUploadFloatStream(accUV1, 2) : 0;
        gp->vboColor     = accColor ? gltfUploadFloatStream(accColor, 4) : 0;

        if (skinIdx >= 0 && accJoints && accWeights)
        {
            gp->vboJoints  = gltfUploadFloatStream(accJoints, 4);
            gp->vboWeights = gltfUploadFloatStream(accWeights, 4);
        }

        if (prim->indices)
        {
            gltfUploadIndices(prim->indices, &gp->ibo, &gp->indexCount, &gp->indexType);
        }
        else
        {
            gp->vertexCount = vertCount;
        }

        glGenVertexArrays(1, &gp->vao);
        glBindVertexArray(gp->vao);

        glBindBuffer(GL_ARRAY_BUFFER, gp->vboPosition);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, gp->vboNormal);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, gp->vboTangent);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, gp->vboTexCoord0);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(3);
        if (gp->vboJoints)
        {
            glBindBuffer(GL_ARRAY_BUFFER, gp->vboJoints);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(4);
            glBindBuffer(GL_ARRAY_BUFFER, gp->vboWeights);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(5);
        }

        /* TEXCOORD_1 (location 6) and COLOR_0 (location 7). When absent a
         * constant default is used so the shader always has valid input. */
        if (gp->vboTexCoord1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, gp->vboTexCoord1);
            glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(6);
        }
        else
        {
            glVertexAttrib2f(6, 0.0f, 0.0f);
        }
        if (gp->vboColor)
        {
            glBindBuffer(GL_ARRAY_BUFFER, gp->vboColor);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(7);
        }
        else
        {
            glVertexAttrib4f(7, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (gp->ibo)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gp->ibo);
        }
        glBindVertexArray(0);

        gltfFillMaterial(ctx, &gp->material, prim->material, accTan);

        memcpy(gp->modelMatrix, scene->nodes[nodeIndex].worldMatrix, sizeof(gp->modelMatrix));
        memcpy(tmp, gp->modelMatrix, sizeof(tmp));
        glusMatrix4x4Inversef(tmp);
        glusMatrix4x4Transposef(tmp);
        glusMatrix4x4ExtractMatrix3x3f(gp->normalMatrix, tmp);

        /* Morph targets (core): default weights + delta SSBO upload. */
        gp->morphTargetCount = (GLint)prim->targets_count;
        if (gp->morphTargetCount > GLUS_GLTF_MAX_MORPH_TARGETS)
        {
            glusLogPrint(GLUS_LOG_WARNING, "glTF: %d morph targets, clamping to %d",
                         gp->morphTargetCount, GLUS_GLTF_MAX_MORPH_TARGETS);
            gp->morphTargetCount = GLUS_GLTF_MAX_MORPH_TARGETS;
        }
        if (gp->morphTargetCount > 0)
        {
            weights     = cnode->weights_count ? cnode->weights : mesh->weights;
            weightCount = cnode->weights_count ? cnode->weights_count : mesh->weights_count;
            gp->morphWeights = (GLfloat*)malloc(sizeof(GLfloat) * gp->morphTargetCount);
            for (ti = 0; ti < gp->morphTargetCount; ti++)
            {
                gp->morphWeights[ti] = ((cgltf_size)ti < weightCount) ? (GLfloat)weights[ti] : 0.0f;
            }
            gltfUploadMorphDeltas(gp, prim, vertCount, gp->morphTargetCount);
        }

        if (accPos->has_min && accPos->has_max)
        {
            gltfExpandBounds(scene, accPos->min, accPos->max, gp->modelMatrix);
        }
    }
}

static GLUSvoid gltfBuildNodes(GLUSgltfScene* scene)
{
    cgltf_data* data = scene->cgltfData;
    GLint       ni, ci, i;

    scene->nodeCount = (GLint)data->nodes_count;
    if (scene->nodeCount <= 0)
    {
        return;
    }
    scene->nodes = (GLUSgltfNode*)calloc((size_t)scene->nodeCount, sizeof(GLUSgltfNode));

    for (ni = 0; ni < scene->nodeCount; ni++)
    {
        cgltf_node*  cn = &data->nodes[ni];
        GLUSgltfNode* gn = &scene->nodes[ni];

        gn->translation[0] = gn->translation[1] = gn->translation[2] = 0.0f;
        gn->rotation[0] = gn->rotation[1] = gn->rotation[2] = 0.0f;
        gn->rotation[3]                                       = 1.0f;
        gn->scale[0] = gn->scale[1] = gn->scale[2] = 1.0f;
        gn->hasMatrix                               = GLUS_FALSE;
        gn->parentIndex                             = -1;
        gn->meshIndex                               = -1;
        gn->skinIndex                               = -1;
        gn->cameraIndex                             = -1;
        glusMatrix4x4Identityf(gn->worldMatrix);
        glusMatrix4x4Identityf(gn->localMatrix);

        if (cn->has_matrix)
        {
            gn->hasMatrix = GLUS_TRUE;
            for (i = 0; i < 16; i++)
            {
                gn->localMatrix[i] = (GLfloat)cn->matrix[i];
            }
        }
        else
        {
            if (cn->has_translation)
            {
                gn->translation[0] = (GLfloat)cn->translation[0];
                gn->translation[1] = (GLfloat)cn->translation[1];
                gn->translation[2] = (GLfloat)cn->translation[2];
            }
            if (cn->has_rotation)
            {
                gn->rotation[0] = (GLfloat)cn->rotation[0];
                gn->rotation[1] = (GLfloat)cn->rotation[1];
                gn->rotation[2] = (GLfloat)cn->rotation[2];
                gn->rotation[3] = (GLfloat)cn->rotation[3];
            }
            if (cn->has_scale)
            {
                gn->scale[0] = (GLfloat)cn->scale[0];
                gn->scale[1] = (GLfloat)cn->scale[1];
                gn->scale[2] = (GLfloat)cn->scale[2];
            }
        }

        gn->childCount = (GLint)cn->children_count;
        if (gn->childCount > 0)
        {
            gn->childIndices = (GLint*)malloc(sizeof(GLint) * (size_t)gn->childCount);
            for (ci = 0; ci < gn->childCount; ci++)
            {
                gn->childIndices[ci] = (GLint)(cn->children[ci] - data->nodes);
            }
        }

        if (cn->mesh)
        {
            gn->meshIndex = (GLint)(cn->mesh - data->meshes);
        }
        if (cn->skin)
        {
            gn->skinIndex = (GLint)(cn->skin - data->skins);
        }
        if (cn->camera)
        {
            gn->cameraIndex = (GLint)(cn->camera - data->cameras);
        }
    }

    for (ni = 0; ni < scene->nodeCount; ni++)
    {
        GLUSgltfNode* gn = &scene->nodes[ni];
        for (ci = 0; ci < gn->childCount; ci++)
        {
            GLint c = gn->childIndices[ci];
            if (c >= 0 && c < scene->nodeCount)
            {
                scene->nodes[c].parentIndex = ni;
            }
        }
    }
}

static GLUSvoid gltfBuildRootNodes(GLUSgltfScene* scene, GLint sceneIndex)
{
    cgltf_data*  data = scene->cgltfData;
    cgltf_scene* sc = NULL;
    GLint        ni;

    if (sceneIndex >= 0 && sceneIndex < (GLint)data->scenes_count)
    {
        sc = &data->scenes[sceneIndex];
    }
    else if (data->scene)
    {
        sc = data->scene;
    }

    scene->rootNodeCount = 0;
    if (sc)
    {
        scene->rootNodes = (GLint*)malloc(sizeof(GLint) * (sc->nodes_count > 0 ? sc->nodes_count : 1));
        for (ni = 0; ni < (GLint)sc->nodes_count; ni++)
        {
            scene->rootNodes[scene->rootNodeCount++] = (GLint)(sc->nodes[ni] - data->nodes);
        }
    }
    else
    {
        GLint cap = scene->nodeCount > 0 ? scene->nodeCount : 1;
        scene->rootNodes = (GLint*)malloc(sizeof(GLint) * (size_t)cap);
        for (ni = 0; ni < scene->nodeCount; ni++)
        {
            if (scene->nodes[ni].parentIndex == -1)
            {
                scene->rootNodes[scene->rootNodeCount++] = ni;
            }
        }
    }
}

static GLUSvoid gltfBuildSkins(GLUSgltfScene* scene)
{
    cgltf_data* data = scene->cgltfData;
    GLint       si, ji;

    scene->skinCount = (GLint)data->skins_count;
    if (scene->skinCount <= 0)
    {
        return;
    }
    scene->skins = (GLUSgltfSkin*)calloc((size_t)scene->skinCount, sizeof(GLUSgltfSkin));

    for (si = 0; si < scene->skinCount; si++)
    {
        cgltf_skin*   cs = &data->skins[si];
        GLUSgltfSkin* gs = &scene->skins[si];

        gs->jointCount = (GLint)cs->joints_count;
        if (gs->jointCount > GLUS_GLTF_MAX_JOINTS)
        {
            glusLogPrint(GLUS_LOG_WARNING, "glTF: skin %d has %d joints, clamping to %d", si, gs->jointCount, GLUS_GLTF_MAX_JOINTS);
            gs->jointCount = GLUS_GLTF_MAX_JOINTS;
        }
        if (gs->jointCount <= 0)
        {
            continue;
        }
        gs->jointNodeIndices    = (GLint*)malloc(sizeof(GLint) * (size_t)gs->jointCount);
        gs->inverseBindMatrices = (GLfloat*)malloc(sizeof(GLfloat) * (size_t)gs->jointCount * 16);
        gs->jointMatrices       = (GLfloat*)malloc(sizeof(GLfloat) * (size_t)gs->jointCount * 16);
        for (ji = 0; ji < gs->jointCount; ji++)
        {
            gs->jointNodeIndices[ji] = (GLint)(cs->joints[ji] - data->nodes);
            glusMatrix4x4Identityf(&gs->inverseBindMatrices[ji * 16]);
            glusMatrix4x4Identityf(&gs->jointMatrices[ji * 16]);
        }
        if (cs->inverse_bind_matrices)
        {
            GLint   rc = (GLint)cs->inverse_bind_matrices->count;
            GLfloat* ibm;
            if (rc > gs->jointCount)
            {
                rc = gs->jointCount;
            }
            ibm = gltfReadAccessorFloats(cs->inverse_bind_matrices, 16);
            if (ibm)
            {
                memcpy(gs->inverseBindMatrices, ibm, (size_t)rc * 16 * sizeof(GLfloat));
                free(ibm);
            }
        }
    }
}

static GLUSvoid gltfBuildAnimations(GLUSgltfScene* scene)
{
    cgltf_data* data = scene->cgltfData;
    GLint       ai, chi;

    scene->animationCount = (GLint)data->animations_count;
    if (scene->animationCount <= 0)
    {
        return;
    }
    scene->animations = (GLUSgltfAnimation*)calloc((size_t)scene->animationCount, sizeof(GLUSgltfAnimation));

    for (ai = 0; ai < scene->animationCount; ai++)
    {
        cgltf_animation*   ca = &data->animations[ai];
        GLUSgltfAnimation* ga = &scene->animations[ai];

        ga->name        = gltfCopyString(ca->name);
        ga->channels    = (GLUSgltfAnimChannel*)calloc(ca->channels_count > 0 ? ca->channels_count : 1, sizeof(GLUSgltfAnimChannel));
        ga->channelCount = 0;
        ga->startTime   = 1e30f;
        ga->endTime     = -1e30f;

        for (chi = 0; chi < (GLint)ca->channels_count; chi++)
        {
            cgltf_animation_channel*  ch      = &ca->channels[chi];
            cgltf_animation_sampler*  sampler = ch->sampler;
            GLUSgltfAnimChannel*      ac;
            GLint                     path, interp, comp;

            if (!ch->target_node)
            {
                continue;
            }
            switch (ch->target_path)
            {
            case cgltf_animation_path_type_translation:
                path = GLUS_GLTF_PATH_TRANSLATION;
                comp = 3;
                break;
            case cgltf_animation_path_type_rotation:
                path = GLUS_GLTF_PATH_ROTATION;
                comp = 4;
                break;
            case cgltf_animation_path_type_scale:
                path = GLUS_GLTF_PATH_SCALE;
                comp = 3;
                break;
            case cgltf_animation_path_type_weights:
                /* Morph-weight track; values are flattened scalars
                 * (numTargets per keyframe), interpreted at sample time. */
                path = GLUS_GLTF_PATH_WEIGHTS;
                comp = 1;
                break;
            default:
                continue;
            }
            switch (sampler->interpolation)
            {
            case cgltf_interpolation_type_step:
                interp = GLUS_ANIMATION_STEP;
                break;
            case cgltf_interpolation_type_cubic_spline:
                interp = GLUS_ANIMATION_CUBICSPLINE;
                break;
            default:
                interp = GLUS_ANIMATION_LINEAR;
                break;
            }

            ac              = &ga->channels[ga->channelCount++];
            ac->nodeIndex   = (GLint)(ch->target_node - data->nodes);
            ac->path        = path;
            ac->interpolation = interp;
            ac->keyframeCount = (GLint)sampler->input->count;
            ac->componentCount = comp;

            ac->times  = gltfReadAccessorFloats(sampler->input, 1);
            ac->values = gltfReadAccessorFloats(sampler->output, comp);
            if (!ac->times || !ac->values)
            {
                free(ac->times);
                free(ac->values);
                ac->times  = NULL;
                ac->values = NULL;
                ga->channelCount--;
                continue;
            }

            if (ac->keyframeCount > 0)
            {
                if (ac->times[0] < ga->startTime)
                {
                    ga->startTime = ac->times[0];
                }
                if (ac->times[ac->keyframeCount - 1] > ga->endTime)
                {
                    ga->endTime = ac->times[ac->keyframeCount - 1];
                }
            }
        }

        if (ga->endTime < ga->startTime)
        {
            ga->startTime = 0.0f;
            ga->endTime   = 0.0f;
        }
        ga->duration = ga->endTime - ga->startTime;
    }
}

static GLUSvoid gltfBuildCameras(GLUSgltfScene* scene)
{
    cgltf_data* data = scene->cgltfData;
    GLint       ci;

    scene->cameraCount = (GLint)data->cameras_count;
    if (scene->cameraCount <= 0)
    {
        return;
    }
    scene->cameras = (GLUSgltfCamera*)calloc((size_t)scene->cameraCount, sizeof(GLUSgltfCamera));

    for (ci = 0; ci < scene->cameraCount; ci++)
    {
        cgltf_camera*   cc = &data->cameras[ci];
        GLUSgltfCamera* gc = &scene->cameras[ci];
        gc->nodeIndex = -1;

        if (cc->type == cgltf_camera_type_perspective)
        {
            gc->type        = GLUS_GLTF_CAMERA_PERSPECTIVE;
            gc->aspectRatio = cc->data.perspective.has_aspect_ratio ? (GLfloat)cc->data.perspective.aspect_ratio : 0.0f;
            gc->yfov        = (GLfloat)cc->data.perspective.yfov;
            gc->znear       = (GLfloat)cc->data.perspective.znear;
            gc->zfar        = cc->data.perspective.has_zfar ? (GLfloat)cc->data.perspective.zfar : 0.0f;
        }
        else if (cc->type == cgltf_camera_type_orthographic)
        {
            gc->type  = GLUS_GLTF_CAMERA_ORTHOGRAPHIC;
            gc->xmag  = (GLfloat)cc->data.orthographic.xmag;
            gc->ymag  = (GLfloat)cc->data.orthographic.ymag;
            gc->znear = (GLfloat)cc->data.orthographic.znear;
            gc->zfar  = (GLfloat)cc->data.orthographic.zfar;
        }
        else
        {
            gc->type = GLUS_GLTF_CAMERA_NONE;
        }
    }

    for (ci = 0; ci < scene->nodeCount; ci++)
    {
        if (scene->nodes[ci].cameraIndex >= 0 && scene->nodes[ci].cameraIndex < scene->cameraCount)
        {
            scene->cameras[scene->nodes[ci].cameraIndex].nodeIndex = ci;
        }
    }
}

static GLUSvoid gltfComputeWorldMatrix(GLUSgltfScene* scene, GLint nodeIndex, const GLfloat parentWorld[16])
{
    GLUSgltfNode* gn = &scene->nodes[nodeIndex];
    GLfloat       local[16];
    GLfloat       T[16], R[16], S[16], RS[16], q[4];
    GLint         ci;

    if (gn->hasMatrix)
    {
        memcpy(local, gn->localMatrix, sizeof(local));
    }
    else
    {
        glusMatrix4x4Identityf(T);
        T[12] = gn->translation[0];
        T[13] = gn->translation[1];
        T[14] = gn->translation[2];

        q[0] = gn->rotation[0];
        q[1] = gn->rotation[1];
        q[2] = gn->rotation[2];
        q[3] = gn->rotation[3];
        glusQuaternionGetMatrix4x4f(R, q);

        glusMatrix4x4Identityf(S);
        S[0]  = gn->scale[0];
        S[5]  = gn->scale[1];
        S[10] = gn->scale[2];

        glusMatrix4x4Multiplyf(RS, R, S);
        glusMatrix4x4Multiplyf(local, T, RS);
    }

    glusMatrix4x4Multiplyf(gn->worldMatrix, parentWorld, local);

    for (ci = 0; ci < gn->childCount; ci++)
    {
        gltfComputeWorldMatrix(scene, gn->childIndices[ci], gn->worldMatrix);
    }
}

static GLUSvoid gltfComputeJointMatrices(GLUSgltfScene* scene)
{
    GLint si, ji;
    for (si = 0; si < scene->skinCount; si++)
    {
        GLUSgltfSkin* gs = &scene->skins[si];
        for (ji = 0; ji < gs->jointCount; ji++)
        {
            GLint jni = gs->jointNodeIndices[ji];
            if (jni >= 0 && jni < scene->nodeCount)
            {
                glusMatrix4x4Multiplyf(&gs->jointMatrices[ji * 16], scene->nodes[jni].worldMatrix, &gs->inverseBindMatrices[ji * 16]);
            }
        }
    }
}

static GLUSvoid gltfRefreshPrimitiveTransforms(GLUSgltfScene* scene)
{
    GLint i;
    for (i = 0; i < scene->primitiveCount; i++)
    {
        GLUSgltfPrimitive* gp = &scene->primitives[i];
        GLfloat            tmp[16];

        if (gp->skinIndex >= 0)
        {
            continue;
        }
        if (gp->nodeIndex >= 0)
        {
            memcpy(gp->modelMatrix, scene->nodes[gp->nodeIndex].worldMatrix, sizeof(gp->modelMatrix));
        }
        memcpy(tmp, gp->modelMatrix, sizeof(tmp));
        glusMatrix4x4Inversef(tmp);
        glusMatrix4x4Transposef(tmp);
        glusMatrix4x4ExtractMatrix3x3f(gp->normalMatrix, tmp);
    }
}

/* Sample one morph-weight sub-track (target `target`) from a flattened weights
 * channel. numTargets is the number of morph targets per keyframe. */
static GLUSfloat gltfSampleWeight(const GLUSgltfAnimChannel* ac, GLint numTargets, GLint target, GLUSfloat t)
{
    GLint    n    = ac->keyframeCount;
    GLUSboolean cub = (ac->interpolation == GLUS_ANIMATION_CUBICSPLINE) ? GLUS_TRUE : GLUS_FALSE;
    GLint    stride = cub ? 3 * numTargets : numTargets;
    GLint    i;
    GLUSfloat u, v0, v1;

    if (n <= 0 || numTargets <= 0)
    {
        return 0.0f;
    }
    /* For CUBICSPLINE the vertex value sits at slot 1 (in, vertex, out). */
#define GLTF_WEIGHT(k, slot) ac->values[(k) * stride + (cub ? (slot) * numTargets : 0) + (target)]

    if (n == 1 || t <= ac->times[0])
    {
        return GLTF_WEIGHT(0, 1);
    }
    if (t >= ac->times[n - 1])
    {
        return GLTF_WEIGHT(n - 1, 1);
    }
    for (i = 0; i < n - 1; i++)
    {
        if (t < ac->times[i + 1])
        {
            break;
        }
    }
    if (ac->interpolation == GLUS_ANIMATION_STEP)
    {
        return GLTF_WEIGHT(i, 1);
    }
    u = (t - ac->times[i]) / (ac->times[i + 1] - ac->times[i]);
    v0 = GLTF_WEIGHT(i, 1);
    v1 = GLTF_WEIGHT(i + 1, 1);
    if (cub)
    {
        GLUSfloat delta = ac->times[i + 1] - ac->times[i];
        GLUSfloat m0    = GLTF_WEIGHT(i, 2);     /* out-tangent of i    */
        GLUSfloat m1    = GLTF_WEIGHT(i + 1, 0); /* in-tangent of i + 1 */
        return glusMathCubicHermitef(v0, delta * m0, v1, delta * m1, u);
    }
    return v0 + (v1 - v0) * u;
#undef GLTF_WEIGHT
}

static GLUSvoid gltfApplyAnimation(GLUSgltfScene* scene)
{
    GLUSgltfAnimation* ga;
    GLint              ci;

    if (scene->activeAnimation < 0 || scene->activeAnimation >= scene->animationCount)
    {
        return;
    }
    ga = &scene->animations[scene->activeAnimation];
    for (ci = 0; ci < ga->channelCount; ci++)
    {
        GLUSgltfAnimChannel* ac = &ga->channels[ci];
        GLUSgltfNode*        gn;

        if (ac->nodeIndex < 0 || ac->nodeIndex >= scene->nodeCount)
        {
            continue;
        }
        gn = &scene->nodes[ac->nodeIndex];
        switch (ac->path)
        {
        case GLUS_GLTF_PATH_TRANSLATION:
            glusAnimationSampleVec3f(gn->translation, ac->times, ac->values, ac->keyframeCount, ac->interpolation, scene->animationTime);
            break;
        case GLUS_GLTF_PATH_ROTATION:
            glusAnimationSampleQuaternionf(gn->rotation, ac->times, ac->values, ac->keyframeCount, ac->interpolation, scene->animationTime);
            break;
        case GLUS_GLTF_PATH_SCALE:
            glusAnimationSampleVec3f(gn->scale, ac->times, ac->values, ac->keyframeCount, ac->interpolation, scene->animationTime);
            break;
        case GLUS_GLTF_PATH_WEIGHTS:
        {
            GLint pi;
            for (pi = 0; pi < scene->primitiveCount; pi++)
            {
                GLUSgltfPrimitive* gp = &scene->primitives[pi];
                if (gp->nodeIndex == ac->nodeIndex && gp->morphTargetCount > 0 && gp->morphWeights)
                {
                    GLint tg;
                    for (tg = 0; tg < gp->morphTargetCount; tg++)
                    {
                        gp->morphWeights[tg] = gltfSampleWeight(ac, gp->morphTargetCount, tg, scene->animationTime);
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

GLUSAPI GLUSboolean GLUSAPIENTRY glusGltfLoadSceneWith(const GLUSchar* filename, const GLUSgltfLoadOptions* options, GLUSgltfScene* scene)
{
    cgltf_options       gltfOptions;
    cgltf_data*         data = NULL;
    cgltf_result        res;
    GLUSgltfLoadContext ctx;
    GLint               sceneIndex;
    GLUSboolean         sRGB;
    GLUSboolean         uploadMeshes;
    GLint               cursor = 0;
    GLint               ni;
    GLint               totalPrimitives = 0;
    GLint               imgCap;
    GLint               i;
    GLfloat             dx, dy, dz;

    if (!filename || !scene)
    {
        return GLUS_FALSE;
    }
    memset(scene, 0, sizeof(*scene));
    scene->activeAnimation = -1;

    sceneIndex   = options ? options->sceneIndex : -1;
    sRGB         = options ? options->sRGBColorTextures : GLUS_TRUE;
    uploadMeshes = options ? options->uploadMeshes : GLUS_TRUE;

    memset(&gltfOptions, 0, sizeof(gltfOptions));
    res = cgltf_parse_file(&gltfOptions, filename, &data);
    if (res != cgltf_result_success)
    {
        glusLogPrint(GLUS_LOG_ERROR, "glTF: cgltf_parse_file failed (%d) for '%s'", res, filename);
        return GLUS_FALSE;
    }
    res = cgltf_load_buffers(&gltfOptions, data, filename);
    if (res != cgltf_result_success)
    {
        glusLogPrint(GLUS_LOG_ERROR, "glTF: cgltf_load_buffers failed (%d)", res);
        cgltf_free(data);
        return GLUS_FALSE;
    }
    scene->cgltfData = data;

    /* GLUS implements the glTF 2.0 core only. Warn about any required
     * extension so unsupported assets (Draco compression, BasisU textures,
     * specular-glossiness, ...) do not fail silently with garbage geometry. */
    for (i = 0; i < (GLint)data->extensions_required_count; i++)
    {
        glusLogPrint(GLUS_LOG_WARNING, "glTF: required extension '%s' is not supported; the asset may not render correctly.",
                     data->extensions_required[i]);
    }

    scene->basePath = (GLUSchar*)malloc(1024);
    gltfDeriveBasePath(filename, scene->basePath, 1024);

    scene->sceneMin[0] = scene->sceneMin[1] = scene->sceneMin[2] = 1e30f;
    scene->sceneMax[0] = scene->sceneMax[1] = scene->sceneMax[2] = -1e30f;
    scene->sceneRadius                                            = 1.0f;

    gltfBuildNodes(scene);
    gltfBuildRootNodes(scene, sceneIndex);
    gltfBuildSkins(scene);
    gltfBuildAnimations(scene);
    gltfBuildCameras(scene);

    glusGltfUpdateTransforms(scene);

    if (uploadMeshes)
    {
        scene->defaultWhiteTexture  = gltfCreateTexture1x1(255, 255, 255, 255);
        scene->defaultNormalTexture = gltfCreateTexture1x1(128, 128, 255, 255);

        for (ni = 0; ni < scene->nodeCount; ni++)
        {
            if (scene->nodes[ni].meshIndex >= 0)
            {
                totalPrimitives += (GLint)data->meshes[scene->nodes[ni].meshIndex].primitives_count;
            }
        }
        if (totalPrimitives > 0)
        {
            scene->primitives = (GLUSgltfPrimitive*)calloc((size_t)totalPrimitives, sizeof(GLUSgltfPrimitive));
        }

        ctx.scene      = scene;
        ctx.basePath   = scene->basePath;
        ctx.sRGB       = sRGB;
        imgCap         = (GLint)data->images_count > 0 ? (GLint)data->images_count : 1;
        ctx.cache      = (GLUSgltfImageCacheEntry*)calloc((size_t)imgCap, sizeof(GLUSgltfImageCacheEntry));
        ctx.cacheCount = 0;
        ctx.cacheCap   = imgCap;

        for (ni = 0; ni < scene->nodeCount; ni++)
        {
            if (scene->nodes[ni].meshIndex >= 0)
            {
                gltfProcessNodeMeshes(&ctx, ni, &cursor);
            }
        }
        scene->primitiveCount = cursor;

        if (ctx.cacheCount > 0)
        {
            scene->textures = (GLuint*)malloc(sizeof(GLuint) * (size_t)ctx.cacheCount);
            for (i = 0; i < ctx.cacheCount; i++)
            {
                scene->textures[i] = ctx.cache[i].texture;
            }
            scene->textureCount = ctx.cacheCount;
        }
        free(ctx.cache);

        if (scene->primitiveCount > 0)
        {
            scene->sceneCenter[0] = (scene->sceneMin[0] + scene->sceneMax[0]) * 0.5f;
            scene->sceneCenter[1] = (scene->sceneMin[1] + scene->sceneMax[1]) * 0.5f;
            scene->sceneCenter[2] = (scene->sceneMin[2] + scene->sceneMax[2]) * 0.5f;
            dx                    = scene->sceneMax[0] - scene->sceneMin[0];
            dy                    = scene->sceneMax[1] - scene->sceneMin[1];
            dz                    = scene->sceneMax[2] - scene->sceneMin[2];
            scene->sceneRadius    = sqrtf(dx * dx + dy * dy + dz * dz) * 0.5f;
            if (scene->sceneRadius < 0.01f)
            {
                scene->sceneRadius = 1.0f;
            }
        }
    }

    if (scene->animationCount > 0)
    {
        glusGltfSetActiveAnimation(scene, 0);
    }

    glusLogPrint(GLUS_LOG_INFO, "glTF: loaded '%s' (%d nodes, %d primitives, %d textures, %d animations, %d cameras)",
                 filename, scene->nodeCount, scene->primitiveCount, scene->textureCount, scene->animationCount, scene->cameraCount);

    return GLUS_TRUE;
}

GLUSAPI GLUSboolean GLUSAPIENTRY glusGltfLoadScene(const GLUSchar* filename, GLUSgltfScene* scene)
{
    return glusGltfLoadSceneWith(filename, NULL, scene);
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfDestroyScene(GLUSgltfScene* scene)
{
    GLint i;

    if (!scene)
    {
        return;
    }
    if (scene->primitives)
    {
        for (i = 0; i < scene->primitiveCount; i++)
        {
            GLUSgltfPrimitive* gp = &scene->primitives[i];
            if (gp->vao)
            {
                glDeleteVertexArrays(1, &gp->vao);
            }
            if (gp->vboPosition)
            {
                glDeleteBuffers(1, &gp->vboPosition);
            }
            if (gp->vboNormal)
            {
                glDeleteBuffers(1, &gp->vboNormal);
            }
            if (gp->vboTangent)
            {
                glDeleteBuffers(1, &gp->vboTangent);
            }
            if (gp->vboTexCoord0)
            {
                glDeleteBuffers(1, &gp->vboTexCoord0);
            }
            if (gp->vboTexCoord1)
            {
                glDeleteBuffers(1, &gp->vboTexCoord1);
            }
            if (gp->vboColor)
            {
                glDeleteBuffers(1, &gp->vboColor);
            }
            if (gp->vboJoints)
            {
                glDeleteBuffers(1, &gp->vboJoints);
            }
            if (gp->vboWeights)
            {
                glDeleteBuffers(1, &gp->vboWeights);
            }
            if (gp->ibo)
            {
                glDeleteBuffers(1, &gp->ibo);
            }
            if (gp->morphPositionSSBO)
            {
                glDeleteBuffers(1, &gp->morphPositionSSBO);
            }
            if (gp->morphNormalSSBO)
            {
                glDeleteBuffers(1, &gp->morphNormalSSBO);
            }
            if (gp->morphTangentSSBO)
            {
                glDeleteBuffers(1, &gp->morphTangentSSBO);
            }
            free(gp->morphWeights);
        }
        free(scene->primitives);
    }
    if (scene->nodes)
    {
        for (i = 0; i < scene->nodeCount; i++)
        {
            free(scene->nodes[i].childIndices);
        }
        free(scene->nodes);
    }
    free(scene->rootNodes);
    if (scene->skins)
    {
        for (i = 0; i < scene->skinCount; i++)
        {
            free(scene->skins[i].jointNodeIndices);
            free(scene->skins[i].inverseBindMatrices);
            free(scene->skins[i].jointMatrices);
        }
        free(scene->skins);
    }
    if (scene->animations)
    {
        GLint ci;
        for (i = 0; i < scene->animationCount; i++)
        {
            free(scene->animations[i].name);
            for (ci = 0; ci < scene->animations[i].channelCount; ci++)
            {
                free(scene->animations[i].channels[ci].times);
                free(scene->animations[i].channels[ci].values);
            }
            free(scene->animations[i].channels);
        }
        free(scene->animations);
    }
    free(scene->cameras);
    if (scene->textures)
    {
        for (i = 0; i < scene->textureCount; i++)
        {
            glDeleteTextures(1, &scene->textures[i]);
        }
        free(scene->textures);
    }
    if (scene->defaultWhiteTexture)
    {
        glDeleteTextures(1, &scene->defaultWhiteTexture);
    }
    if (scene->defaultNormalTexture)
    {
        glDeleteTextures(1, &scene->defaultNormalTexture);
    }
    free(scene->basePath);
    if (scene->cgltfData)
    {
        cgltf_free(scene->cgltfData);
    }
    memset(scene, 0, sizeof(*scene));
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfUpdateTransforms(GLUSgltfScene* scene)
{
    GLfloat identity[16];
    GLint   ri;

    if (!scene)
    {
        return;
    }
    glusMatrix4x4Identityf(identity);
    for (ri = 0; ri < scene->rootNodeCount; ri++)
    {
        gltfComputeWorldMatrix(scene, scene->rootNodes[ri], identity);
    }
    gltfComputeJointMatrices(scene);
    gltfRefreshPrimitiveTransforms(scene);
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfCameraMatrices(const GLUSgltfScene* scene, GLUSint cameraIndex, GLUSfloat viewportAspect, GLUSfloat viewOut[16], GLUSfloat projOut[16])
{
    const GLUSgltfCamera* cam;

    if (viewOut)
    {
        glusMatrix4x4Identityf(viewOut);
    }
    if (projOut)
    {
        glusMatrix4x4Identityf(projOut);
    }
    if (!scene || cameraIndex < 0 || cameraIndex >= scene->cameraCount)
    {
        return;
    }
    cam = &scene->cameras[cameraIndex];

    if (viewOut)
    {
        if (cam->nodeIndex >= 0 && cam->nodeIndex < scene->nodeCount)
        {
            memcpy(viewOut, scene->nodes[cam->nodeIndex].worldMatrix, 16 * sizeof(GLUSfloat));
            glusMatrix4x4Inversef(viewOut);
        }
        else
        {
            glusMatrix4x4Identityf(viewOut);
        }
    }

    if (projOut)
    {
        if (cam->type == GLUS_GLTF_CAMERA_PERSPECTIVE)
        {
            GLUSfloat aspect = cam->aspectRatio > 0.0f ? cam->aspectRatio : (viewportAspect > 0.0f ? viewportAspect : 1.0f);
            GLUSfloat farv   = cam->zfar > 0.0f ? cam->zfar : cam->znear * 1e5f;
            glusMatrix4x4Perspectivef(projOut, cam->yfov, aspect, cam->znear, farv);
        }
        else if (cam->type == GLUS_GLTF_CAMERA_ORTHOGRAPHIC)
        {
            glusMatrix4x4Identityf(projOut);
            if (cam->xmag != 0.0f)
            {
                projOut[0] = 1.0f / cam->xmag;
            }
            if (cam->ymag != 0.0f)
            {
                projOut[5] = 1.0f / cam->ymag;
            }
            if (cam->zfar > cam->znear)
            {
                projOut[10] = -2.0f / (cam->zfar - cam->znear);
                projOut[14] = -(cam->zfar + cam->znear) / (cam->zfar - cam->znear);
            }
            else
            {
                projOut[10] = -1.0f;
                projOut[14] = -cam->znear;
            }
            projOut[15] = 1.0f;
        }
    }
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfSetActiveAnimation(GLUSgltfScene* scene, GLUSint animIndex)
{
    if (!scene)
    {
        return;
    }
    scene->activeAnimation = animIndex;
    if (animIndex >= 0 && animIndex < scene->animationCount)
    {
        scene->animationTime = scene->animations[animIndex].startTime;
    }
    else
    {
        scene->animationTime = 0.0f;
    }
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfSetAnimationTime(GLUSgltfScene* scene, GLUSfloat time)
{
    if (!scene)
    {
        return;
    }
    scene->animationTime = time;
    gltfApplyAnimation(scene);
    glusGltfUpdateTransforms(scene);
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfUpdateAnimation(GLUSgltfScene* scene, GLUSfloat deltaTime)
{
    if (!scene)
    {
        return;
    }
    if (scene->activeAnimation >= 0 && scene->activeAnimation < scene->animationCount)
    {
        GLUSgltfAnimation* ga = &scene->animations[scene->activeAnimation];
        scene->animationTime += deltaTime;
        if (ga->duration > 0.0f && scene->animationTime > ga->endTime)
        {
            scene->animationTime = ga->startTime + fmodf(scene->animationTime - ga->startTime, ga->duration);
        }
        gltfApplyAnimation(scene);
    }
    glusGltfUpdateTransforms(scene);
}

GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfDrawPrimitive(const GLUSgltfScene* scene, GLUSint primitiveIndex)
{
    const GLUSgltfPrimitive* gp;

    if (!scene || primitiveIndex < 0 || primitiveIndex >= scene->primitiveCount)
    {
        return;
    }
    gp = &scene->primitives[primitiveIndex];
    glBindVertexArray(gp->vao);
    if (gp->ibo)
    {
        glDrawElements(gp->mode, gp->indexCount, gp->indexType, 0);
    }
    else
    {
        glDrawArrays(gp->mode, 0, gp->vertexCount);
    }
    glBindVertexArray(0);
}

GLUSAPI struct cgltf_data* GLUSAPIENTRY glusGltfGetCgltfData(const GLUSgltfScene* scene)
{
    if (!scene)
    {
        return NULL;
    }
    return scene->cgltfData;
}

GLUSAPI const GLUSchar* GLUSAPIENTRY glusGltfGetBasePath(const GLUSgltfScene* scene)
{
    if (!scene || !scene->basePath)
    {
        return "";
    }
    return scene->basePath;
}
