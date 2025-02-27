#if !defined(XAC_H)
#define XAC_H
#include "shared_formats.h"

enum EMeshType
{
    MESHTYPE_STATIC = 0,    /**< Static mesh, like a cube or building (can still be position/scale/rotation animated though). */
    MESHTYPE_DYNAMIC = 1,   /**< Has mesh deformers that have to be processed on the CPU. */
    MESHTYPE_GPUSKINNED = 2 /**< Just a skinning mesh deformer that gets processed on the GPU with skinned shader. */
};

enum EPhonemeSet
{
    PHONEMESET_NONE = 0,
    PHONEMESET_NEUTRAL_POSE = 1 << 0,
    PHONEMESET_M_B_P_X = 1 << 1,
    PHONEMESET_AA_AO_OW = 1 << 2,
    PHONEMESET_IH_AE_AH_EY_AY_H = 1 << 3,
    PHONEMESET_AW = 1 << 4,
    PHONEMESET_N_NG_CH_J_DH_D_G_T_K_Z_ZH_TH_S_SH = 1 << 5,
    PHONEMESET_IY_EH_Y = 1 << 6,
    PHONEMESET_UW_UH_OY = 1 << 7,
    PHONEMESET_F_V = 1 << 8,
    PHONEMESET_L_EL = 1 << 9,
    PHONEMESET_W = 1 << 10,
    PHONEMESET_R_ER = 1 << 11
};

// enum ECloneFlags //TransformData
// {
//     CLONE_NONE = 0,                                                                                                                   /**< Clone nothing, but share all transform data instead. */
//     CLONE_WORLDMATRICES = 1 << 0,                                                                                                     /**< Clone the world space matrices. */
//     CLONE_LOCALMATRICES = 1 << 1,                                                                                                     /**< Clone the local space matrices. */
//     CLONE_INVBINDPOSEMATRICES = 1 << 2,                                                                                               /**< Clone the inverse bind pose matrices. */
//     CLONE_ORGTRANSFORMS = 1 << 3,                                                                                                     /**< Clone the original (bind pose) local space transformations. */
//     CLONE_LOCALTRANSFORMS = 1 << 4,                                                                                                   /**< Clone the current local space transformations. */
//     CLONE_ALL = (CLONE_WORLDMATRICES | CLONE_LOCALMATRICES | CLONE_INVBINDPOSEMATRICES | CLONE_ORGTRANSFORMS | CLONE_LOCALTRANSFORMS) /**< Clone everything, share nothing. */
// };

// enum ENodeFlags //TransformData
// {
//     FLAG_NEEDLOCALTMUPDATE = 1 << 0, /**< Does the local space matrix need updating? */
//     FLAG_HASSCALE = 1 << 1,          /**< Has the node a scale factor? */
//     FLAG_HASSCALEROTATION = 1 << 2   /**< Is the node using a scale rotation? */
// };

enum EWaveletType
{
    WAVELET_HAAR = 0,  /**< The Haar wavelet, which is most likely what you want to use. It is the fastest also. */
    WAVELET_DAUB4 = 1, /**< Daubechies 4 wavelet, can result in bit better compression ratios, but slower than Haar. */
    WAVELET_CDF97 = 2  /**< The CDF97 wavelet, used in JPG as well. This is the slowest, but often results in the best compression ratios. */
};

enum ENodeCloneFlags
{
    CLONE_ATTRIBUTES = 1 << 0,                                                                                           /**< Clone (duplicate) the node attributes. If not set, the node attributes will NOT be copied (and not shared either). */
    CLONE_NODESTACKS = 1 << 1,                                                                                           /**< Clone (duplicate) the node stacks from a given node. */
    CLONE_NODECOLLISIONSTACKS = 1 << 2,                                                                                  /**< Clone (duplicate) the collision node stacks from a given node. */
    CLONE_MESHES = 1 << 3,                                                                                               /**< Clone (duplicate) the mesh data. */
    CLONE_COLLISIONMESHES = 1 << 4,                                                                                      /**< Clone (duplicate) the collision mesh data. */
    CLONE_DEFAULT = CLONE_ATTRIBUTES,                                                                                    /**< The default node cloning flags, which only copies the node attributes. The rest will be shared. */
    CLONE_ALL = (CLONE_ATTRIBUTES | CLONE_NODESTACKS | CLONE_NODECOLLISIONSTACKS | CLONE_MESHES | CLONE_COLLISIONMESHES) /**< The node cloning flags to clone all attributes available. So nothing will be shared. */
};

enum ENodeFlags
{
    FLAG_INCLUDEINBOUNDSCALC = 1 << 0, /**< Specifies whether we have to include this node in the bounds calculation or not (true on default). */
    FLAG_ATTACHMENT = 1 << 1           /**< Indicates if this node is an attachment node or not (false on default). */
};

enum EPlane
{
    PLANE_XY = 0, /**< The XY plane, so where Z is constant. */
    PLANE_XZ = 1, /**< The XZ plane, so where Y is constant. */
    PLANE_YZ = 2  /**< The YZ plane, so where X is constant. */
};

enum EDependencyType
{
    DEPENDENCY_MESHES = 1 << 0,    /**< Shared meshes. */
    DEPENDENCY_TRANSFORMS = 1 << 1 /**< Shared transforms. */
};

enum EActorCloneFlags
{
    CLONE_MATERIALS = 1 << 0,                                                                                       /**< Clones the materials itself, so do not share them. */
    CLONE_NODEATTRIBUTES = 1 << 1,                                                                                  /**< Clones the node attributes, so do not share them. */
    CLONE_CONTROLLERS = 1 << 2,                                                                                     /**< Clone (duplicate) the controllers. If not set, the controllers will NOT be copied. */
    CLONE_MESHES = 1 << 3,                                                                                          /**< Clones the mesh data, so does not share the mesh data. */
    CLONE_COLLISIONMESHES = 1 << 4,                                                                                 /**< Clones the collision mesh data, so does not share the collision mesh data. */
    CLONE_DEFAULT = (CLONE_NODEATTRIBUTES | CLONE_CONTROLLERS),                                                     /**< The default actor cloning flags, which only copies the node attributes. The rest will be shared. */
    CLONE_ALL = (CLONE_MATERIALS | CLONE_NODEATTRIBUTES | CLONE_CONTROLLERS | CLONE_MESHES | CLONE_COLLISIONMESHES) /**< The actor cloning flags to clone all attributes available. So nothing will be shared. */
};

/// The motion based actor repositioning mask
enum ERepositioningMask
{
    REPOSITION_POSITION = 1 << 0, /**< Update the actor position based on the repositioning node. */
    REPOSITION_ROTATION = 1 << 1, /**< Update the actor rotation based on the repositioning node. */
    REPOSITION_SCALE = 1 << 2     /**< [CURRENTLY UNSUPPORTED] Update the actor scale based on the repositioning node. */
};

/// The order of multiplication when composing a transformation matrix from a translation, rotation and scale.
enum EMultiplicationOrder
{
    SCALE_ROTATION_TRANSLATION = 0, /**< LocalTM = scale * rotation * translation (Maya style). */
    ROTATION_SCALE_TRANSLATION = 1  /**< LocalTM = rotation * scale * translation (3DSMax style) [default]. */
};

enum ELimitType
{
    TRANSLATIONX = 1 << 0, /**< Position limit on the x axis. */
    TRANSLATIONY = 1 << 1, /**< Position limit on the y axis. */
    TRANSLATIONZ = 1 << 2, /**< Position limit on the z axis. */
    ROTATIONX = 1 << 3,    /**< Rotation limit on the x axis. */
    ROTATIONY = 1 << 4,    /**< Rotation limit on the y axis. */
    ROTATIONZ = 1 << 5,    /**< Rotation limit on the z axis. */
    SCALEX = 1 << 6,       /**< Scale limit on the x axis. */
    SCALEY = 1 << 7,       /**< Scale limit on the y axis. */
    SCALEZ = 1 << 8        /**< Scale limit on the z axis. */
};

enum
{
    ATTRIB_POSITIONS = 0,     /**< Vertex positions. Typecast to MCore::Vector3. Positions are always exist. */
    ATTRIB_NORMALS = 1,       /**< Vertex normals. Typecast to MCore::Vector3. Normals are always exist. */
    ATTRIB_TANGENTS = 2,      /**< Vertex tangents. Typecast to <b> MCore::Vector4 </b>. */
    ATTRIB_UVCOORDS = 3,      /**< Vertex uv coordinates. Typecast to MCore::Vector2. */
    ATTRIB_COLORS32 = 4,      /**< Vertex colors in 32-bits. Typecast to uint32. */
    ATTRIB_ORGVTXNUMBERS = 5, /**< Original vertex numbers. Typecast to uint32. Original vertex numbers always exist. */
    ATTRIB_COLORS128 = 6,     /**< Vertex colors in 128-bits. Typecast to MCore::RGBAColor. */
    ATTRIB_BITANGENTS = 7     /**< Vertex bitangents (aka binormal). Typecast to MCore::Vector3. When tangents exists bitangents may still not exist! */
};

