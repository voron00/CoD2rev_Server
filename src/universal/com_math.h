#pragma once

#include <limits.h>

#ifdef USE_SSE2
#include <emmintrin.h>
#include "../misc/sse_mathfun.h"
#endif

typedef unsigned char byte;

typedef float vec_t;

typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

typedef int fixed4_t;
typedef int fixed8_t;
typedef int fixed16_t;

extern vec2_t vec2_origin;
extern vec3_t vec3_origin;
extern vec4_t vec4_origin;

#define IS_NAN isnan

// angle indexes
#define PITCH               0       // up / down
#define YAW                 1       // left / right
#define ROLL                2       // fall over

#define DEG2RAD( a ) ( ( ( a ) * M_PI ) / 180.0f )
#define RAD2DEG( a ) ( ( ( a ) * 180.0f ) / M_PI )

#define DEG_TO_RAD      ( M_PI / 180.0f )
#define HALF_DEG_TO_RAD ( M_PI / 360.0f )
#define HALF_PI         ( M_PI_2 )
#define PI              ( M_PI )
#define QUARTER_PI      ( M_PI_4 )
#define RAD_TO_DEG      ( 180.0f / M_PI )
#define SQUARED_PI      ( M_PI * M_PI )
#define THREE_PI_HALVES ( 1.5f * M_PI )
#define TWO_PI          ( 2.0f * M_PI )

#define NUMVERTEXNORMALS 162
extern vec3_t bytedirs[NUMVERTEXNORMALS];

/* Converts from rand() to range 0-1 ie: 1/(2^15-1) */
#define RANDSCALE       ( 1.0f / 32768.0f )

#define Square( x ) ( ( x ) * ( x ) )

#define ANGLE2SHORT( x )  ( (int)( ( x ) * 65536 / 360 ) & 65535 )
#define SHORT2ANGLE( x )  ( ( x ) * ( 360.0 / 65536 ) )

#define SnapVector( v ) {v[0] = ( (int)( v[0] ) ); v[1] = ( (int)( v[1] ) ); v[2] = ( (int)( v[2] ) );}

#define lerp(a, b, t) ((a) + (t) * ((b) - (a)))

#define MAX_SHORT_PRODUCT_SQUARE ( Square ( SHRT_MAX ) )

#define VEC_EPSILON         0.001f
#define NORMALIZE_EPSILON   0.002f
#define SIZE_EPSILON        0.0001f

float Q_acos( float c );

/*
==============
Q_fabs
==============
*/
inline float Q_fabs( float f )
{
	int tmp = ( *(int*)&f ) & 0x7FFFFFFF;
	return *(float*)&tmp;
}

/*
==============
Q_rint
==============
*/
inline int Q_rint( float in )
{
	return floor( in + 0.5 );
}

/*
==============
I_sqrt
==============
*/
inline float I_sqrt( const float x )
{
#ifdef USE_SSE2
	v4sf vx = _mm_set_ps1(x);
	return _mm_sqrt_ps(vx)[0];
#else
	return sqrtf( x );
#endif
}

/*
==============
I_rsqrt
==============
*/
inline float I_rsqrt( const float x )
{
#ifdef USE_SSE2
	v4sf vx = _mm_set_ps1(x);
	return _mm_rsqrt_ps(vx)[0];
#else
	return 1.0f / sqrtf( x );
#endif
}

/*
==============
Vec4Length
==============
*/
inline float Vec4Length( const vec4_t v )
{
	return I_sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3] );
}

/*
==============
Vec4LengthSq
==============
*/
inline float Vec4LengthSq( const vec4_t v )
{
	return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3] );
}

/*
==============
VectorCompare
==============
*/
inline bool VectorCompare( const vec3_t v1, const vec3_t v2 )
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
		return false;

	return true;
}

/*
==============
Vector2Compare
==============
*/
inline bool Vector2Compare( const vec2_t v1, const vec2_t v2 )
{
	if (v1[0] != v2[0] || v1[1] != v2[1])
		return false;

	return true;
}

/*
==============
Vector4Compare
==============
*/
inline bool Vector4Compare( const vec4_t v1, const vec4_t v2 )
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2] || v1[3] != v2[3])
		return false;

	return true;
}

