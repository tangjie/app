
#ifndef BASE_BASE_TYPES_H__
#define BASE_BASE_TYPES_H__

typedef signed char		int8_t;
typedef signed short	int16_t;
typedef signed int		int32_t;
typedef signed __int64		int64_t;
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned __int64	uint64_t;

//since almost cast is very useful, so we include it here.
#include "base/memory/casts.h"

#endif	// BASE_BASE_TYPES_H__