// collection of XAC chunk IDs
enum
{
    XAC_CHUNK_NODE = 0,
    XAC_CHUNK_MESH = 1,
    XAC_CHUNK_SKINNINGINFO = 2,
    XAC_CHUNK_STDMATERIAL = 3,
    XAC_CHUNK_STDMATERIALLAYER = 4,
    XAC_CHUNK_FXMATERIAL = 5,
    XAC_CHUNK_LIMIT = 6,
    XAC_CHUNK_INFO = 7,
    XAC_CHUNK_MESHLODLEVELS = 8,
    XAC_CHUNK_STDPROGMORPHTARGET = 9,
    XAC_CHUNK_NODEGROUPS = 10,
    XAC_CHUNK_NODES = 11,             // XAC_Nodes
    XAC_CHUNK_STDPMORPHTARGETS = 12,  // XAC_PMorphTargets
    XAC_CHUNK_MATERIALINFO = 13,      // XAC_MaterialInfo
    XAC_CHUNK_NODEMOTIONSOURCES = 14, // XAC_NodeMotionSources
    XAC_CHUNK_ATTACHMENTNODES = 15,   // XAC_AttachmentNodes
    XAC_FORCE_32BIT = 0xFFFFFFFF
};

// material layer map types
enum
{
    XAC_LAYERID_UNKNOWN = 0,       // unknown layer
    XAC_LAYERID_AMBIENT = 1,       // ambient layer
    XAC_LAYERID_DIFFUSE = 2,       // a diffuse layer
    XAC_LAYERID_SPECULAR = 3,      // specular layer
    XAC_LAYERID_OPACITY = 4,       // opacity layer
    XAC_LAYERID_BUMP = 5,          // bump layer
    XAC_LAYERID_SELFILLUM = 6,     // self illumination layer
    XAC_LAYERID_SHINE = 7,         // shininess (for specular)
    XAC_LAYERID_SHINESTRENGTH = 8, // shine strength (for specular)
    XAC_LAYERID_FILTERCOLOR = 9,   // filter color layer
    XAC_LAYERID_REFLECT = 10,      // reflection layer
    XAC_LAYERID_REFRACT = 11,      // refraction layer
    XAC_LAYERID_ENVIRONMENT = 12,  // environment map layer
    XAC_LAYERID_DISPLACEMENT = 13, // displacement map layer
    XAC_LAYERID_FORCE_8BIT = 0xFF  // don't use more than 8 bit values
};

// the actor file type header
struct XAC_Header
{
    uint8_t mFourcc[4];  // must be "XAC "
    uint8_t mHiVersion;  // high version (2  in case of v2.34)
    uint8_t mLoVersion;  // low version  (34 in case of v2.34)
    uint8_t mEndianType; // the endian in which the data is saved [0=little, 1=big]
    uint8_t mMulOrder;   // see the enum MULORDER_....
};

struct XAC_Info
{
    uint32_t mRepositioningMask;      // the repositioning mask, which describes what transformation components to adjust when repositioning is enabled
    uint32_t mRepositioningNodeIndex; // the node number
    uint8_t mExporterHighVersion;
    uint8_t mExporterLowVersion;
    uint16_t padding;

    std::string mSourceApp;
    std::string mOrigFilename;
    std::string mCompDate;
    std::string mActorName;

    // followed by:
    // string : source application (e.g. "3D Studio MAX 8", "Maya 7.0")
    // string : original filename of the 3dsMax/Maya file
    // string : compilation date of the exporter
    // string : the name of the actor
};

//
struct XAC_Info2
{
    uint32_t mRepositioningMask;      // the repositioning mask, which describes what transformation components to adjust when repositioning is enabled
    uint32_t mRepositioningNodeIndex; // the node number
    uint8_t mExporterHighVersion;
    uint8_t mExporterLowVersion;
    float mRetargetRootOffset;
    uint16_t padding;

    std::string mSourceApp;
    std::string mOrigFilename;
    std::string mCompDate;
    std::string mActorName;

