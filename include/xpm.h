#if !defined(XPM_H)
#define XPM_H
#include "shared_formats.h"

// the chunks
enum
{
    XPM_CHUNK_SUBMOTION = 100,
    XPM_CHUNK_INFO = 101,
    XPM_CHUNK_MOTIONEVENTTABLE = SHARED_CHUNK_MOTIONEVENTTABLE,
    XPM_CHUNK_SUBMOTIONS = 102,
    XPM_FORCE_32BIT = 0xFFFFFFFF
};

// motion file header
struct XPM_Header
{
    uint8_t mFourcc[4];  // must be "XPM "
    uint8_t mHiVersion;  // high version (2  in case of v2.34)
    uint8_t mLoVersion;  // low version  (34 in case of v2.34)
    uint8_t mEndianType; // the endian in which the data is saved [0=little, 1=big]
    uint8_t mMulOrder;   // see the enum MULORDER_....
};

//
struct XPM_Info
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

// progressive morph sub motion
struct XPM_ProgressiveSubMotion
{
    float mPoseWeight;    // pose weight to use in case no animation data is present
    float mMinWeight;     // minimum allowed weight value (used for unpacking the keyframe weights)
    float mMaxWeight;     // maximum allowed weight value (used for unpacking the keyframe weights)
    uint32_t mPhonemeSet; // the phoneme set of the submotion, 0 if this is a normal progressive morph target submotion
    uint32_t mNumKeys;    // number of keyframes to follow

    // followed by:
    // string : name (the name of this motion part)
    // XPM_UnsignedShortKey[mNumKeys]
};

// a float key
struct XPM_FloatKey
{
    float mTime;  // the time, in seconds
    float mValue; // the value
};

// a uint16_t key
struct XPM_UnsignedShortKey
{
    float mTime;     // the time in seconds
    uint16_t mValue; // the value
};

struct XPM_SubMotions
{
    uint32_t mNumSubMotions;
    // followed by:
    // XPM_ProgressiveSubMotion[ mNumSubMotions ]
};
#endif // XPM_H
