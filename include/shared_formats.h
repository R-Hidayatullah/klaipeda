#if !defined(SHARED_FORMATS_H)
#define SHARED_FORMATS_H
#include "klaipeda.h"

enum ESkeletalMotionType
{
    SKELMOTIONTYPE_NORMAL = 0, /**< A regular keyframe and keytrack based skeletal motion. */
    SKELMOTIONTYPE_WAVELET = 1 /**< A wavelet compressed skeletal motion. */
};

enum EFileType
{
    FILETYPE_UNKNOWN = 0,           /**< An unknown file, or something went wrong. */
    FILETYPE_ACTOR,                 /**< An actor file (.xac). */
    FILETYPE_SKELETALMOTION,        /**< A skeletal motion file (.xsm). */
    FILETYPE_WAVELETSKELETALMOTION, /**< A wavelet compressed skeletal motion (.xsm). */
    FILETYPE_PMORPHMOTION           /**< A progressive morph motion file (.xpm). */
};

// shared chunk ID's
enum
{
    SHARED_CHUNK_MOTIONEVENTTABLE = 50,
    SHARED_CHUNK_TIMESTAMP = 51
};

// matrix multiplication order
enum
{
    MULORDER_SCALE_ROT_TRANS = 0,
    MULORDER_ROT_SCALE_TRANS = 1
};

// a chunk
struct FileChunk
{
    uint32_t mChunkID;     // the chunk ID
    uint32_t mSizeInBytes; // the size in bytes of this chunk (excluding this chunk struct)
    uint32_t mVersion;     // the version of the chunk
};

// color [0..1] range
struct FileColor
{
    float mR; // red
    float mG; // green
    float mB; // blue
    float mA; // alpha
};

// a 3D vector
struct FileVector3
{
    float mX; // x+ = to the right
    float mY; // y+ = up
    float mZ; // z+ = forwards (into the depth)
};

// a compressed 3D vector
struct File16BitVector3
{
    uint16_t mX; // x+ = to the right
    uint16_t mY; // y+ = up
    uint16_t mZ; // z+ = forwards (into the depth)
};

// a compressed 3D vector
struct File8BitVector3
{
    uint8_t mX; // x+ = to the right
    uint8_t mY; // y+ = up
    uint8_t mZ; // z+ = forwards (into the depth)
};

// a quaternion
struct FileQuaternion
{
    float mX;
    float mY;
    float mZ;
    float mW;
};

// the 16 bit component quaternion
struct File16BitQuaternion
{
    int16_t mX;
    int16_t mY;
    int16_t mZ;
    int16_t mW;
};

// a motion event
struct FileMotionEvent
{
    float mTime;
    uint32_t mEventTypeIndex; // index into the event type string table
    uint32_t mParamIndex;     // index into the parameter string table
};

// a motion event version 2
struct FileMotionEvent2
{
    float mStartTime;
    float mEndTime;
    uint32_t mEventTypeIndex; // index into the event type string table
    uint32_t mParamIndex;     // index into the parameter string table
};

// a time stamp chunk
struct FileTime
{
    uint16_t mYear;
    int8_t mMonth;
    int8_t mDay;
    int8_t mHours;
    int8_t mMinutes;
    int8_t mSeconds;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

// enable struct size packing
// #include "../../Core/Source/PackPush1.h"

// a motion event version 3
struct FileMotionEvent3
{
    float mStartTime;
    float mEndTime;
    uint32_t mEventTypeIndex; // index into the event type string table
    uint16_t mParamIndex;     // index into the parameter string table
};

// motion event track (for file formats with a single track)
struct FileMotionEventTrack
{
    uint32_t mNumEvents;
    uint32_t mNumTypeStrings;
    uint32_t mNumParamStrings;

    // followed by:
    // [mNumTypeStrings] string objects
    // [mNumParamStrings] string objects
    // FileMotionEvent3[mNumEvents]
};

// motion event track
struct FileMotionEventTrack2
{
    uint32_t mNumEvents;
    uint32_t mNumTypeStrings;
    uint32_t mNumParamStrings;
    uint8_t mIsEnabled;

    // followed by:
    // String track name
    // [mNumTypeStrings] string objects
    // [mNumParamStrings] string objects
    // FileMotionEvent3[mNumEvents]
};

// a motion event table
struct FileMotionEventTable
{
    uint32_t mNumTracks;

    // followed by:
    // FileMotionEventTrack2[mNumTracks]
};

// attribute
struct FileAttribute
{
    uint32_t mDataType;
    uint32_t mNumBytes;
    uint32_t mFlags;

    // followed by:
    //		uint8_t	mData[mNumBytes];
};

#endif // SHARED_FORMATS_H