/*
==============
VectorLength
==============
*/
inline float VectorLength( const vec3_t v )
{
	return I_sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

/*
==============
Vec2Length
==============
*/
inline float Vec2Length( const vec2_t v )
{
	return I_sqrt( v[0] * v[0] + v[1] * v[1] );
}

/*
==============
VectorLengthSquared
==============
*/
inline float VectorLengthSquared( const vec3_t v )
{
	return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

/*
==============
Vec2LengthSq
==============
*/
inline float Vec2LengthSq( const vec2_t v )
{
	return ( v[0] * v[0] + v[1] * v[1] );
}

/*
==============
DotProduct
==============
*/
inline float DotProduct( const vec3_t a, const vec3_t b )
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/*
==============
Dot2Product
==============
*/
inline float Dot2Product( const vec2_t a, const vec2_t b )
{
	return a[0] * b[0] + a[1] * b[1];
}

/*
==============
DotProduct4
==============
*/
inline float DotProduct4( const vec4_t a, const vec4_t b )
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

/*
==============
VectorSubtract
==============
*/
inline void VectorSubtract( const vec3_t a, const vec3_t b, vec3_t c )
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
}

/*
==============
Vector2Subtract
==============
*/
inline void Vector2Subtract( const vec2_t a, const vec2_t b, vec2_t c )
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
}

/*
==============
VectorSubtract4
==============
*/
inline void VectorSubtract4( const vec4_t a, const vec4_t b, vec4_t c )
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
	c[3] = a[3] - b[3];
}

/*
==============
VectorAdd
==============
*/
inline void VectorAdd( const vec3_t a, const vec3_t b, vec3_t c )
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
}

/*
==============
Vector2Add
==============
*/
inline void Vector2Add( const vec2_t a, const vec2_t b, vec2_t c )
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
}

/*
==============
VectorAdd4
==============
*/
inline void VectorAdd4( const vec4_t a, const vec4_t b, vec4_t c )
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
	c[3] = a[3] + b[3];
}

/*
==============
VectorCopy
==============
*/
inline void VectorCopy( const vec3_t a, vec3_t b )
{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
}

/*
==============
Vector2Copy
==============
*/
inline void Vector2Copy( const vec2_t a, vec2_t b )
{
	b[0] = a[0];
	b[1] = a[1];
}

/*
==============
VectorCopy4
==============
*/
inline void VectorCopy4( const vec4_t a, vec4_t b )
{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
	b[3] = a[3];
}

/*
==============
Vec2Multiply
==============
*/
inline float Vec2Multiply( const vec2_t v )
{
	return v[0] * v[0] + v[1] * v[1];
}

/*
==============
VectorScale
==============
*/
inline void VectorScale( const vec3_t v, const float s, vec3_t o )
{
	o[0] = s * v[0];
	o[1] = s * v[1];
	o[2] = s * v[2];
}

/*
==============
Vec2Scale
==============
*/
inline void Vec2Scale( const vec2_t v, const float s, vec2_t o )
{
	o[0] = s * v[0];
	o[1] = s * v[1];
}

/*
==============
VectorScale4
==============
*/
inline void VectorScale4( const vec4_t v, const float s, vec4_t o )
{
	o[0] = s * v[0];
	o[1] = s * v[1];
	o[2] = s * v[2];
	o[3] = s * v[3];
}

/*
==============
VectorMA
==============
*/
inline void VectorMA( const vec3_t v, const float s, const vec3_t b, vec3_t o )
{
	o[0] = s * b[0] + v[0];
	o[1] = s * b[1] + v[1];
	o[2] = s * b[2] + v[2];
}

/*
==============
VectorMA2
==============
*/
inline void VectorMA2( const vec2_t v, const float s, const vec2_t b, vec2_t o )
{
	o[0] = s * b[0] + v[0];
	o[1] = s * b[1] + v[1];
}

/*
==============
VectorMA4
==============
*/
inline void VectorMA4( const vec4_t v, const float s, const vec4_t b, vec4_t o )
{
	o[0] = s * b[0] + v[0];
	o[1] = s * b[1] + v[1];
	o[2] = s * b[2] + v[2];
	o[3] = s * b[3] + v[3];
}

/*
==============
VectorClear
==============
*/
inline void VectorClear( vec3_t v )
{
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;
}

/*
==============
Vector2Clear
==============
*/
inline void Vector2Clear( vec2_t v )
{
	v[0] = 0;
	v[1] = 0;
}

/*
==============
Vector4Clear
==============
*/
inline void Vector4Clear( vec4_t v )
{
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;
	v[3] = 0;
}

/*
==============
VectorSet
==============
*/
inline void VectorSet( vec3_t v, const float x, const float y, const float z )
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

/*
==============
VectorInverse
==============
*/
inline void VectorInverse( vec3_t v )
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