    // followed by:
    // string : source application (e.g. "3D Studio MAX 8", "Maya 7.0")
    // string : original filename of the 3dsMax/Maya file
    // string : compilation date of the exporter
    // string : the name of the actor
};

//
struct XAC_Info3
{
    uint32_t mTrajectoryNodeIndex;       // the node number of the trajectory node used for motion extraction
    uint32_t mMotionExtractionNodeIndex; // the node number of the trajectory node used for motion extraction
    uint32_t mMotionExtractionMask;      // the motion extraction mask, which describes what transformation components to adjust when motion extraction is enabled
    uint8_t mExporterHighVersion;
    uint8_t mExporterLowVersion;
    float mRetargetRootOffset;
    uint16_t padding;

    std::string mSourceApp;
    std::string mOrigFilename;
    std::string mCompDate;
    std::string mActorName;

    // followed by:
    // string : source application (e.g. "3ds Max 2011", "Maya 2011")
    // string : original filename of the 3dsMax/Maya file
    // string : compilation date of the exporter
    // string : the name of the actor
};

//
struct XAC_Info4
{
    uint32_t mNumLODs;                   // the number of level of details
    uint32_t mTrajectoryNodeIndex;       // the node number of the trajectory node used for motion extraction
    uint32_t mMotionExtractionNodeIndex; // the node number of the trajectory node used for motion extraction
    uint8_t mExporterHighVersion;
    uint8_t mExporterLowVersion;
    float mRetargetRootOffset;
    uint16_t padding;

    std::string mSourceApp;
    std::string mOrigFilename;
    std::string mCompDate;
    std::string mActorName;

    // followed by:
    // string : source application (e.g. "3ds Max 2011", "Maya 2011")
    // string : original filename of the 3dsMax/Maya file
    // string : compilation date of the exporter
    // string : the name of the actor
};

// a node header
struct XAC_Node
{
    FileQuaternion mLocalQuat; // the local rotation (before hierarchy)
    FileQuaternion mScaleRot;  // scale rotation (space in which to perform scaling)
    FileVector3 mLocalPos;     // the local translation (before hierarchy)
    FileVector3 mLocalScale;   // the local scale (before hierarchy)
    FileVector3 mShear;        // shear (x=XY, y=XZ, z=YZ)
    uint32_t mSkeletalLODs;    // each bit representing if the node is active or not, in the give LOD (bit number)
    uint32_t mParentIndex;     // parent node number, or 0xFFFFFFFF in case of a root node

    std::string mNodeName;
    // followed by:
    // string : node name (the unique name of the node)
};

// a node header
struct XAC_Node2
{
    FileQuaternion mLocalQuat; // the local rotation (before hierarchy)
    FileQuaternion mScaleRot;  // scale rotation (space in which to perform scaling)
    FileVector3 mLocalPos;     // the local translation (before hierarchy)
    FileVector3 mLocalScale;   // the local scale (before hierarchy)
    FileVector3 mShear;        // shear (x=XY, y=XZ, z=YZ)
    uint32_t mSkeletalLODs;    // each bit representing if the node is active or not, in the give LOD (bit number)
    uint32_t mParentIndex;     // parent node number, or 0xFFFFFFFF in case of a root node
    uint8_t mNodeFlags;        // #1 bit boolean specifies whether we have to include this node in the bounds calculation or not
    uint8_t padding[3];

    std::string mNodeName;

    // followed by:
    // string : node name (the unique name of the node)
};

// a node header
struct XAC_Node3
{
    FileQuaternion mLocalQuat; // the local rotation (before hierarchy)
    FileQuaternion mScaleRot;  // scale rotation (space in which to perform scaling)
    FileVector3 mLocalPos;     // the local translation (before hierarchy)
    FileVector3 mLocalScale;   // the local scale (before hierarchy)
    FileVector3 mShear;        // shear (x=XY, y=XZ, z=YZ)
    uint32_t mSkeletalLODs;    // each bit representing if the node is active or not, in the give LOD (bit number)
    uint32_t mParentIndex;     // parent node number, or 0xFFFFFFFF in case of a root node
    uint8_t mNodeFlags;        // #1 bit boolean specifies whether we have to include this node in the bounds calculation or not
    float mOBB[16];
    uint8_t padding[3];

    std::string mNodeName;

    // followed by:
    // string : node name (the unique name of the node)
};

