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

#ifndef GLUS_GLTF_H_
#define GLUS_GLTF_H_

/**
 * glTF 2.0 core loader for GLUS.
 *
 * Parses a glTF 2.0 asset (.glb or .gltf) using cgltf and uploads it to OpenGL,
 * producing render-ready primitives that match the GLUS glTF PBR shaders
 * (../shader/glus_gltf_pbr.vert.glsl, glus_gltf_pbr.frag.glsl and the skinned
 * variant). Only the glTF 2.0 core specification is handled here; Khronos /
 * vendor extensions (KHR_gaussian_splatting, KHR_materials_*, etc.) are NOT
 * interpreted by GLUS. The parsed cgltf tree is kept alive for the lifetime of
 * the scene and is reachable via glusGltfGetCgltfData(), so each application
 * can read whatever extension data it needs itself.
 *
 * The application owns the shader programs and the IBL / tonemap state. GLUS
 * provides: scene loading, per-frame animation / skin update, and the VAO +
 * draw call per primitive. The material factors and texture names live on each
 * primitive so the application can bind them to its own uniforms.
 */

/**
 * Material alpha mode: the rendered output is fully opaque and the alpha
 * channel is ignored.
 */
#define GLUS_GLTF_ALPHA_OPAQUE 0

/**
 * Material alpha mode: fragments are discarded when their alpha is below
 * GLUSgltfMaterial::alphaCutoff.
 */
#define GLUS_GLTF_ALPHA_MASK 1

/**
 * Material alpha mode: fragments are blended using their alpha value.
 */
#define GLUS_GLTF_ALPHA_BLEND 2

/**
 * Animation target path: node translation (vec3).
 */
#define GLUS_GLTF_PATH_TRANSLATION 0

/**
 * Animation target path: node rotation (quaternion, xyzw).
 */
#define GLUS_GLTF_PATH_ROTATION 1

/**
 * Animation target path: node scale (vec3).
 */
#define GLUS_GLTF_PATH_SCALE 2

/**
 * Animation target path: morph target weights (scalar per target).
 */
#define GLUS_GLTF_PATH_WEIGHTS 3

/**
 * Parsed, uploaded material for a primitive. The values are pre-resolved from
 * the glTF pbrMetallicRoughness material and are ready to feed directly to the
 * GLUS glTF PBR fragment shader uniforms.
 */
typedef struct _GLUSgltfMaterial
{
    /* Texture names on texture units 0..4 (matching the shader). */
    GLuint baseColorTexture;
    GLuint metallicRoughnessTexture;
    GLuint normalTexture;
    GLuint occlusionTexture;
    GLuint emissiveTexture;

    GLfloat baseColorFactor[4];
    GLfloat metallicFactor;
    GLfloat roughnessFactor;
    GLfloat emissiveFactor[3];
    GLfloat occlusionStrength;
    GLfloat normalScale;
    GLfloat alphaCutoff;
    GLint   alphaMode;    /* GLUS_GLTF_ALPHA_* */
    GLint   doubleSided;
    GLint   hasNormalMap; /* GLUS_TRUE when a tangent stream + normal map exist */

    /* Texture coordinate set (0 or 1) each texture info references. */
    GLint   baseColorTexCoordSet;
    GLint   metallicRoughnessTexCoordSet;
    GLint   normalTexCoordSet;
    GLint   occlusionTexCoordSet;
    GLint   emissiveTexCoordSet;
} GLUSgltfMaterial;

/**
 * A single renderable primitive. The VAO binds attribute streams that match the
 * GLUS glTF PBR vertex shaders:
 *   location 0 = vec3 position
 *   location 1 = vec3 normal
 *   location 2 = vec4 tangent (xyz + handedness w)
 *   location 3 = vec2 texCoord0
 *   location 4 = vec4 joints0  (skinned primitives only)
 *   location 5 = vec4 weights0 (skinned primitives only)
 */
