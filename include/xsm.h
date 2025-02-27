#if !defined(XSM_H)
#define XSM_H
#include "shared_formats.h"

// the chunks
enum
{
    XSM_CHUNK_SUBMOTION = 200,
    XSM_CHUNK_INFO = 201,
    XSM_CHUNK_MOTIONEVENTTABLE = SHARED_CHUNK_MOTIONEVENTTABLE,
    XSM_CHUNK_SUBMOTIONS = 202,
    XSM_CHUNK_WAVELETINFO = 203, // see XSM_WaveletInfo
    XSM_FORCE_32BIT = 0xFFFFFFFF
};

// the wavelet type used during compression
enum
{
    XSM_WAVELET_HAAR = 0,
    XSM_WAVELET_D4 = 1,
    XSM_WAVELET_CDF97 = 2
};

// the compressor used to compress the quantized data
enum
{
    XSM_COMPRESSOR_HUFFMAN = 0,
    XSM_COMPRESSOR_RICE = 1
};

// motion file header
struct XSM_Header
{
    uint8_t mFourcc[4];  // must be "XSM "
    uint8_t mHiVersion;  // high version (2  in case of v2.34)
    uint8_t mLoVersion;  // low version  (34 in case of v2.34)
    uint8_t mEndianType; // the endian in which the data is saved [0=little, 1=big]
    uint8_t mMulOrder;   // see the enum MULORDER_....
};

//
struct XSM_Info
{
    uint32_t mMotionFPS; // the motion frame rate
    uint8_t mExporterHighVersion;
    uint8_t mExporterLowVersion;

    // followed by:
    // string : source application (e.g. "3D Studio MAX 7", "Maya 6.5")
    // string : original filename of the 3DSMAX/Maya file
    // string : compilation date of the exporter
    // string : the name of the motion
};

//
struct XSM_Info2
{
    float mImportanceFactor; // the motion importance factor used by the automatic motion lod
    float mMaxAcceptableError;
    uint32_t mMotionFPS; // the motion frame rate
    uint8_t mExporterHighVersion;
    uint8_t mExporterLowVersion;

    // followed by:
    // string : source application (e.g. "3D Studio MAX 7", "Maya 6.5")
    // string : original filename of the 3DSMAX/Maya file
    // string : compilation date of the exporter
    // string : the name of the motion
};

// information chunk (added a motion extraction mask uint32_t)
struct XSM_Info3
{
    float mImportanceFactor; // the motion importance factor used by the automatic motion lod
    float mMaxAcceptableError;
    uint32_t mMotionFPS;            // the motion frame rate
    uint32_t mMotionExtractionMask; // motion extraction mask
    uint8_t mExporterHighVersion;
    uint8_t mExporterLowVersion;

    // followed by:
    // string : source application (e.g. "3D Studio MAX 7", "Maya 6.5")
    // string : original filename of the 3DSMAX/Maya file
    // string : compilation date of the exporter
    // string : the name of the motion
};

// skeletal submotion
struct XSM_SkeletalSubMotion
{
    FileQuaternion mPoseRot;          // initial pose rotation
    FileQuaternion mBindPoseRot;      // bind pose rotation
    FileQuaternion mPoseScaleRot;     // pose scale rotation
    FileQuaternion mBindPoseScaleRot; // bind pose scale rotation
    FileVector3 mPosePos;             // initial pose position
    FileVector3 mPoseScale;           // initial pose scale
    FileVector3 mBindPosePos;         // bind pose position
    FileVector3 mBindPoseScale;       // bind pose scale
    uint32_t mNumPosKeys;             // number of position keyframes to follow
    uint32_t mNumRotKeys;             // number of rotation keyframes to follow
    uint32_t mNumScaleKeys;           // number of scale keyframes to follow
    uint32_t mNumScaleRotKeys;        // number of scale rotation keyframes to follow

    // followed by:
    // string : motion part name
    // XSM_Vector3Key[ mNumPosKeys ]
    // XSM_QuaternionKey[ mNumRotKeys ]
    // XSM_Vector3Key[ mNumScaleKeys ]
    // XSM_QuaternionKey[ mNumScaleRotKeys ]
};

// skeletal submotion
struct XSM_SkeletalSubMotion2
{
    FileQuaternion mPoseRot;          // initial pose rotation
    FileQuaternion mBindPoseRot;      // bind pose rotation
    FileQuaternion mPoseScaleRot;     // pose scale rotation
    FileQuaternion mBindPoseScaleRot; // bind pose scale rotation
    FileVector3 mPosePos;             // initial pose position
    FileVector3 mPoseScale;           // initial pose scale
    FileVector3 mBindPosePos;         // bind pose position
    FileVector3 mBindPoseScale;       // bind pose scale
    uint32_t mNumPosKeys;             // number of position keyframes to follow
    uint32_t mNumRotKeys;             // number of rotation keyframes to follow
    uint32_t mNumScaleKeys;           // number of scale keyframes to follow
    uint32_t mNumScaleRotKeys;        // number of scale rotation keyframes to follow
    float mMaxError;                  // maximum error used by the automatic motion LOD system

    // followed by:
    // string : motion part name
    // XSM_Vector3Key[ mNumPosKeys ]
    // XSM_QuaternionKey[ mNumRotKeys ]
    // XSM_Vector3Key[ mNumScaleKeys ]
    // XSM_QuaternionKey[ mNumScaleRotKeys ]
};