// a node header
struct XAC_Node4
{
    FileQuaternion mLocalQuat; // the local rotation (before hierarchy)
    FileQuaternion mScaleRot;  // scale rotation (space in which to perform scaling)
    FileVector3 mLocalPos;     // the local translation (before hierarchy)
    FileVector3 mLocalScale;   // the local scale (before hierarchy)
    FileVector3 mShear;        // shear (x=XY, y=XZ, z=YZ)
    uint32_t mSkeletalLODs;    // each bit representing if the node is active or not, in the give LOD (bit number)
    uint32_t mMotionLODs;      // each bit representing if the node is active or not, in the give LOD (bit number)
    uint32_t mParentIndex;     // parent node number, or 0xFFFFFFFF in case of a root node
    uint32_t mNumChilds;       // the number of child nodes
    uint8_t mNodeFlags;        // #1 bit boolean specifies whether we have to include this node in the bounds calculation or not
    float mOBB[16];
    float mImportanceFactor; // importance factor used for the automatic motion lod
    uint8_t padding[3];

    std::string mNodeName;

    // followed by:
    // string : node name (the unique name of the node)
};

// a mesh LOD level
struct XAC_MeshLODLevel
{
    uint32_t mLODLevel;
    uint32_t mSizeInBytes;

    // followed by:
    // array[uint8_t] The LOD model memory file
};

// uv (texture) coordinate
struct XAC_UV
{
    float mU;
    float mV;
};

struct XAC_SkinningInfo
{
    uint32_t mNodeIndex;
    uint8_t mIsForCollisionMesh;
    uint8_t padding[3];

    // followed by:
    // for all mesh org num vertices
    //     uint8_t numInfluences
    //         XAC_SkinInfluence[numInfluences];
};

struct XAC_SkinningInfo2
{
    uint32_t mNodeIndex;          // the node number in the actor
    uint32_t mNumTotalInfluences; // the total number of influences of all vertices together
    uint8_t mIsForCollisionMesh;  // is it for a collision mesh?
    uint8_t padding[3];

    // followed by:
    //	 XAC_SkinInfluence[mNumTotalInfluences]
    //	 XAC_SkinningInfoTableEntry[mesh.GetNumOrgVerts()]
};

struct XAC_SkinningInfo3
{
    uint32_t mNodeIndex;          // the node number in the actor
    uint32_t mNumLocalBones;      // number of local bones to reserve space for, this represents how many bones are used by the mesh the skinning is linked to
    uint32_t mNumTotalInfluences; // the total number of influences of all vertices together
    uint8_t mIsForCollisionMesh;  // is it for a collision mesh?
    uint8_t padding[3];
    // followed by:
    //	 XAC_SkinInfluence[mNumTotalInfluences]
    //	 XAC_SkinningInfoTableEntry[mesh.GetNumOrgVerts()]
};

struct XAC_SkinningInfo4
{
    uint32_t mNodeIndex;          // the node number in the actor
    uint32_t mLOD;                // the level of detail
    uint32_t mNumLocalBones;      // number of local bones to reserve space for, this represents how many bones are used by the mesh the skinning is linked to
    uint32_t mNumTotalInfluences; // the total number of influences of all vertices together
    uint8_t mIsForCollisionMesh;  // is it for a collision mesh?
    uint8_t padding[3];

    // followed by:
    //	 XAC_SkinInfluence[mNumTotalInfluences]
    //	 XAC_SkinningInfoTableEntry[mesh.GetNumOrgVerts()]
};

struct XAC_SkinningInfoTableEntry
{
    uint32_t mStartIndex;  // index inside the SkinInfluence array
    uint32_t mNumElements; // the number of influences for this item/entry that follow from the given start index
};

// a skinning influence
struct XAC_SkinInfluence
{
    float mWeight;
    uint32_t mNodeNr;
};

// standard material
struct XAC_StandardMaterial
{
    FileColor mAmbient;        // ambient color
    FileColor mDiffuse;        // diffuse color
    FileColor mSpecular;       // specular color
    FileColor mEmissive;       // self illumination color
    float mShine;              // shine
    float mShineStrength;      // shine strength
    float mOpacity;            // the opacity amount [1.0=full opac, 0.0=full transparent]
    float mIOR;                // index of refraction
    uint8_t mDoubleSided;      // double sided?
    uint8_t mWireFrame;        // render in wireframe?
    uint8_t mTransparencyType; // F=filter / S=substractive / A=additive / U=unknown
    uint8_t padding;

