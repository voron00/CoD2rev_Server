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

#define DotProduct(a,b)         ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define Dot2Product(a,b)        ((a)[0]*(b)[0]+(a)[1]*(b)[1])
#define VectorSubtract(a,b,c)   ((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define Vector2Subtract(a,b,c)  ((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1])
#define VectorAdd(a,b,c)        ((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define Vector2Add(a,b,c)       ((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1])
#define VectorCopy(a,b)         ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define Vector2Copy(a,b)        ((b)[0]=(a)[0],(b)[1]=(a)[1])
#define Vec2Multiply(v)         ((v)[0]*(v)[0]+(v)[1]*(v)[1])

#define	VectorScale(v, s, o)    ((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	Vec2Scale(v, s, o)      ((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s))
#define VectorMA(v, s, b, o)    ((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define CrossProduct(a,b,c)     ((c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1],(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2],(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0])

#define DotProduct4( x,y )        ( ( x )[0] * ( y )[0] + ( x )[1] * ( y )[1] + ( x )[2] * ( y )[2] + ( x )[3] * ( y )[3] )
#define VectorSubtract4( a,b,c )  ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1],( c )[2] = ( a )[2] - ( b )[2],( c )[3] = ( a )[3] - ( b )[3] )
#define VectorAdd4( a,b,c )       ( ( c )[0] = ( a )[0] + ( b )[0],( c )[1] = ( a )[1] + ( b )[1],( c )[2] = ( a )[2] + ( b )[2],( c )[3] = ( a )[3] + ( b )[3] )
#define VectorCopy4( a,b )        ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2],( b )[3] = ( a )[3] )
#define VectorScale4( v, s, o )   ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ),( o )[2] = ( v )[2] * ( s ),( o )[3] = ( v )[3] * ( s ) )
#define VectorMA4( v, s, b, o )   ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ),( o )[3] = ( v )[3] + ( b )[3] * ( s ) )

#define VectorClear(a)		      ((a)[0]=(a)[1]=(a)[2]=0)
#define Vector2Clear(a)		      ((a)[0]=(a)[1]=0)
#define VectorNegate( a,b )       ( ( b )[0] = -( a )[0],( b )[1] = -( a )[1],( b )[2] = -( a )[2] )
#define VectorSet(v, x, y, z)	  ((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy( a,b )        ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2],( b )[3] = ( a )[3] )

#define SnapVector( v ) {v[0] = (int)v[0]; v[1] = (int)v[1]; v[2] = (int)v[2];}

#define	ANGLE2SHORT(x)	((int)((x)*65536.0f/360.0f) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

#define Square( x ) ( ( x ) * ( x ) )

float Q_fabs( float f );

// angle indexes
#define PITCH               0       // up / down
#define YAW                 1       // left / right
#define ROLL                2       // fall over

/*
==============
I_fpos
==============
*/
inline bool I_fispos(float x)
{
	return x >= 0.0;
}

/*
==============
I_fneg
==============
*/
inline bool I_fisneg(float x)
{
	return x < 0.0;
}

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

/*
==============
I_rsqrt
==============
*/
inline float I_rsqrt(const float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;                        // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck?
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

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
void MatrixTransformVector43(const float *in1, const float *in2, float *out);
void MatrixTransposeTransformVector(const float *in1, const vec3_t in2[3], float *out);
void MatrixTransposeTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out);
void MatrixInverse(const float *in, float *out);
void MatrixMultiply( float in1[3][3], float in2[3][3], float out[3][3] );
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
vec_t Vec2Normalize( vec3_t v );
vec_t Vec3Normalize( vec3_t v );
vec_t Vec4Normalize( vec4_t v );
double Vec2NormalizeTo(const float *v, float *out);
vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out );
void Vec3Cross(const vec3_t v0, const vec3_t v1, vec3_t cross);
float AngleNormalize360( float angle );
float AngleNormalize180( float angle );
float AngleDelta( float angle1, float angle2 );
int BoxOnPlaneSide( vec3_t emins, vec3_t emaxs, struct cplane_s *p );
float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
float RadiusFromBounds2D( const vec3_t mins, const vec3_t maxs );
vec_t Q_rint( vec_t in );
void SnapAngles(vec3_t angles);
void vectoangles( const vec3_t value1, vec3_t angles );
void AxisToAngles( vec3_t axis[3], vec3_t angles );
float AngleMod( float a );
float AngleSubtract( float a1, float a2 );
double vectosignedyaw(float *vec);