typedef struct _GLUSgltfPrimitive
{
    /* Geometry. */
    GLuint  vao;
    GLuint  vboPosition;
    GLuint  vboNormal;
    GLuint  vboTangent;
    GLuint  vboTexCoord0;
    GLuint  vboJoints;   /* 0 when not skinned */
    GLuint  vboWeights;  /* 0 when not skinned */
    GLuint  vboTexCoord1; /* 0 when TEXCOORD_1 is absent */
    GLuint  vboColor;     /* 0 when COLOR_0 is absent */
    GLuint  ibo;         /* 0 when non-indexed */
    GLsizei vertexCount; /* used when ibo == 0 */
    GLsizei indexCount;  /* used when ibo != 0 */
    GLenum  indexType;   /* GL_UNSIGNED_INT, GL_UNSIGNED_SHORT, ... */
    GLenum  mode;        /* GL_TRIANGLES, GL_POINTS, ... (core glTF modes) */

    /* Material. */
    GLUSgltfMaterial material;

    /* Transform linkage. */
    GLint   nodeIndex; /* owning node, -1 if none */
    GLint   skinIndex; /* -1 = not skinned, else index into GLUSgltfScene::skins */

    /* Static model + normal matrices; updated each frame for animated, non-skinned primitives. */
    GLfloat modelMatrix[16];
    GLfloat normalMatrix[9];

    /* Morph targets (core). Per-target deltas are packed into SSBOs and blended
     * in the morph vertex shader; weights are animated each frame. */
    GLint    morphTargetCount;
    GLuint   morphPositionSSBO; /* 0 if the primitive has no morph targets */
    GLuint   morphNormalSSBO;   /* 0 if no normal deltas */
    GLuint   morphTangentSSBO;  /* 0 if no tangent deltas */
    GLfloat* morphWeights;      /* [morphTargetCount], current weights */
} GLUSgltfPrimitive;

/**
 * A scene-graph node with local TRS (or an explicit matrix) and a recomputed
 * world matrix. Parent / child relationships use indices into
 * GLUSgltfScene::nodes.
 */
typedef struct _GLUSgltfNode
{
    GLfloat translation[3];
    GLfloat rotation[4]; /* xyzw quaternion */
    GLfloat scale[3];
    GLint   hasMatrix;
    GLfloat localMatrix[16];  /* used when hasMatrix == GLUS_TRUE */
    GLfloat worldMatrix[16];  /* recomputed every frame */

    GLint  parentIndex;
    GLint* childIndices;
    GLint  childCount;

    /* Back-references, -1 when absent. */
    GLint meshIndex;   /* index of the node's mesh in the parsed tree */
    GLint skinIndex;   /* skin used by this node's mesh, -1 if not skinned */
    GLint cameraIndex; /* index into GLUSgltfScene::cameras */
} GLUSgltfNode;

/**
 * A skin: a list of joint nodes and their inverse bind matrices, plus the
 * per-frame joint matrices (jointMatrix[j] = worldMatrix(joint[j]) * IBM[j]).
 */
typedef struct _GLUSgltfSkin
{
    GLint    jointCount;
    GLint*   jointNodeIndices;     /* [jointCount] */
    GLfloat* inverseBindMatrices;  /* [jointCount * 16] */
    GLfloat* jointMatrices;        /* [jointCount * 16], recomputed every frame */
} GLUSgltfSkin;

/**
 * One animation channel: a sampler bound to a node path. Times and values are
 * owned by the channel. Interpolation uses the GLUS_ANIMATION_* constants from
 * glus_animation.h so glusAnimationSampleVec3f / glusAnimationSampleQuaternionf
 * can consume the arrays directly.
 */
typedef struct _GLUSgltfAnimChannel
{
    GLint    nodeIndex;
    GLint    path;          /* GLUS_GLTF_PATH_* */
    GLint    interpolation; /* GLUS_ANIMATION_STEP / LINEAR / CUBICSPLINE */
    GLint    keyframeCount;
    GLfloat* times;         /* [keyframeCount] */
    GLfloat* values;        /* layout depends on path + interpolation */
    GLint    componentCount;
} GLUSgltfAnimChannel;