    // followed by:
    // string : material name
};

// standard material, with integrated set of standard material layers
struct XAC_StandardMaterial2
{
    FileColor mAmbient;        // ambient color
    FileColor mDiffuse;        // diffuse color
    FileColor mSpecular;       // specular color
    FileColor mEmissive;       // self illumination color
    float mShine;              // shine
    float mShineStrength;      // shine strength
    float mOpacity;            // the opacity amount [1.0=full opac, 0.0=full transparent]
    float mIOR;                // index of refraction
    uint8_t mDoubleSided;      // double sided?
    uint8_t mWireFrame;        // render in wireframe?
    uint8_t mTransparencyType; // F=filter / S=substractive / A=additive / U=unknown
    uint8_t mNumLayers;        // the number of material layers

    // followed by:
    // string : material name
    // XAC_StandardMaterialLayer2[ mNumLayers ]
};

// standard material, with integrated set of standard material layers
struct XAC_StandardMaterial3
{
    uint32_t mLOD;             // the level of detail
    FileColor mAmbient;        // ambient color
    FileColor mDiffuse;        // diffuse color
    FileColor mSpecular;       // specular color
    FileColor mEmissive;       // self illumination color
    float mShine;              // shine
    float mShineStrength;      // shine strength
    float mOpacity;            // the opacity amount [1.0=full opac, 0.0=full transparent]
    float mIOR;                // index of refraction
    uint8_t mDoubleSided;      // double sided?
    uint8_t mWireFrame;        // render in wireframe?
    uint8_t mTransparencyType; // F=filter / S=substractive / A=additive / U=unknown
    uint8_t mNumLayers;        // the number of material layers

    // followed by:
    // string : material name
    // XAC_StandardMaterialLayer2[ mNumLayers ]
};

// a material layer
struct XAC_StandardMaterialLayer
{
    float mAmount;            // the amount, between 0 and 1
    float mUOffset;           // u offset (horizontal texture shift)
    float mVOffset;           // v offset (vertical texture shift)
    float mUTiling;           // horizontal tiling factor
    float mVTiling;           // vertical tiling factor
    float mRotationRadians;   // texture rotation in radians
    uint16_t mMaterialNumber; // the parent material number (as read from the file, where 0 means the first material)
    uint8_t mMapType;         // the map type (see enum in somewhere near the top of file)
    uint8_t padding;

    // followed by:
    // string : texture filename
};

// a material layer (version 2)
struct XAC_StandardMaterialLayer2
{
    float mAmount;            // the amount, between 0 and 1
    float mUOffset;           // u offset (horizontal texture shift)
    float mVOffset;           // v offset (vertical texture shift)
    float mUTiling;           // horizontal tiling factor
    float mVTiling;           // vertical tiling factor
    float mRotationRadians;   // texture rotation in radians
    uint16_t mMaterialNumber; // the parent material number (as read from the file, where 0 means the first material)
    uint8_t mMapType;         // the map type (see enum in somewhere near the top of file)
    uint8_t mBlendMode;       // blend mode that is used to control how successive layers of textures are combined together

    // followed by:
    // string : texture filename
};

//-------------------------------------------------------
// a vertex attribute layer
struct XAC_VertexAttributeLayer
{
    uint32_t mLayerTypeID;       // the type of vertex attribute layer
    uint32_t mAttribSizeInBytes; // the size of a single vertex attribute of this type, in bytes
    uint8_t mEnableDeformations; // enable deformations on this layer?
    uint8_t mIsScale;            // is this a scale value, or not? (coordinate system conversion thing)
    uint8_t padding[3];

    // followed by:
    // (sizeof(mAttribSizeInBytes) * mesh.numVertices) bytes, or mesh.numVertices mDataType objects
};

// a submesh
struct XAC_SubMesh
{
    uint32_t mNumIndices;    // number of indices
    uint32_t mNumVerts;      // number of vertices
    uint32_t mMaterialIndex; // material number, indexes into the file, so 0 means the first read material
    uint32_t mNumBones;      // the number of bones used by this submesh

    // followed by:
    // uint32_t[mNumIndices]
    // uint32_t[mNumBones]
};

