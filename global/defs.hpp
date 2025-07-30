#pragma once

using vec_t = float;
using vec2_t = float[2];
using vec3_t = float[3];
using vec4_t = float[4];


#define PI 3.14159265f

#ifndef M_PI
#define M_PI        3.14159265358979323846f // matches value in gcc v2 math.h
#endif

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorAddAll(a,b,c)		((c)[0]=(a)[0]+(b),(c)[1]=(a)[1]+(b),(c)[2]=(a)[2]+(b))
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define VectorScaleAll(a,b,c)	((c)[0]=(a)[0]*(b),(c)[1]=(a)[1]*(b),(c)[2]=(a)[2]*(b))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorClear( a )              ( ( a )[0] = ( a )[1] = ( a )[2] = 0 )

#define ANGLE2SHORT( x )  ( (int)( ( x ) * 65536 / 360 ) & 65535 )
#define SHORT2ANGLE( x )  ( ( x ) * ( 360.0f / 65536 ) )

#define DEG2RAD(a) (((a) * M_PI) / 180.0F)
#define RAD2DEG(a) (((a) * 180.0f) / M_PI)
#define RAD2SHORT(a) ((a) * (32768.f / (float)M_PI))
#define SHORT2RAD(a) ((a) * ((float)M_PI / 32768.f))
#define SHORT2DEG(a) (((a) / 32768.f) * 180.0f)

#ifndef NONCOPYABLE
#define NONCOPYABLE(TypeName) \
	TypeName(TypeName&&) = delete; \
	TypeName(const TypeName&) = delete; \
	TypeName& operator=(const TypeName&) = delete; \
	TypeName& operator=(TypeName&&) = delete;
#endif