// skeletal submotion
struct XSM_SkeletalSubMotion3
{
    File16BitQuaternion mPoseRot;          // initial pose rotation
    File16BitQuaternion mBindPoseRot;      // bind pose rotation
    File16BitQuaternion mPoseScaleRot;     // pose scale rotation
    File16BitQuaternion mBindPoseScaleRot; // bind pose scale rotation
    FileVector3 mPosePos;                  // initial pose position
    FileVector3 mPoseScale;                // initial pose scale
    FileVector3 mBindPosePos;              // bind pose position
    FileVector3 mBindPoseScale;            // bind pose scale
    uint32_t mNumPosKeys;                  // number of position keyframes to follow
    uint32_t mNumRotKeys;                  // number of rotation keyframes to follow
    uint32_t mNumScaleKeys;                // number of scale keyframes to follow
    uint32_t mNumScaleRotKeys;             // number of scale rotation keyframes to follow
    float mMaxError;                       // maximum error used by the automatic motion LOD system

    // followed by:
    // string : motion part name
    // XSM_Vector3Key[ mNumPosKeys ]
    // XSM_16BitQuaternionKey[ mNumRotKeys ]
    // XSM_Vector3Key[ mNumScaleKeys ]
    // XSM_16BitQuaternionKey[ mNumScaleRotKeys ]
};

// a 3D vector key
struct XSM_Vector3Key
{
    FileVector3 mValue; // the value
    float mTime;        // the time in seconds
};

// a quaternion key
struct XSM_QuaternionKey
{
    FileQuaternion mValue; // the value
    float mTime;           // the time in seconds
};

// a 16-bit compressed quaternion key
struct XSM_16BitQuaternionKey
{
    File16BitQuaternion mValue; // the value
    float mTime;                // the time in seconds
};

// regular submotion header
struct XSM_SubMotions
{
    uint32_t mNumSubMotions; // the number of skeletal motions

    // followed by:
    // XSM_SkeletalSubMotion2[ mNumSubMotions ]
};

// regular submotion header
struct XSM_SubMotions2
{
    uint32_t mNumSubMotions; // the number of skeletal motions

    // followed by:
    // XSM_SkeletalSubMotion3[ mNumSubMotions ]
};

// wavelet submotion mapping entry
struct XSM_WaveletMapping
{
    uint16_t mPosIndex;
    uint16_t mRotIndex;
    uint16_t mScaleRotIndex;
    uint16_t mScaleIndex;
};

// wavelet skeletal submotions header
struct XSM_WaveletInfo
{
    uint32_t mNumChunks;
    uint32_t mSamplesPerChunk;
    uint32_t mDecompressedRotNumBytes;
    uint32_t mDecompressedPosNumBytes;
    uint32_t mDecompressedScaleNumBytes;
    uint32_t mNumRotTracks;
    uint32_t mNumScaleRotTracks;
    uint32_t mNumScaleTracks;
    uint32_t mNumPosTracks;
    uint32_t mChunkOverhead;
    uint32_t mCompressedSize;
    uint32_t mOptimizedSize;
    uint32_t mUncompressedSize;
    uint32_t mScaleRotOffset;
    uint32_t mNumSubMotions;
    float mPosQuantFactor;
    float mRotQuantFactor;
    float mScaleQuantFactor;
    float mSampleSpacing;
    float mSecondsPerChunk;
    float mMaxTime;
    uint8_t mWaveletID;    // see the enum with XSM_WAVELET_HAAR etc
    uint8_t mCompressorID; // see the enum with XSM_COMPRESSOR_HUFFMAN inside

    // followed by:
    //		XSM_WaveletMapping[mNumSubMotions]
    //		XSM_WaveletSubMotion[mNumSubMotions]
    //		XSM_WaveletChunk[mNumChunks]
};

// skeletal submotion
struct XSM_WaveletSkeletalSubMotion
{
    File16BitQuaternion mPoseRot;          // initial pose rotation
    File16BitQuaternion mBindPoseRot;      // bind pose rotation
    File16BitQuaternion mPoseScaleRot;     // pose scale rotation
    File16BitQuaternion mBindPoseScaleRot; // bind pose scale rotation
    FileVector3 mPosePos;                  // initial pose position
    FileVector3 mPoseScale;                // initial pose scale
    FileVector3 mBindPosePos;              // bind pose position
    FileVector3 mBindPoseScale;            // bind pose scale
    float mMaxError;                       // maximum error used by the automatic motion LOD system

    // followed by:
    // string : motion part name
};

// a wavelet compressed chunk
struct XSM_WaveletChunk
{
    float mRotQuantScale;
    float mPosQuantScale;
    float mScaleQuantScale;
    float mStartTime;
    uint32_t mCompressedRotNumBytes;
    uint32_t mCompressedPosNumBytes;
    uint32_t mCompressedScaleNumBytes;
    uint32_t mCompressedPosNumBits;
    uint32_t mCompressedRotNumBits;
    uint32_t mCompressedScaleNumBits;

    // followed by:
    //		uint8_t	mCompressedRotData[mCompressedRotNumBytes]
    //		uint8_t	mCompressedPosData[mCompressedPosNumBytes]
    //		uint8_t	mCompressedScaleData[mCompressedScaleNumBytes]
};

#endif // XSM_H