// a mesh
struct XAC_Mesh
{
    uint32_t mNodeIndex;      // the node number this mesh belongs to (0 means the first node in the file, 1 means the second, etc.)
    uint32_t mNumOrgVerts;    // number of original vertices
    uint32_t mTotalVerts;     // total number of vertices (of all submeshes)
    uint32_t mTotalIndices;   // total number of indices (of all submeshes)
    uint32_t mNumSubMeshes;   // number of submeshes to follow
    uint32_t mNumLayers;      // the number of layers to follow
    uint8_t mIsCollisionMesh; // is this mesh a collision mesh or a normal mesh?
    uint8_t padding[3];
    // followed by:
    // XAC_VertexAttributeLayer[mNumLayers]
    // XAC_SubMesh[mNumSubMeshes]
};

// a mesh
struct XAC_Mesh2
{
    uint32_t mNodeIndex;      // the node number this mesh belongs to (0 means the first node in the file, 1 means the second, etc.)
    uint32_t mLOD;            // the level of detail
    uint32_t mNumOrgVerts;    // number of original vertices
    uint32_t mTotalVerts;     // total number of vertices (of all submeshes)
    uint32_t mTotalIndices;   // total number of indices (of all submeshes)
    uint32_t mNumSubMeshes;   // number of submeshes to follow
    uint32_t mNumLayers;      // the number of layers to follow
    uint8_t mIsCollisionMesh; // is this mesh a collision mesh or a normal mesh?
    uint8_t padding[3];

    // followed by:
    // XAC_VertexAttributeLayer[mNumLayers]
    // XAC_SubMesh[mNumSubMeshes]
};

//-------------------------------------------------------

// node limit information
struct XAC_Limit
{
    FileVector3 mTranslationMin; // the minimum translation values
    FileVector3 mTranslationMax; // the maximum translation value.
    FileVector3 mRotationMin;    // the minimum rotation values
    FileVector3 mRotationMax;    // the maximum rotation values
    FileVector3 mScaleMin;       // the minimum scale values
    FileVector3 mScaleMax;       // the maximum scale values
    uint8_t mLimitFlags[9];      // the limit type activation flags
    uint32_t mNodeNumber;        // the node number where this info belongs to
};

// a progressive morph target mesh
struct XAC_PMorphTarget
{
    float mRangeMin;               // the slider min
    float mRangeMax;               // the slider max
    uint32_t mLOD;                 // the level of detail to which this expression part belongs to
    uint32_t mNumMeshDeformDeltas; // the number of mesh deform data objects to follow
    uint32_t mNumTransformations;  // the number of transformations to follow
    uint32_t mPhonemeSets;         // the number of phoneme sets to follow

    // followed by:
    // string : progressive morph target name
    // XAC_PMorphTargetMeshDeltas[ mNumDeformDatas ]
    // XAC_PMorphTargetTransform[ mNumTransformations ]
};

// a chunk that contains all morph targets in the file
struct XAC_PMorphTargets
{
    uint32_t mNumMorphTargets; // the number of morph targets to follow
    uint32_t mLOD;             // the LOD level the morph targets are for

    // followed by:
    // XAC_PMorphTarget[ mNumMorphTargets ]
};

// morph target deformation data
struct XAC_PMorphTargetMeshDeltas
{
    uint32_t mNodeIndex;
    float mMinValue;       // minimum range value for x y and z components of the compressed position vectors
    float mMaxValue;       // maximum range value for x y and z components of the compressed position vectors
    uint32_t mNumVertices; // the number of deltas

    // followed by:
    // File16BitVector3[ mNumVertices ]  (delta position values)
    // File8BitVector3[ mNumVertices ]	 (delta normal values)
    // File8BitVector3[ mNumVertices ]	 (delta tangent values)
    // uint32_t[ mNumVertices ]			 (vertex numbers)
};

// a progressive morph target transformation
struct XAC_PMorphTargetTransform
{
    uint32_t mNodeIndex;           // the node name where the transform belongs to
    FileQuaternion mRotation;      // the node rotation
    FileQuaternion mScaleRotation; // the node delta scale rotation
    FileVector3 mPosition;         // the node delta position
    FileVector3 mScale;            // the node delta scale
};

// an FX material, using parameters
struct XAC_FXMaterial
{
    uint32_t mNumIntParams;
    uint32_t mNumFloatParams;
    uint32_t mNumColorParams;
    uint32_t mNumBitmapParams;

    // followed by:
    // string : name
    // string : effect file (path excluded, extension included)
    // XAC_FXIntParameter   [ mNumIntParams ]
    // XAC_FXFloatParameter [ mNumFloatParams ]
    // XAC_FXColorParameter [ mNumColorParams ]
    // [mNumBitmapParams]
    //		string : param name
    //		string : value
};

// an FX material, using parameters
struct XAC_FXMaterial2
{
    uint32_t mNumIntParams;
    uint32_t mNumFloatParams;
    uint32_t mNumColorParams;
    uint32_t mNumBoolParams;
    uint32_t mNumVector3Params;
    uint32_t mNumBitmapParams;

    // followed by:
    // string : name
    // string : effect file (path excluded, extension included)
    // string : shader technique
    // XAC_FXIntParameter		[ mNumIntParams ]
    // XAC_FXFloatParameter		[ mNumFloatParams ]
    // XAC_FXColorParameter		[ mNumColorParams ]
    // XAC_FXBoolParameter		[ mNumBoolParams ]
    // XAC_FXVector3Parameter	[ mNumVector3Params ]
    // [mNumBitmapParams]
    //		string : param name
    //		string : value
};

// an FX material, using parameters
struct XAC_FXMaterial3
{
    uint32_t mLOD; // the level of detail
    uint32_t mNumIntParams;
    uint32_t mNumFloatParams;
    uint32_t mNumColorParams;
    uint32_t mNumBoolParams;
    uint32_t mNumVector3Params;
    uint32_t mNumBitmapParams;

    // followed by:
    // string : name
    // string : effect file (path excluded, extension included)
    // string : shader technique
    // XAC_FXIntParameter		[ mNumIntParams ]
    // XAC_FXFloatParameter		[ mNumFloatParams ]
    // XAC_FXColorParameter		[ mNumColorParams ]
    // XAC_FXBoolParameter		[ mNumBoolParams ]
    // XAC_FXVector3Parameter	[ mNumVector3Params ]
    // [mNumBitmapParams]
    //		string : param name
    //		string : value
};

struct XAC_FXIntParameter
{
    int32_t mValue; // beware, not unsigned, as negative would also be allowed
                    // followed by:
                    // string : name
};

struct XAC_FXFloatParameter
{
    float mValue;
    // followed by:
    // string : name
};

struct XAC_FXColorParameter
{
    FileColor mValue;
    // followed by:
    // string : name
};

struct XAC_FXVector3Parameter
{
    FileVector3 mValue;
    // followed by:
    // string : name
};

struct XAC_FXBoolParameter
{
    uint8_t mValue; // 0 = no, 1 = yes
                    // followed by:
                    // string : name
};

struct XAC_FXBitmapParameter
{
    std::string mParamName;
    std::string mValueName;
    // followed by:
    // string : name
};

// a node group
struct XAC_NodeGroup
{
    uint16_t mNumNodes;
    uint8_t mDisabledOnDefault; // 0 = no, 1 = yes

    // followed by:
    // string : name
    // uint16_t [mNumNodes]
};

// a collection of all nodes
struct XAC_Nodes
{
    uint32_t mNumNodes;
    uint32_t mNumRootNodes;
    // followed by XAC_Node4[mNumNodes]
};

// material statistics, which appears before the actual material chunks
struct XAC_MaterialInfo
{
    uint32_t mNumTotalMaterials;    // total number of materials to follow (including default/extra material)
    uint32_t mNumStandardMaterials; // the number of standard materials in the file
    uint32_t mNumFXMaterials;       // the number of fx materials in the file
};

// material statistics, which appears before the actual material chunks
struct XAC_MaterialInfo2
{
    uint32_t mLOD;                  // the level of detail
    uint32_t mNumTotalMaterials;    // total number of materials to follow (including default/extra material)
    uint32_t mNumStandardMaterials; // the number of standard materials in the file
    uint32_t mNumFXMaterials;       // the number of fx materials in the file
};

// node motion sources used for the motion mirroring feature
struct XAC_NodeMotionSources
{
    uint32_t mNumNodes;
    // followed by uint16_t[mNumNodes]	// an index per node, which indicates the index of the node to extract the motion data from in case mirroring for a given motion is enabled. This array can be NULL in case no mirroring data has been setup.
};

// list of node number which are used for attachments
struct XAC_AttachmentNodes
{
    uint32_t mNumNodes;
    // followed by uint16_t[mNumNodes]	// an index per attachment node
};

#endif // XAC_H
