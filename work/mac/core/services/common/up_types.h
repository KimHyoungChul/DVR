
#ifndef ___UP_TYPES_H
#define ___UP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IN
#define OUT
#define INOUT

/**
 *  Alias of int
 */
typedef int            UPSint;

/**
 *  Alias of unsigned int
 */
typedef unsigned int   UPUint;

//typedef UINT64 UPSint64;

/**
 *   32-bit signed integer
 */
typedef signed long         UPSint32;

/** 
 * 64-bit signed integer
 */
typedef long long  UPSint64;

/** 
 * 64-bit unsigned integer
 */
typedef unsigned long long  UPUint64;

/**
 *   32-bit unsigned integer
 */
typedef unsigned long       UPUint32;

/**
 *   16-bit signed integer
 */
typedef signed short        UPSint16;

/**
 *   16-bit unsigned integer
 */
typedef unsigned short      UPUint16;

/**
 *   8-bit signed integer
 */
typedef signed char         UPSint8;

/**
 *   8-bit unsigned integer
 */
typedef unsigned char       UPUint8;

/**
 *   Unicode character
 */
typedef UPUint16            UPWChar;

/**
 *   UTF-8 character
 */
typedef UPUint8             UPUTF8Char;

/**
 *   Boolean type
 */
typedef int                 UPBool;

/** @brief result type */
typedef signed int UPResult;

/** @brief failure */
#define UPFailure (-1)

/** @brief UPResult Success */
#define UPSuccess (0)

/** \brief The strucnture type for storing 64bit long variable. */
typedef struct _UpLongSizeTag
{
    /** \brief The high order 32-bit for the long variable. */
    UPUint high;
    /** \brief The low order 32-bit for the long variable. */
    UPUint low;
}UpLongSize;

/**
 *   UPBool False
 */
#define UP_FALSE            0

/**
 *   UPBool True
 */
#define UP_TRUE             1

/**
 *   Handle type
 */
typedef const void*         UPHandle;

/**
 *   Invalid handle
 */
#define UP_INVALID_HANDLE   ((void*)UP_NULL)

/**
 *  The handle for zlib i/o operation
 */
#define UP_INVALID_INDEX (-1)
/**
 *   NULL
 */
#ifndef __cplusplus
#define UP_NULL             ((void *)0)
#else
#define UP_NULL             (0)
#endif

/**
 *   Double-precision floating-point number
 */
typedef double UPDouble;
/**
 *   Single-precision floating-point number
 */
typedef float  UPFloat;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !___UPTYPES_H */

/**
 * @} end of group
 */