/**
 * A named glTF animation (clip). All channels of the clip are sampled together.
 */
typedef struct _GLUSgltfAnimation
{
    GLUSchar* name;
    GLUSgltfAnimChannel* channels;
    GLint      channelCount;
    GLfloat    startTime;
    GLfloat    endTime;
    GLfloat    duration;
} GLUSgltfAnimation;

/**
 * glTF camera (perspective or orthographic), attached to a node.
 */
typedef struct _GLUSgltfCamera
{
    GLint type; /* GLUS_GLTF_CAMERA_PERSPECTIVE or _ORTHOGRAPHIC */
    GLint nodeIndex;

    /* Perspective members. */
    GLfloat aspectRatio; /* 0.0 => use the viewport aspect ratio */
    GLfloat yfov;        /* vertical field of view, radians */
    GLfloat znear;
    GLfloat zfar;        /* 0.0 => infinite far plane */

    /* Orthographic members. */
    GLfloat xmag;
    GLfloat ymag;
} GLUSgltfCamera;

#define GLUS_GLTF_CAMERA_NONE 0
#define GLUS_GLTF_CAMERA_PERSPECTIVE 1
#define GLUS_GLTF_CAMERA_ORTHOGRAPHIC 2

/**
 * Options for glusGltfLoadSceneWith(). Pass NULL to glusGltfLoadScene() for the
 * defaults shown below.
 */
typedef struct _GLUSgltfLoadOptions
{
    GLUSint     sceneIndex;        /* -1 => use asset.scene, else explicit scene */
    GLUSboolean sRGBColorTextures; /* GLUS_TRUE => base colour + emissive loaded sRGB */
    /**
     * GLUS_TRUE (default) uploads every triangle primitive to OpenGL and loads
     * material textures. Set GLUS_FALSE for assets whose primitives are driven
     * by an extension the application handles itself (e.g. KHR_gaussian_splatting):
     * GLUS still parses the file, loads the buffers and builds the node / skin /
     * animation / camera tables, and keeps the cgltf tree reachable via
     * glusGltfGetCgltfData(), but creates no mesh or texture GPU objects.
     */
    GLUSboolean uploadMeshes;
} GLUSgltfLoadOptions;

/**
 * A loaded glTF 2.0 scene. All arrays are heap allocated and freed by
 * glusGltfDestroyScene().
 */
typedef struct _GLUSgltfScene
{
    /* Parsed tree kept alive for the extension hook (see glusGltfGetCgltfData). */
    struct cgltf_data* cgltfData;

    GLUSgltfNode*      nodes;
    GLint             nodeCount;
    GLint*            rootNodes;
    GLint             rootNodeCount;

    GLUSgltfPrimitive* primitives;
    GLint             primitiveCount;

    GLUSgltfSkin*     skins;
    GLint            skinCount;

    GLUSgltfAnimation* animations;
    GLint             animationCount;

    GLUSgltfCamera*   cameras;
    GLint            cameraCount;

    /* OpenGL texture names created for image-based materials, plus 1x1 fallbacks. */
    GLuint* textures;
    GLint   textureCount;
    GLuint  defaultWhiteTexture;
    GLuint  defaultNormalTexture;

    /* Axis-aligned bounding box of the loaded scene's meshes (world space). */
    GLfloat sceneMin[3];
    GLfloat sceneMax[3];
    GLfloat sceneCenter[3];
    GLfloat sceneRadius;

    /* Animation playback state. */
    GLint   activeAnimation; /* index into animations, -1 = none */
    GLfloat animationTime;

    /* Directory of the source file, used to resolve external image URIs. */
    GLUSchar* basePath;
} GLUSgltfScene;

/**
 * Load a glTF 2.0 asset (.glb or .gltf) using default options. Equivalent to
 * glusGltfLoadSceneWith() with sceneIndex = -1 and sRGB colour textures.
 *
 * @param filename Path to the .glb / .gltf file.
 * @param scene    Zero-initialised scene to fill. Freed with glusGltfDestroyScene().
 *
 * @return GLUS_TRUE on success.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusGltfLoadScene(const GLUSchar* filename, GLUSgltfScene* scene);

/**
 * Load a glTF 2.0 asset with explicit options.
 *
 * @param filename Path to the .glb / .gltf file.
 * @param options  Options, or NULL for defaults.
 * @param scene    Zero-initialised scene to fill. Freed with glusGltfDestroyScene().
 *
 * @return GLUS_TRUE on success.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusGltfLoadSceneWith(const GLUSchar* filename, const GLUSgltfLoadOptions* options, GLUSgltfScene* scene);

/**
 * Free all GPU objects and host memory owned by a scene. Safe to call on a
 * zero-initialised scene.
 *
 * @param scene Scene to destroy.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfDestroyScene(GLUSgltfScene* scene);

/**
 * Advance the active animation by deltaTime seconds, sample its channels and
 * recompute world matrices and joint matrices. When no animation is active the
 * world matrices are recomputed once from the rest pose.
 *
 * @param scene     Scene to update.
 * @param deltaTime Seconds elapsed since the previous frame.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfUpdateAnimation(GLUSgltfScene* scene, GLUSfloat deltaTime);

/**
 * Select the active animation clip.
 *
 * @param scene      Scene to update.
 * @param animIndex  Index into GLUSgltfScene::animations, or -1 for none.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfSetActiveAnimation(GLUSgltfScene* scene, GLUSint animIndex);

/**
 * Seek the active animation to an absolute time and recompute transforms.
 *
 * @param scene Scene to update.
 * @param time  Absolute animation time in seconds.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfSetAnimationTime(GLUSgltfScene* scene, GLUSfloat time);

/**
 * Bind the primitive's VAO and issue its draw call. The application is
 * responsible for the shader program, material uniforms and texture bindings.
 *
 * @param scene         Scene containing the primitive.
 * @param primitiveIndex Index of the primitive to draw.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfDrawPrimitive(const GLUSgltfScene* scene, GLUSint primitiveIndex);

/**
 * Recompute all node world matrices and skin joint matrices from the current
 * local TRS / matrix state. Useful after manually editing node transforms.
 *
 * @param scene Scene to update.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfUpdateTransforms(GLUSgltfScene* scene);

/**
 * Compute the view and projection matrices for a glTF camera.
 *
 * The view matrix is the inverse of the camera node's world matrix (call after
 * glusGltfUpdateAnimation / glusGltfUpdateTransforms so the world matrices are
 * current). Perspective cameras with zfar == 0 use a far approximation; when
 * aspectRatio == 0 the supplied viewport aspect is used.
 *
 * @param scene          Loaded scene.
 * @param cameraIndex    Index into GLUSgltfScene::cameras.
 * @param viewportAspect Viewport width / height (used when the camera has no aspectRatio).
 * @param viewOut        Receives the 4x4 view matrix (column-major).
 * @param projOut        Receives the 4x4 projection matrix (column-major).
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusGltfCameraMatrices(const GLUSgltfScene* scene, GLUSint cameraIndex, GLUSfloat viewportAspect, GLUSfloat viewOut[16], GLUSfloat projOut[16]);

/**
 * Extension hook: the parsed cgltf tree, kept alive until glusGltfDestroyScene.
 * Applications that need glTF extension data (e.g. KHR_gaussian_splatting)
 * #include "cgltf.h" and read what they need from this handle. GLUS itself
 * never interprets extensions.
 *
 * @param scene Loaded scene.
 *
 * @return The cgltf_data handle, or NULL.
 */
GLUSAPI struct cgltf_data* GLUSAPIENTRY glusGltfGetCgltfData(const GLUSgltfScene* scene);

/**
 * The directory portion of the source filename (with trailing separator), used
 * to resolve relative image URIs. Owned by the scene.
 *
 * @param scene Loaded scene.
 *
 * @return Base path string, possibly empty.
 */
GLUSAPI const GLUSchar* GLUSAPIENTRY glusGltfGetBasePath(const GLUSgltfScene* scene);

#endif /* GLUS_GLTF_H_ */