/*
==============
VectorNegate
==============
*/
inline void VectorNegate( const vec3_t a, vec3_t b )
{
	b[0] = -a[0];
	b[1] = -a[1];
	b[2] = -a[2];
}

/*
==============
I_fsel
==============
*/
inline float I_fsel( const float x, const float y, const float z )
{
	if ( x < 0.0f )
	{
		return z;
	}

	return y;
}

/*
==============
I_fmax
==============
*/
inline float I_fmax( const float x, const float y )
{
	float z = x - y;

	return I_fsel( z, x, y );
}

/*
==============
I_fmin
==============
*/
inline float I_fmin( const float x, const float y )
{
	float z = y - x;

	return I_fsel( z, x, y );
}

/*
==============
I_fclamp
==============
*/
inline float I_fclamp( const float val, const float min, const float max )
{
	return I_fmax( min, I_fmin( val, max ) );
}

/*
==============
I_sel
==============
*/
inline int I_sel( const int x, const int y, const int z )
{
	if ( x < 0.0f )
	{
		return z;
	}

	return y;
}

/*
==============
I_max
==============
*/
inline int I_max( const int x, const int y )
{
	int z = x - y;

	return I_sel( z, x, y );
}

/*
==============
I_min
==============
*/
inline int I_min( const int x, const int y )
{
	int z = y - x;

	return I_sel( z, x, y );
}

/*
==============
I_clamp
==============
*/
inline int I_clamp( const int val, const int min, const int max )
{
	return I_max( min, I_min( val, max ) );
}

/*
==============
I_sgn
==============
*/
inline float I_sgn(const float x)
{
	if ( x < 0.0f )
	{
		return -1.0f;
	}

	return 1.0f;
}

/*
==============
I_side
==============
*/
inline int I_side( const float x )
{
	return x >= 0.0f;
}

/*
==============
I_fabs
==============
*/
inline float I_fabs( const float x )
{
#ifdef USE_SSE2
	// fill the registers
	v4sf vx = _mm_set_ps1(x);
	// -0.f in binary has only the sign bit (bit 31) set to 1
	v4sf mask = _mm_set1_ps(-0.f);
	// Clear the sign bit using bitwise AND-NOT
	return _mm_andnot_ps(mask, vx)[0];
#else
	return fabs( x );
#endif
}

/*
==============
FastSinCosDouble
==============
*/
inline void FastSinCosDouble( double x, double *s, double *c )
{
#ifdef USE_SSE2
	// fill the registers
	v4sf vx = _mm_set_ps1(x);
	v4sf vs = _mm_set_ps1(*s);
	v4sf vc = _mm_set_ps1(*c);

	// perform calculations
	sincos_ps(vx, &vs, &vc);

	// copy result
	*s = vs[0];
	*c = vc[0];
#else
	*s = sin(x);
	*c = cos(x);
#endif
}

/*
==============
FastSinCos
==============
*/
inline void FastSinCos( float x, float *s, float *c )
{
#ifdef USE_SSE2
	// fill the registers
	v4sf vx = _mm_set_ps1(x);
	v4sf vs = _mm_set_ps1(*s);
	v4sf vc = _mm_set_ps1(*c);

	// perform calculations
	sincos_ps(vx, &vs, &vc);

	// copy result
	*s = vs[0];
	*c = vc[0];
#else
	*s = sinf(x);
	*c = cosf(x);
#endif
}

/*
==============
Vec3Avg
==============
*/
inline void Vec3Avg( const vec3_t a, const vec3_t b, vec3_t sum )
{
	sum[0] = (a[0] + b[0]) * 0.5f;
	sum[1] = (a[1] + b[1]) * 0.5f;
	sum[2] = (a[2] + b[2]) * 0.5f;
}

/*
==============
Vec3IsNormalized
==============
*/
inline bool Vec3IsNormalized( const vec3_t v )
{
	if ( I_fabs( VectorLengthSquared( v ) - 1.0f ) < NORMALIZE_EPSILON )
	{
		return true;
	}
	return false;
}

/*
==============
Vec4IsNormalized
==============
*/
inline bool Vec4IsNormalized( const vec4_t v )
{
	if ( I_fabs( Vec4LengthSq( v ) - 1.0f ) < NORMALIZE_EPSILON )
	{
		return true;
	}
	return false;
}

/*
=================
Vec3Lerp
=================
*/
inline void Vec3Lerp( const vec3_t start, const vec3_t end, float fraction, vec3_t endpos )
{
	endpos[0] = (end[0] - start[0]) * fraction + start[0];
	endpos[1] = (end[1] - start[1]) * fraction + start[1];
	endpos[2] = (end[2] - start[2]) * fraction + start[2];
}

