#pragma once

#include "math.h"

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef int fixed4_t;
typedef int fixed8_t;
typedef int fixed16_t;

extern vec2_t vec2_origin;
extern vec3_t vec3_origin;
extern vec4_t vec4_origin;

#define Vector2Copy(a,b) ((b)[0]=(a)[0],(b)[1]=(a)[1])

#define DotProduct(a,b)         ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VectorSubtract(a,b,c)   ((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)        ((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)         ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])

#define	VectorScale(v, s, o)    ((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define VectorMA(v, s, b, o)    ((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define CrossProduct(a,b,c)     ((c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1],(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2],(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0])

#define DotProduct4( x,y )        ( ( x )[0] * ( y )[0] + ( x )[1] * ( y )[1] + ( x )[2] * ( y )[2] + ( x )[3] * ( y )[3] )
#define VectorSubtract4( a,b,c )  ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1],( c )[2] = ( a )[2] - ( b )[2],( c )[3] = ( a )[3] - ( b )[3] )
#define VectorAdd4( a,b,c )       ( ( c )[0] = ( a )[0] + ( b )[0],( c )[1] = ( a )[1] + ( b )[1],( c )[2] = ( a )[2] + ( b )[2],( c )[3] = ( a )[3] + ( b )[3] )
#define VectorCopy4( a,b )        ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2],( b )[3] = ( a )[3] )
#define VectorScale4( v, s, o )   ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ),( o )[2] = ( v )[2] * ( s ),( o )[3] = ( v )[3] * ( s ) )
#define VectorMA4( v, s, b, o )   ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ),( o )[3] = ( v )[3] + ( b )[3] * ( s ) )

#define VectorClear(a)		((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate( a,b )       ( ( b )[0] = -( a )[0],( b )[1] = -( a )[1],( b )[2] = -( a )[2] )
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy( a,b )        ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2],( b )[3] = ( a )[3] )

#define SnapVector( v ) {v[0] = (int)v[0]; v[1] = (int)v[1]; v[2] = (int)v[2];}

#define	ANGLE2SHORT(x)	((int)((x)*65536.0f/360.0f) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

float Q_fabs( float f );

// angle indexes
#define PITCH               0       // up / down
#define YAW                 1       // left / right
#define ROLL                2       // fall over

/*
==============
I_fmax
==============
*/
inline float I_fmax(float x, float y)
{
	if ((float)(x - y) < 0.0)
	{
		return y;
	}
	else
	{
		return x;
	}
}

/*
==============
I_fmin
==============
*/
inline float I_fmin(float x, float y)
{
	if ((float)(y - x) < 0.0)
	{
		return y;
	}
	else
	{
		return x;
	}
}

/*
==============
I_fclamp
==============
*/
inline float I_fclamp(float val, float min, float max)
{
	return I_fmax(min, I_fmin(val, max));
}

/*
==============
I_max
==============
*/
inline int I_max(int x, int y)
{
	if (x < y)
	{
		return y;
	}
	return x;
}

/*
==============
I_min
==============
*/
inline int I_min(int x, int y)
{
	if (y >= x)
	{
		return x;
	}
	return y;
}

int VectorCompare(const vec3_t v1, const vec3_t v2);
int Vector2Compare(const vec2_t v1, const vec2_t v2);
int Vector4Compare(const vec4_t v1, const vec4_t v2);
vec_t VectorLength( const vec3_t v );
vec_t Vec2Length( const vec2_t v );
vec_t VectorLengthSquared( const vec3_t v );
vec_t Vec4LengthSq( const vec4_t v);
void VectorInverse( vec3_t v );
void MatrixTransformVector(const float *in1, const float *in2, float *out);
void MatrixInverse(const float *in, float *out);
void MatrixMultiply( float in1[3][3], float in2[3][3], float out[3][3] );
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );