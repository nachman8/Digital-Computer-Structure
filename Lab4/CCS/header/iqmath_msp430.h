#ifndef __IQMATH_MSP430_H__
#define __IQMATH_MSP430_H__

//*****************************************************************************
// Simple IQMath implementation for MSP430
// Q16 format: 16-bit integer, 16-bit fractional
//*****************************************************************************

#include <stdint.h>

// Define Q16 format (most common for MSP430)
#define GLOBAL_IQ    16
typedef long _iq16;
typedef long _iq;

//*****************************************************************************
// Convert a value into an IQ16 number
//*****************************************************************************
#define _IQ16(A)    ((long)((A) * (1L << 16)))
#define _IQ(A)      _IQ16(A)

//*****************************************************************************
// Convert an IQ16 number to a floating point value
//*****************************************************************************
#define _IQ16toF(A) ((float)(A) / (1L << 16))
#define _IQtoF(A)   _IQ16toF(A)

//*****************************************************************************
// Multiplies two IQ16 numbers
//*****************************************************************************
#define _IQ16mpy(A, B) ((long)(((long long)(A) * (long long)(B)) >> 16))
#define _IQmpy(A, B)   _IQ16mpy(A, B)

//*****************************************************************************
// Divides two IQ16 numbers
//*****************************************************************************
#define _IQ16div(A, B) ((long)(((long long)(A) << 16) / (long long)(B)))
#define _IQdiv(A, B)   _IQ16div(A, B)

//*****************************************************************************
// Simple multiplies or divides by powers of 2 (efficient bit shifts)
//*****************************************************************************
#define _IQ16mpy2(A)   ((A) << 1)
#define _IQ16mpy4(A)   ((A) << 2)
#define _IQ16mpy8(A)   ((A) << 3)
#define _IQ16div2(A)   ((A) >> 1)
#define _IQ16div4(A)   ((A) >> 2)
#define _IQ16div8(A)   ((A) >> 3)

#define _IQmpy2(A)     _IQ16mpy2(A)
#define _IQmpy4(A)     _IQ16mpy4(A)
#define _IQmpy8(A)     _IQ16mpy8(A)
#define _IQdiv2(A)     _IQ16div2(A)
#define _IQdiv4(A)     _IQ16div4(A)
#define _IQdiv8(A)     _IQ16div8(A)

//*****************************************************************************
// Returns the integer portion of an IQ16 number
//*****************************************************************************
#define _IQ16int(A)    ((A) >> 16)
#define _IQint(A)      _IQ16int(A)

//*****************************************************************************
// Computes the fractional portion of an IQ16 number
//*****************************************************************************
#define _IQ16frac(A)   ((A) & 0x0000FFFF)
#define _IQfrac(A)     _IQ16frac(A)

//*****************************************************************************
// Computes the absolute value of an IQ16 number
//*****************************************************************************
#define _IQ16abs(A)    (((A) < 0) ? -(A) : (A))
#define _IQabs(A)      _IQ16abs(A)

//*****************************************************************************
// Saturates an IQ16 number in a given range
//*****************************************************************************
#define _IQ16sat(A, Pos, Neg) (((A) > (Pos)) ? (Pos) : (((A) < (Neg)) ? (Neg) : (A)))
#define _IQsat(A, Pos, Neg)   _IQ16sat(A, Pos, Neg)

//*****************************************************************************
// Common constants in IQ16 format
//*****************************************************************************
#define _IQ16_PI      _IQ16(3.14159265359)
#define _IQ16_2PI     _IQ16(6.28318530718)
#define _IQ16_PI_2    _IQ16(1.57079632679)

#define _IQ_PI        _IQ16_PI
#define _IQ_2PI       _IQ16_2PI
#define _IQ_PI_2      _IQ16_PI_2

#endif // __IQMATH_MSP430_H__