/*
==============
FloatIsNegative
==============
*/
inline int const FloatIsNegative( const float x )
{
	return x < 0.0f;
}

/*
==============
FloatSign
==============
*/
inline int const FloatSign( const float x )
{
	return -2 * FloatIsNegative( x ) + 1;
}

/*
==============
FastCeil
==============
*/
inline int const FastCeil( const float x )
{
	return ceil( x );
}

#define Vec3Length VectorLength

void MatrixTransformVector(const vec3_t in1, const vec3_t in2[3], vec3_t out);
void MatrixTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out);
void MatrixTransposeTransformVector(const vec3_t in1, const vec3_t in2[3], vec3_t out);
void MatrixTransposeTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out);
void MatrixInverse(const float in1[3][3], float out[3][3]);
void MatrixMultiply( const float in1[3][3], const float in2[3][3], float out[3][3] );
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
vec_t Vec2Normalize( vec3_t v );
vec_t Vec3Normalize( vec3_t v );
vec_t Vec4Normalize( vec4_t v );
vec_t Vec2NormalizeTo( const vec2_t v, vec2_t out );
vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out );
void Vec3Cross(const vec3_t v0, const vec3_t v1, vec3_t cross);
float AngleNormalize360( float angle );
float AngleNormalize180( float angle );
float AngleDelta( float angle1, float angle2 );
int BoxOnPlaneSide( vec3_t emins, vec3_t emaxs, const struct cplane_s *p );
float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
float RadiusFromBounds2D( const vec2_t mins, const vec2_t maxs );
void SnapAngles(vec3_t angles);
void vectoangles( const vec3_t value1, vec3_t angles );
void AxisToAngles( vec3_t axis[3], vec3_t angles );
float AngleMod( float a );
float AngleSubtract( float a1, float a2 );
vec_t vectosignedyaw(const vec3_t vec);
int BoxDistSqrdExceeds(const vec3_t absmin, const vec3_t absmax, const vec3_t org, float fogOpaqueDistSqrd);
float Vec3Distance(const vec3_t v1, const vec3_t v2);
float Vec3DistanceSq(const vec3_t v1, const vec3_t v2);
float DiffTrack(float tgt, float cur, float rate, float deltaTime);
float DiffTrackAngle(float tgt, float cur, float rate, float deltaTime);
float AngleNormalize180Accurate(float angle);
float AngleNormalize360Accurate(float angle);
void VectorAngleMultiply(vec3_t vec, float angle);
float vectoyaw( const vec3_t vec );
float vectopitch( const vec3_t vec );
float PitchForYawOnNormal(const float fYaw, const vec3_t normal);
float Abs(const vec3_t v);
void YawVectors2D(const float yaw, vec2_t forward, vec2_t right);
void YawVectors(const float yaw, vec3_t forward, vec3_t right);
void ShrinkBoundsToHeight(vec3_t mins, vec3_t maxs);
void ClearBounds( vec3_t mins, vec3_t maxs );
void AddPointToBounds(const vec3_t v, vec3_t mins, vec3_t maxs);
void AddPointToBounds2D(const vec2_t v, vec2_t mins, vec2_t maxs);
byte DirToByte( vec3_t dir );
void ByteToDir( int b, vec3_t dir );
void Rand_Init(int seed);
float flrand(float min, float max);
int irand(int min, int max);
float RotationToYaw(const vec2_t rot);
void MatrixTranspose(const float in[3][3], float out[3][3]);
void MatrixMultiply43(const float in1[4][3], const float in2[4][3], float out[4][3]);
void MatrixInverseOrthogonal43(const float in[4][3], float out[4][3]);
void ExpandBoundsToWidth(vec3_t mins, vec3_t maxs);
void YawToAxis(float yaw, vec3_t axis[3]);
void ProjectPointOnPlane(const vec3_t p, const vec3_t normal, vec3_t dst);
void RoundFloatArray(vec3_t x, vec3_t y);
void TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] );
void RotatePoint( vec3_t point, const vec3_t matrix[3] );
void AnglesSubtract( vec3_t v1, vec3_t v2, vec3_t v3 );
float Vec2DistanceSq(const vec2_t v1, const vec2_t v2);
float Vec2Distance(const vec2_t v1, const vec2_t v2);
void CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] );
bool Vec3IsNormalized(const vec3_t v);
bool Vec4IsNormalized(const vec4_t v);
