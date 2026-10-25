#include "../qcommon/qcommon.h"

vec2_t vec2_origin = { 0, 0 };
vec3_t vec3_origin = { 0, 0, 0 };
vec4_t vec4_origin = { 0, 0, 0, 0 };

static unsigned int holdrand = 0x89ABCDEF;

/*
==============
irand
==============
*/
int irand( int min, int max )
{
	holdrand = 214013 * holdrand + 2531011;
	return ((int)((holdrand >> 17) * (max - min)) >> 15) + min;
}

/*
==============
flrand
==============
*/
float flrand( float min, float max )
{
	holdrand = 214013 * holdrand + 2531011;
	return ((float)((holdrand >> 17) * (max - min)) / 32768.0f) + min;
}

/*
==============
Rand_Init
==============
*/
void Rand_Init( int seed )
{
	holdrand = seed;
}

/*
==============
AxisClear
==============
*/
void AxisClear( vec3_t axis[3] )
{
	axis[0][0] = 1.0f;
	axis[0][1] = 0.0f;
	axis[0][2] = 0.0f;
	axis[1][0] = 0.0f;
	axis[1][1] = 1.0f;
	axis[1][2] = 0.0f;
	axis[2][0] = 0.0f;
	axis[2][1] = 0.0f;
	axis[2][2] = 1.0f;
}

/*
==============
ExtendBounds
==============
*/
void ExtendBounds( vec3_t mins, vec3_t maxs, const vec3_t offset )
{
	if ( offset[0] > 0.0f )
		maxs[0] = maxs[0] + offset[0];
	else
		mins[0] = mins[0] + offset[0];

	if ( offset[1] > 0.0f )
		maxs[1] = maxs[1] + offset[1];
	else
		mins[1] = mins[1] + offset[1];

	if ( offset[2] > 0.0f )
		maxs[2] = maxs[2] + offset[2];
	else
		mins[2] = mins[2] + offset[2];
}

/*
==============
AngleNormalize180Accurate
==============
*/
float AngleNormalize180Accurate( float angle )
{
	if ( angle <= -180.0 )
	{
		do
			angle = angle + 360.0;
		while ( angle <= -180.0 );

		return angle;
	}

	if ( angle > 180.0 )
	{
		do
			angle = angle - 360.0;
		while ( angle > 180.0 );

		return angle;
	}

	return angle;
}

/*
==============
AngleNormalize360Accurate
==============
*/
float AngleNormalize360Accurate( float angle )
{
	if ( angle < 0.0 )
	{
		do
			angle = angle + 360.0;
		while ( angle < 0.0 );

		return angle;
	}

	if ( angle >= 360.0 )
	{
		do
			angle = angle - 360.0;
		while ( angle >= 360.0 );

		return angle;
	}

	return angle;
}

/*
=================
AngleNormalize360

returns angle normalized to the range [0 <= angle < 360]
=================
*/
float AngleNormalize360( float angle )
{
	return (360.0 / 65536) * ((int)(angle * (65536 / 360.0)) & 65535);
}

/*
=================
AngleSubtract

Always returns a value from -180 to 180
=================
*/
float AngleSubtract( float a1, float a2 )
{
	float	a;

	a = a1 - a2;
	while ( a > 180 )
	{
		a -= 360;
	}
	while ( a < -180 )
	{
		a += 360;
	}
	return a;
}

/*
===============
LerpAngle
===============
*/
float LerpAngle( float from, float to, float frac )
{
	float	a;

	if ( to - from > 180 )
	{
		to -= 360;
	}
	if ( to - from < -180 )
	{
		to += 360;
	}
	a = from + frac * (to - from);

	return a;
}

/*
===============
AngleMod
===============
*/
float AngleMod(float a )
{
	a = (360.0/65536) * ((int)(a*(65536/360.0)) & 65535);
	return a;
}

/*
===============
ColorBytes4
===============
*/
unsigned ColorBytes4( float r, float g, float b, float a )
{
	unsigned	i;

	( (byte *)&i )[0] = r * 255;
	( (byte *)&i )[1] = g * 255;
	( (byte *)&i )[2] = b * 255;
	( (byte *)&i )[3] = a * 255;

	return i;
}

/*
===============
ColorBytes3
===============
*/
unsigned ColorBytes3( float r, float g, float b )
{
	unsigned	i;

	( (byte *)&i )[0] = r * 255;
	( (byte *)&i )[1] = g * 255;
	( (byte *)&i )[2] = b * 255;

	return i;
}

/*
==============
QuatEigenTrace
==============
*/
float QuatEigenTrace( vec4_t quat )
{
	float xx, yy, zz, magSqr;

	xx = Square( quat[0] );
	yy = Square( quat[1] );
	zz = Square( quat[2] );

	magSqr = ( (xx + yy ) + zz ) + ( Square( quat[3] ) );

	if ( magSqr == 0.0f )
	{
		return 0.0f;
	}

	xx *= 1.0f / magSqr;
	yy *= 1.0f / magSqr;
	zz *= 1.0f / magSqr;

	return ( xx + yy + zz );
}

/*
==============
QuatToAxis
==============
*/
void QuatToAxis( const vec4_t quat, vec3_t axis[3] )
{
	float yy, xy, zz, zw, scaledX, scaledY, yw, xz, yz, xx, xw, magSqr;

	xx = Square( quat[0] );
	yy = Square( quat[1] );
	zz = Square( quat[2] );

	magSqr = ( (xx + yy ) + zz ) + ( Square( quat[3] ) );
	assert(magSqr > 0.0f);

	xx *= 2.0f / magSqr;
	yy *= 2.0f / magSqr;
	zz *= 2.0f / magSqr;

	scaledX = ( 2.0f / magSqr ) * quat[0];

	xy = scaledX * quat[1];
	xz = scaledX * quat[2];
	xw = scaledX * quat[3];

	scaledY = ( 2.0f / magSqr ) * quat[1];

	yz = scaledY * quat[2];
	yw = scaledY * quat[3];

	zw = ( (2.0f / magSqr ) * quat[2] ) * quat[3];

	axis[0][0] = 1.0f - ( yy + zz );

	axis[0][1] = xy + zw;
	axis[0][2] = xz - yw;
	axis[1][0] = xy - zw;

	axis[1][1] = 1.0f - ( xx + zz );

	axis[1][2] = yz + xw;
	axis[2][0] = xz + yw;
	axis[2][1] = yz - xw;

	axis[2][2] = 1.0f - ( xx + yy );
}

/*
==============
QuatInverse
==============
*/
void QuatInverse( const vec4_t in, vec4_t out )
{
	out[0] = -in[0];
	out[1] = -in[1];
	out[2] = -in[2];
	out[3] = in[3];
}

/*
==================
QuatMultiply
==================
*/
void QuatMultiply( const vec4_t in1, const vec4_t in2, vec4_t out )
{
	out[0] = in1[0] * in2[3] + in1[3] * in2[0] + in1[2] * in2[1] - in1[1] * in2[2];
	out[1] = in1[1] * in2[3] - in1[2] * in2[0] + in1[3] * in2[1] + in1[0] * in2[2];
	out[2] = in1[2] * in2[3] + in1[1] * in2[0] - in1[0] * in2[1] + in1[3] * in2[2];
	out[3] = in1[3] * in2[3] - in1[0] * in2[0] - in1[1] * in2[1] - in1[2] * in2[2];
}

/*
==================
MatrixTransformVector43Equals
==================
*/
void MatrixTransformVector43Equals( vec3_t out, const float in[4][3] )
{
	float temp[2];

	temp[0] = ( ( ( out[0] * in[0][0] ) + ( out[1] * in[1][0] ) ) + ( out[2] * in[2][0] ) ) + in[3][0];
	temp[1] = ( ( ( out[0] * in[0][1] ) + ( out[1] * in[1][1] ) ) + ( out[2] * in[2][1] ) ) + in[3][1];

	out[2] =  ( ( ( out[0] * in[0][2] ) + ( out[1] * in[1][2] ) ) + ( out[2] * in[2][2] ) ) + in[3][2];

	out[0] = temp[0];
	out[1] = temp[1];
}

/*
==================
MatrixTransformVector43
==================
*/
void MatrixTransformVector43( const vec3_t in1, const float in2[4][3], vec3_t out )
{
	assert(in1 != out);

	out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0] + in2[3][0];
	out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1] + in2[3][1];
	out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2] + in2[3][2];
}

/*
==================
MatrixTransposeTransformVector
==================
*/
void MatrixTransposeTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out )
{
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[0][1] + in1[2] * in2[0][2];
	out[1] = in1[0] * in2[1][0] + in1[1] * in2[1][1] + in1[2] * in2[1][2];
	out[2] = in1[0] * in2[2][0] + in1[1] * in2[2][1] + in1[2] * in2[2][2];
}

/*
==================
Vec3RotateTranspose
==================
*/
void Vec3RotateTranspose( const vec3_t in, const vec3_t matrix[3], vec3_t out )
{
	assert(in != out);

	out[0] = ( in[0] * matrix[0][0] ) + ( in[1] * matrix[1][0] ) + ( in[2] * matrix[2][0] );
	out[1] = ( in[0] * matrix[0][1] ) + ( in[1] * matrix[1][1] ) + ( in[2] * matrix[2][1] );
	out[2] = ( in[0] * matrix[0][2] ) + ( in[1] * matrix[1][2] ) + ( in[2] * matrix[2][2] );
}

/*
==================
Vec3Rotate
==================
*/
void Vec3Rotate( const vec3_t in, const vec3_t matrix[3], vec3_t out )
{
	assert(in != out);

	out[0] = ( in[0] * matrix[0][0] ) + ( in[1] * matrix[0][1] ) + ( in[2] * matrix[0][2] );
	out[1] = ( in[0] * matrix[1][0] ) + ( in[1] * matrix[1][1] ) + ( in[2] * matrix[1][2] );
	out[2] = ( in[0] * matrix[2][0] ) + ( in[1] * matrix[2][1] ) + ( in[2] * matrix[2][2] );
}

/*
==================
Vec3Max
==================
*/
void Vec3Max( vec3_t a, vec3_t b, vec3_t out )
{
	vec3_t v;

	if ( a[0] > b[0] )
		v[0] = a[0];
	else
		v[0] = b[0];

	out[0] = v[0];

	if ( a[1] > b[1] )
		v[1] = a[1];
	else
		v[1] = b[1];

	out[1] = v[1];

	if ( a[2] > b[2] )
		v[2] = a[2];
	else
		v[2] = b[2];

	out[2] = v[2];
}

/*
==================
Vec3UnpackNormalized
==================
*/
void Vec3UnpackNormalized( unsigned int, vec3_t vec )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
ClampShort
==================
*/
short ClampShort( int i )
{
	if ( i < SHRT_MIN )
	{
		return SHRT_MIN;
	}

	if ( i > SHRT_MAX )
	{
		return SHRT_MAX;
	}

	return i;
}

/*
==================
ClampChar
==================
*/
char ClampChar( int i )
{
	if ( i < CHAR_MIN )
	{
		return CHAR_MIN;
	}

	if ( i > CHAR_MAX )
	{
		return CHAR_MAX;
	}

	return i;
}

/*
==================
Q_log2
==================
*/
int Q_log2( int val )
{
	int answer;

	answer = 0;
	while ( ( val>>=1 ) != 0 )
	{
		answer++;
	}
	return answer;
}

/*
==================
GraphGetValueFromFraction
==================
*/
float GraphGetValueFromFraction( int knotCount, const float (*knots)[2], float fraction ) // untested
{
	float result;
	float adjustedFrac;
	int knotIndex;

	result = 1.0f;

	assert(knots);
	assert(knotCount >= 2);
	assert(fraction >= 0.0f && fraction <= 1.0f);
	assert(knots[knotCount - 1][0] == 1.0f);

	for ( knotIndex = 1; knotIndex < knotCount; knotIndex++ )
	{
		if ( knots[knotIndex][0] >= fraction )
		{
			adjustedFrac = ( fraction - knots[knotIndex - 1][0] ) / ( knots[knotIndex][0] - knots[knotIndex - 1][0] );
			assert(adjustedFrac >= 0.0f && adjustedFrac <= 1.0f);

			assert(knots[knotIndex - 1][1] >= 0.0f && knots[knotIndex - 1][1] <= 1.0f);
			assert(knots[knotIndex][1] >= 0.0f && knots[knotIndex][1] <= 1.0f);

			result = ( ( knots[knotIndex][1] - knots[knotIndex - 1][1] ) * adjustedFrac ) + knots[knotIndex - 1][1];
			break;
		}
	}

	assert(result >= 0.0f && result <= 1.0f);
	return result;
}

/*
==================
randomf
==================
*/
float randomf( void )
{
	return rand() / (float)RAND_MAX;
}

/*
==================
ConvertMatToQuat
==================
*/
void ConvertMatToQuat( float axis[3][3],vec4_t quat )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
VectorRotateAngles
==================
*/
void VectorRotateAngles( float const * const,float const * const,float * const )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
ColorNormalize
==================
*/
float ColorNormalize( vec3_t in, vec3_t out )
{
	float max;

	max = in[0];

	if ( in[1] > in[0] )
		max = in[1];

	if ( in[2] > max )
		max = in[2];

	if ( max == 0.0f )
	{
		out[2] = 1.0f;
		out[1] = 1.0f;
		out[0] = 1.0f;

		return 0.0f;
	}

	out[0] = ( 1.0f / max ) * in[0];
	out[1] = ( 1.0f / max ) * in[1];
	out[2] = ( 1.0f / max ) * in[2];

	return max;
}

/*
==================
BoxDistSqrdExceeds
==================
*/
int BoxDistSqrdExceeds( const vec3_t absmin, const vec3_t absmax, const vec3_t org, float fogOpaqueDistSqrd )
{
	float minsSqrd, maxsSqrd;
	vec3_t mins, maxs;
	float total;
	int i;

	VectorSubtract( absmin, org, mins );
	VectorSubtract( absmax, org, maxs );

	total = 0.0f;

	for ( i = 0; i < 3; i++ )
	{
		if ( mins[i] * maxs[i] > 0.0f )
		{
			minsSqrd = mins[i] * mins[i];
			maxsSqrd = maxs[i] * maxs[i];

			if ( minsSqrd > maxsSqrd )
				total += maxsSqrd;
			else
				total += minsSqrd;
		}
	}

	return total > fogOpaqueDistSqrd;
}

/*
==================
ProjectPointOnPlane
==================
*/
void ProjectPointOnPlane( const vec3_t p, const vec3_t normal, vec3_t dst )
{
	assert( Vec3IsNormalized( normal ) );
	float d = DotProduct( normal, p );
	VectorMA( p, -d, normal, dst );
}

/*
==================
ShrinkBoundsToHeight
==================
*/
void ShrinkBoundsToHeight( vec3_t mins, vec3_t maxs )
{
	vec3_t d;

	VectorSubtract( maxs, mins, d );

	if ( d[0] > d[2] )
	{
		mins[0] += (d[0] - d[2]) * 0.5f;
		maxs[0] -= (d[0] - d[2]) * 0.5f;
	}

	if ( d[1] > d[2] )
	{
		mins[1] += (d[1] - d[2]) * 0.5f;
		maxs[1] -= (d[1] - d[2]) * 0.5f;
	}
}

/*
=================
AngleNormalize180

returns angle normalized to the range [-180 < angle <= 180]
=================
*/
float AngleNormalize180( float angle )
{
	angle = AngleNormalize360( angle );
	if ( angle > 180.0 )
	{
		angle -= 360.0;
	}
	return angle;
}

/*
=================
AnglesSubtract
=================
*/
void AnglesSubtract( vec3_t v1, vec3_t v2, vec3_t v3 )
{
	v3[0] = AngleSubtract( v1[0], v2[0] );
	v3[1] = AngleSubtract( v1[1], v2[1] );
	v3[2] = AngleSubtract( v1[2], v2[2] );
}

/*
=================
NormalizeColor
=================
*/
float NormalizeColor( const vec3_t in, vec3_t out )
{
	float max;

	max = in[0];
	if( in[1] > max )
	{
		max = in[1];
	}
	if( in[2] > max )
	{
		max = in[2];
	}

	if( !max )
	{
		VectorClear( out );
	}
	else
	{
		out[0] = in[0] / max;
		out[1] = in[1] / max;
		out[2] = in[2] / max;
	}
	return max;
}

/*
=================
QuatRatioEigenTrace
=================
*/
float QuatRatioEigenTrace( vec4_t left, vec4_t right )
{
	vec4_t inverse, product;

	QuatInverse( right, inverse );
	QuatMultiply( left, inverse, product) ;

	return QuatEigenTrace( product );
}

/*
=================
MatrixTransposeTransformVector43
=================
*/
void MatrixTransposeTransformVector43( const vec3_t in1, const float in2[4][3], vec3_t out )
{
	vec3_t temp;

	VectorSubtract( in1, in2[3], temp) ;

	out[0] = DotProduct( in2[0], temp );
	out[1] = DotProduct( in2[1], temp );
	out[2] = DotProduct( in2[2], temp );
}

/*
=================
Vec2DistanceSq
=================
*/
float Vec2DistanceSq( const vec2_t v1, const vec2_t v2 )
{
	vec2_t dir;

	Vector2Subtract( v2, v1, dir );
	return Square( dir[0] ) + Square( dir[1] );
}

vec3_t bytedirs[NUMVERTEXNORMALS] =
{
	{-0.525731f, 0.000000f, 0.850651f}, {-0.442863f, 0.238856f, 0.864188f},
	{-0.295242f, 0.000000f, 0.955423f}, {-0.309017f, 0.500000f, 0.809017f},
	{-0.162460f, 0.262866f, 0.951056f}, {0.000000f, 0.000000f, 1.000000f},
	{0.000000f, 0.850651f, 0.525731f}, {-0.147621f, 0.716567f, 0.681718f},
	{0.147621f, 0.716567f, 0.681718f}, {0.000000f, 0.525731f, 0.850651f},
	{0.309017f, 0.500000f, 0.809017f}, {0.525731f, 0.000000f, 0.850651f},
	{0.295242f, 0.000000f, 0.955423f}, {0.442863f, 0.238856f, 0.864188f},
	{0.162460f, 0.262866f, 0.951056f}, {-0.681718f, 0.147621f, 0.716567f},
	{-0.809017f, 0.309017f, 0.500000f},{-0.587785f, 0.425325f, 0.688191f},
	{-0.850651f, 0.525731f, 0.000000f},{-0.864188f, 0.442863f, 0.238856f},
	{-0.716567f, 0.681718f, 0.147621f},{-0.688191f, 0.587785f, 0.425325f},
	{-0.500000f, 0.809017f, 0.309017f}, {-0.238856f, 0.864188f, 0.442863f},
	{-0.425325f, 0.688191f, 0.587785f}, {-0.716567f, 0.681718f, -0.147621f},
	{-0.500000f, 0.809017f, -0.309017f}, {-0.525731f, 0.850651f, 0.000000f},
	{0.000000f, 0.850651f, -0.525731f}, {-0.238856f, 0.864188f, -0.442863f},
	{0.000000f, 0.955423f, -0.295242f}, {-0.262866f, 0.951056f, -0.162460f},
	{0.000000f, 1.000000f, 0.000000f}, {0.000000f, 0.955423f, 0.295242f},
	{-0.262866f, 0.951056f, 0.162460f}, {0.238856f, 0.864188f, 0.442863f},
	{0.262866f, 0.951056f, 0.162460f}, {0.500000f, 0.809017f, 0.309017f},
	{0.238856f, 0.864188f, -0.442863f},{0.262866f, 0.951056f, -0.162460f},
	{0.500000f, 0.809017f, -0.309017f},{0.850651f, 0.525731f, 0.000000f},
	{0.716567f, 0.681718f, 0.147621f}, {0.716567f, 0.681718f, -0.147621f},
	{0.525731f, 0.850651f, 0.000000f}, {0.425325f, 0.688191f, 0.587785f},
	{0.864188f, 0.442863f, 0.238856f}, {0.688191f, 0.587785f, 0.425325f},
	{0.809017f, 0.309017f, 0.500000f}, {0.681718f, 0.147621f, 0.716567f},
	{0.587785f, 0.425325f, 0.688191f}, {0.955423f, 0.295242f, 0.000000f},
	{1.000000f, 0.000000f, 0.000000f}, {0.951056f, 0.162460f, 0.262866f},
	{0.850651f, -0.525731f, 0.000000f},{0.955423f, -0.295242f, 0.000000f},
	{0.864188f, -0.442863f, 0.238856f}, {0.951056f, -0.162460f, 0.262866f},
	{0.809017f, -0.309017f, 0.500000f}, {0.681718f, -0.147621f, 0.716567f},
	{0.850651f, 0.000000f, 0.525731f}, {0.864188f, 0.442863f, -0.238856f},
	{0.809017f, 0.309017f, -0.500000f}, {0.951056f, 0.162460f, -0.262866f},
	{0.525731f, 0.000000f, -0.850651f}, {0.681718f, 0.147621f, -0.716567f},
	{0.681718f, -0.147621f, -0.716567f},{0.850651f, 0.000000f, -0.525731f},
	{0.809017f, -0.309017f, -0.500000f}, {0.864188f, -0.442863f, -0.238856f},
	{0.951056f, -0.162460f, -0.262866f}, {0.147621f, 0.716567f, -0.681718f},
	{0.309017f, 0.500000f, -0.809017f}, {0.425325f, 0.688191f, -0.587785f},
	{0.442863f, 0.238856f, -0.864188f}, {0.587785f, 0.425325f, -0.688191f},
	{0.688191f, 0.587785f, -0.425325f}, {-0.147621f, 0.716567f, -0.681718f},
	{-0.309017f, 0.500000f, -0.809017f}, {0.000000f, 0.525731f, -0.850651f},
	{-0.525731f, 0.000000f, -0.850651f}, {-0.442863f, 0.238856f, -0.864188f},
	{-0.295242f, 0.000000f, -0.955423f}, {-0.162460f, 0.262866f, -0.951056f},
	{0.000000f, 0.000000f, -1.000000f}, {0.295242f, 0.000000f, -0.955423f},
	{0.162460f, 0.262866f, -0.951056f}, {-0.442863f, -0.238856f, -0.864188f},
	{-0.309017f, -0.500000f, -0.809017f}, {-0.162460f, -0.262866f, -0.951056f},
	{0.000000f, -0.850651f, -0.525731f}, {-0.147621f, -0.716567f, -0.681718f},
	{0.147621f, -0.716567f, -0.681718f}, {0.000000f, -0.525731f, -0.850651f},
	{0.309017f, -0.500000f, -0.809017f}, {0.442863f, -0.238856f, -0.864188f},
	{0.162460f, -0.262866f, -0.951056f}, {0.238856f, -0.864188f, -0.442863f},
	{0.500000f, -0.809017f, -0.309017f}, {0.425325f, -0.688191f, -0.587785f},
	{0.716567f, -0.681718f, -0.147621f}, {0.688191f, -0.587785f, -0.425325f},
	{0.587785f, -0.425325f, -0.688191f}, {0.000000f, -0.955423f, -0.295242f},
	{0.000000f, -1.000000f, 0.000000f}, {0.262866f, -0.951056f, -0.162460f},
	{0.000000f, -0.850651f, 0.525731f}, {0.000000f, -0.955423f, 0.295242f},
	{0.238856f, -0.864188f, 0.442863f}, {0.262866f, -0.951056f, 0.162460f},
	{0.500000f, -0.809017f, 0.309017f}, {0.716567f, -0.681718f, 0.147621f},
	{0.525731f, -0.850651f, 0.000000f}, {-0.238856f, -0.864188f, -0.442863f},
	{-0.500000f, -0.809017f, -0.309017f}, {-0.262866f, -0.951056f, -0.162460f},
	{-0.850651f, -0.525731f, 0.000000f}, {-0.716567f, -0.681718f, -0.147621f},
	{-0.716567f, -0.681718f, 0.147621f}, {-0.525731f, -0.850651f, 0.000000f},
	{-0.500000f, -0.809017f, 0.309017f}, {-0.238856f, -0.864188f, 0.442863f},
	{-0.262866f, -0.951056f, 0.162460f}, {-0.864188f, -0.442863f, 0.238856f},
	{-0.809017f, -0.309017f, 0.500000f}, {-0.688191f, -0.587785f, 0.425325f},
	{-0.681718f, -0.147621f, 0.716567f}, {-0.442863f, -0.238856f, 0.864188f},
	{-0.587785f, -0.425325f, 0.688191f}, {-0.309017f, -0.500000f, 0.809017f},
	{-0.147621f, -0.716567f, 0.681718f}, {-0.425325f, -0.688191f, 0.587785f},
	{-0.162460f, -0.262866f, 0.951056f}, {0.442863f, -0.238856f, 0.864188f},
	{0.162460f, -0.262866f, 0.951056f}, {0.309017f, -0.500000f, 0.809017f},
	{0.147621f, -0.716567f, 0.681718f}, {0.000000f, -0.525731f, 0.850651f},
	{0.425325f, -0.688191f, 0.587785f}, {0.587785f, -0.425325f, 0.688191f},
	{0.688191f, -0.587785f, 0.425325f}, {-0.955423f, 0.295242f, 0.000000f},
	{-0.951056f, 0.162460f, 0.262866f}, {-1.000000f, 0.000000f, 0.000000f},
	{-0.850651f, 0.000000f, 0.525731f}, {-0.955423f, -0.295242f, 0.000000f},
	{-0.951056f, -0.162460f, 0.262866f}, {-0.864188f, 0.442863f, -0.238856f},
	{-0.951056f, 0.162460f, -0.262866f}, {-0.809017f, 0.309017f, -0.500000f},
	{-0.864188f, -0.442863f, -0.238856f}, {-0.951056f, -0.162460f, -0.262866f},
	{-0.809017f, -0.309017f, -0.500000f}, {-0.681718f, 0.147621f, -0.716567f},
	{-0.681718f, -0.147621f, -0.716567f}, {-0.850651f, 0.000000f, -0.525731f},
	{-0.688191f, 0.587785f, -0.425325f}, {-0.587785f, 0.425325f, -0.688191f},
	{-0.425325f, 0.688191f, -0.587785f}, {-0.425325f, -0.688191f, -0.587785f},
	{-0.587785f, -0.425325f, -0.688191f}, {-0.688191f, -0.587785f, -0.425325f}
};

//==============================================================


// this isn't a real cheap function to call!
/*
=================
DirToByte
=================
*/
byte DirToByte( vec3_t dir )
{
	byte	i, best;
	float	d, bestd;

	if ( !dir )
	{
		return 0;
	}

	bestd = 0;
	best = 0;
	for (i=0 ; i<NUMVERTEXNORMALS ; i++)
	{
		d = DotProduct(dir, bytedirs[i]);
		if (d > bestd)
		{
			bestd = d;
			best = i;
		}
	}

	return best;
}

/*
=================
ByteToDir
=================
*/
void ByteToDir( int b, vec3_t dir )
{
	if ( b < 0 || b >= NUMVERTEXNORMALS )
	{
		VectorCopy( vec3_origin, dir );
		return;
	}
	VectorCopy (bytedirs[b], dir);
}

/*
=================
UniformPointsOnSphere
=================
*/
void UniformPointsOnSphere( int pointCount, void *points, unsigned int stride )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
UniformPointsOnHemisphere
=================
*/
void UniformPointsOnHemisphere( int pointCount, void *points, unsigned int stride )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
UniformPointsInCircle
=================
*/
void UniformPointsInCircle( int pointCount, void *points, unsigned int stride )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
crandom
=================
*/
float crandom()
{
	return randomf() * 2.0f - 1.0f;
}

/*
=================
VectorRotateAngles
=================
*/
void VectorRotateAngles( vec3_t vIn, vec3_t vRotation, vec3_t out )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
VectorRotateAnglesAroundPoint
=================
*/
void VectorRotateAnglesAroundPoint( vec3_t vIn, vec3_t vRotation, vec3_t vOrigin, vec3_t out )
{
	vec3_t rotated, d;

	VectorSubtract( vIn, vOrigin, d );
	VectorRotateAngles( d, vRotation, rotated );
	VectorAdd( rotated, vOrigin, out );
}

/*
=================
Vec3Mul
=================
*/
void Vec3Mul( const vec3_t a, const vec3_t b, vec3_t product )
{
	product[0] = a[0] * b[0];
	product[1] = a[1] * b[1];
	product[2] = a[2] * b[2];
}

/*
=================
Vec3ProjectionCoords
=================
*/
void Vec3ProjectionCoords( const vec3_t dir, int *xCoord, int *yCoord )
{
	vec3_t dirSq;

	Vec3Mul( dir, dir, dirSq );

	if ( dirSq[0] <= dirSq[2] && dirSq[1] <= dirSq[2] )
	{
		if ( dir[2] > 0.0f )
		{
			*xCoord = 0;
			*yCoord = 1;
		}
		else
		{
			*xCoord = 1;
			*yCoord = 0;
		}
	}
	else if ( dirSq[0] <= dirSq[1] && dirSq[2] <= dirSq[1] )
	{
		if ( dir[1] > 0.0f )
		{
			*xCoord = 2;
			*yCoord = 0;
		}
		else
		{
			*xCoord = 0;
			*yCoord = 2;
		}
	}
	else if ( dir[0] > 0.0f )
	{
		*xCoord = 1;
		*yCoord = 2;
	}
	else
	{
		*xCoord = 2;
		*yCoord = 1;
	}
}

/*
=================
WindingContainsCoplanarPoint
=================
*/
int WindingContainsCoplanarPoint( float (*verts)[3], int vertCount, const vec4_t normal, vec4_t point )
{
	float x[2], y[2];
	int vertIndex, vertIndexPrev, xCoord, yCoord;

	Vec3ProjectionCoords( normal, &xCoord, &yCoord );
	vertIndexPrev = vertCount - 1;

	for ( vertIndex = 0; vertIndex < vertCount; vertIndex++ )
	{
		y[0] = verts[vertIndex][yCoord] - verts[vertIndexPrev][yCoord];
		y[1] = verts[vertIndexPrev][xCoord] - verts[vertIndex][xCoord];

		x[0] = point[xCoord] - verts[vertIndexPrev][xCoord];
		x[1] = point[yCoord] - verts[vertIndexPrev][yCoord];

		if ( Dot2Product( x, y ) < 0.0f )
		{
			return 0;
		}

		vertIndexPrev = vertIndex;
	}

	return 1;
}

/*
=================
ExpandBoundsToWidth
=================
*/
void ExpandBoundsToWidth( vec3_t mins, vec3_t maxs )
{
	float diff, s;
	vec3_t size;

	assert(maxs[0] >= mins[0]);
	assert(maxs[0] >= mins[0]);
	assert(maxs[2] >= mins[2]);

	VectorSubtract( maxs, mins, size );
	s = I_fmax( size[0], size[1] );

	if ( s > size[2] )
	{
		diff = ( s - size[2] ) * 0.5f;

		mins[2] = mins[2] - diff;
		maxs[2] = maxs[2] + diff;
	}
}

/*
=================
AngleDelta
=================
*/
float AngleDelta( float angle1, float angle2 )
{
	return AngleNormalize180( angle1 - angle2 );
}

/*
=================
RotationToYaw
=================
*/
float RotationToYaw( const vec2_t rot )
{
	float r, zz;

	zz = Square( rot[0] );

	r = Square( rot[1] ) + zz;
	assert(r);

	return atan2( rot[0] * rot[1] * ( 2.0f / r ), 1.0f - zz * ( 2.0f / r ) ) * RAD_TO_DEG;
}

/*
=================
AngleEigenTrace
=================
*/
float AngleEigenTrace( float angle )
{
	return Square( sin( angle * DEG_TO_RAD ) );
}

/*
   =================
   GetPerpendicularViewVector
   =================
 * @brief Used to find an "up" vector for drawing a sprite so that it always faces the view as best as possible
 * @param[in] point
 * @param[in] p1
 * @param[in] p2
 * @param[out] up
 */
void GetPerpendicularViewVector( const vec3_t point, const vec3_t p1, const vec3_t p2, vec3_t up )
{
	vec3_t v1, v2;

	VectorSubtract(point, p1, v1);
	Vec3Normalize(v1);

	VectorSubtract(point, p2, v2);
	Vec3Normalize(v2);

	Vec3Cross(v1, v2, up);
	Vec3Normalize(up);
}

/*
=================
vectosignedangles
=================
*/
void vectosignedangles( const vec3_t vec, vec3_t angles )
{
	float pitch, yaw;

	if ( vec[1] == 0 && *vec == 0 )
	{
		yaw = 0;
		if ( vec[2] > 0 )
			pitch = -90;
		else
			pitch = 90;
	}
	else
	{
		yaw = atan2( vec[1], vec[0] ) * 180 / M_PI;
		pitch = atan2( vec[2], I_sqrt( Square( vec[0] ) + Square( vec[1] ) ) ) * -180 / M_PI;
	}

	angles[PITCH] = pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}


/*
=================
vectoangles
=================
*/
void vectoangles( const vec3_t value1, vec3_t angles )
{
	float forward;
	float yaw, pitch;

	if ( value1[1] == 0 && value1[0] == 0 )
	{
		yaw = 0;
		if ( value1[2] > 0 )
		{
			pitch = 270;
		}
		else
		{
			pitch = 90;
		}
	}
	else
	{
		yaw = ( atan2( value1[1], value1[0] ) * 180 / M_PI );
		if(yaw < 0.0)
		{
			yaw += 360.0;
		}
		forward = I_sqrt( value1[0] * value1[0] + value1[1] * value1[1] );
		pitch = ( atan2( value1[2], forward ) * -180 / M_PI );
		if(pitch < 0.0)
		{
			pitch += 360.0;
		}

	}

	angles[PITCH] = pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

/*
=================
vectosignedpitch
=================
*/
float vectosignedpitch( const vec3_t vec )
{
	if ( vec[1] == 0 && *vec == 0 )
	{
		if ( vec[2] > 0 )
			return -90;
		else
			return 90;
	}

	return atan2( vec[2], I_sqrt( Square( vec[0] ) + Square( vec[1] ) ) ) * -180 / M_PI;
}

/*
=================
vectopitch
=================
*/
float vectopitch( const vec3_t vec )
{
	float forward;

	if ( vec[1] == 0 && *vec == 0 )
	{
		if ( vec[2] > 0 )
			return 270;
		else
			return 90;
	}

	forward = atan2( vec[2], I_sqrt( Square( vec[0] ) + Square( vec[1] ) ) ) * -180 / M_PI;

	if ( forward < 0 )
		forward += 360;

	return forward;
}

/*
=================
vectosignedyaw
=================
*/
float vectosignedyaw( const vec3_t vec )
{
	if ( vec[1] == 0 && *vec == 0 )
	{
		return 0;
	}

	float yaw = atan2( vec[1], vec[0] ) * 180 / M_PI;
	assert(yaw >= -180);
	assert(yaw <= 180);

	return yaw;
}

/*
=================
vectoyaw
=================
*/
float vectoyaw( const vec3_t vec )
{
	float yaw;

	if ( vec[1] == 0 && vec[0] == 0 )
	{
		return 0;
	}

	yaw = atan2( vec[1], vec[0] ) * 180 / M_PI;

	if ( yaw < 0)
		yaw += 360;

	return yaw;
}

/*
=================
Vec2Distance
=================
*/
float Vec2Distance( vec2_t v0, vec2_t v1 )
{
	vec2_t dir;

	Vector2Subtract( v1, v0, dir );
	return Vec2Length( dir );
}

/*
=====================
Q_acos

the msvc acos doesn't always return a value between -PI and PI:

int i;
i = 1065353246;
acos(*(float*) &i) == -1.#IND0

This should go in q_math but it is too late to add new traps
to game and ui
=====================
*/
float Q_acos(float c)
{
	float angle;

	angle = acos(c);

	if (angle > M_PI)
	{
		return (float)M_PI;
	}
	if (angle < -M_PI)
	{
		return (float)M_PI;
	}
	return angle;
}

/*
=================
GaussianRandom
=================
*/
void GaussianRandom( float *f0, float *f1 ) // untested
{
	float w, y, x;

	assert(f0);
	assert(f1);

	do
	{
		x = crandom();
		y = crandom();

		w = Square( x ) + Square (y);
	}
	while ( w > 1.0f );

	*f0 = x * I_sqrt( log( w ) * -2.0f / w );
	*f1 = y * I_sqrt( log( w ) * -2.0f / w );
}

/*
=================
VectorPolar
=================
*/
void VectorPolar( vec3_t v, float radius, float theta, float phi )
{
	v[0]=(float)(radius * cos(theta) * cos(phi));
	v[1]=(float)(radius * sin(theta) * cos(phi));
	v[2]=(float)(radius * sin(phi));
}

/*
=================
IsPosInsideArc
=================
*/
int IsPosInsideArc( const vec3_t pos, float posRadius, const vec3_t arcOrigin, float arcRadius, float arcAngle0, float arcAngle1, float arcHalfHeight )
{
	float posYaw, distDelta, adjustedDistDelta;
	vec3_t originDelta;

	assert(pos);
	assert(arcOrigin);

	VectorSubtract(pos, arcOrigin, originDelta);

	distDelta = Vec2Normalize(originDelta);
	adjustedDistDelta = distDelta - posRadius;

	if ( adjustedDistDelta * adjustedDistDelta > arcRadius * arcRadius )
	{
		return 0;
	}

	if ( arcOrigin[2] - arcHalfHeight <= pos[2] && pos[2] <= arcOrigin[2] + arcHalfHeight )
	{
		posYaw = AngleNormalize360(vectoyaw(originDelta));

		if ( arcAngle1 <= arcAngle0 )
		{
			if ( arcAngle1 > posYaw || posYaw > arcAngle0 )
			{
				return 1;
			}
		}
		else if ( arcAngle1 > posYaw && posYaw > arcAngle0 )
		{
			return 1;
		}

		return 0;
	}

	return 0;
}

/*
=================
RadiusFromBounds2D
=================
*/
float RadiusFromBounds2D( const vec2_t mins, const vec2_t maxs )
{
	int i;
	vec2_t corner;
	float a, b;

	for ( i = 0 ; i < 2 ; i++ )
	{
		a = I_fabs( mins[i] );
		b = I_fabs( maxs[i] );
		corner[i] = a > b ? a : b;
	}

	return Vec2Length( corner );
}

/*
=================
RadiusFromBounds
=================
*/
float RadiusFromBounds( const vec3_t mins, const vec3_t maxs )
{
	int i;
	vec3_t corner;
	float a, b;

	for ( i = 0 ; i < 3 ; i++ )
	{
		a = I_fabs( mins[i] );
		b = I_fabs( maxs[i] );
		corner[i] = a > b ? a : b;
	}

	return VectorLength( corner );
}

/*
=================
MatrixRotationZ
=================
*/
void MatrixRotationZ( float mat[3][3], float degree )
{
	float s, c;

	FastSinCos( degree * DEG_TO_RAD, &s, &c );

	mat[0][0] = c;
	mat[0][1] = -s;
	mat[0][2] = 0.0f;
	mat[1][0] = s;
	mat[1][1] = c;
	mat[1][2] = 0.0f;
	mat[2][0] = 0.0f;
	mat[2][1] = 0.0f;
	mat[2][2] = 1.0f;
}

/*
=================
MatrixRotationY
=================
*/
void MatrixRotationY( float mat[3][3], float degree )
{
	float s, c;

	FastSinCos( degree * DEG_TO_RAD, &s, &c );

	mat[0][0] = c;
	mat[0][1] = 0.0f;
	mat[0][2] = s;
	mat[1][0] = 0.0f;
	mat[1][1] = 1.0f;
	mat[1][2] = 0.0f;
	mat[2][0] = -s;
	mat[2][1] = 0.0f;
	mat[2][2] = c;
}

/*
=================
MatrixRotationX
=================
*/
void MatrixRotationX( float mat[3][3], float degree )
{
	float s, c;

	FastSinCos( degree * DEG_TO_RAD, &s, &c );

	mat[0][0] = 1.0f;
	mat[0][1] = 0.0f;
	mat[0][2] = 0.0f;
	mat[1][0] = 0.0f;
	mat[1][1] = c;
	mat[1][2] = -s;
	mat[2][0] = 0.0f;
	mat[2][1] = s;
	mat[2][2] = c;
}

/*
=================
AngleAxisToQuat
=================
*/
void AngleAxisToQuat( float angle, const vec3_t axis, vec4_t quat )
{
	float s;

	FastSinCos( angle * HALF_DEG_TO_RAD, &s, &quat[3] );
	VectorScale( axis, s, quat );
}

/*
=================
RollToQuat
=================
*/
void RollToQuat( float roll, vec4_t quat )
{
	quat[1] = 0.0f;
	quat[2] = 0.0f;

	FastSinCos( roll * HALF_DEG_TO_RAD, &quat[0], &quat[3] );
}

/*
=================
YawToQuat
=================
*/
void YawToQuat( float yaw, vec4_t quat )
{
	quat[0] = 0.0f;
	quat[1] = 0.0f;

	FastSinCos( yaw * HALF_DEG_TO_RAD, &quat[2], &quat[3] );
}

/*
=================
PitchToQuat
=================
*/
void PitchToQuat( float pitch, vec4_t quat )
{
	quat[0] = 0.0f;
	quat[2] = 0.0f;

	FastSinCos( pitch * HALF_DEG_TO_RAD, &quat[1], &quat[3] );
}

/*
=================
VectorAngleMultiply
=================
*/
void VectorAngleMultiply( vec3_t vec, float angle )
{
	float x, y, temp;

	FastSinCos( angle * DEG_TO_RAD, &y, &x );

	temp = vec[0] * x - vec[1] * y;

	vec[1] = vec[1] * x + vec[0] * y;
	vec[0] = temp;
}

/*
=================
YawVectors
=================
*/
void YawVectors( float yaw, vec3_t forward, vec3_t right )
{
	float sy, cy;

	FastSinCos( yaw * DEG_TO_RAD, &sy, &cy );

	if ( forward )
	{
		forward[0] = cy;
		forward[1] = sy;
		forward[2] = 0.0f;
	}

	if ( right )
	{
		right[0] = sy;
		right[1] = -cy;
		right[2] = 0.0f;
	}
}

/*
** assumes "src" is normalized
*/
void PerpendicularVector( const vec3_t src, vec3_t dst )
{
	int	pos;
	vec3_t srcSq;

	assert(Vec3IsNormalized( src ));

	/*
	** find the smallest magnitude axially aligned vector
	*/
	srcSq[0] = src[0] * src[0];
	srcSq[1] = src[1] * src[1];
	srcSq[2] = src[2] * src[2];

	pos = srcSq[0] > srcSq[1];
	if ( srcSq[pos] > srcSq[2] )
		pos = 2;

	/*
	** project the point onto the plane defined by src
	*/
	VectorScale( src, -src[pos], dst );
	dst[pos] += 1.0f;

	/*
	** normalize the result
	*/
	Vec3Normalize( dst );
}

/*
===============
RotatePointAroundVector

This is not implemented very well...
===============
*/
void RotatePointAroundVector( vec3_t dst, vec3_t dir, const vec3_t point, float degrees ) // untested
{
	float	m[3][3];
	float	im[3][3];
	float	zrot[3][3];
	float	tmpmat[3][3];
	float	rot[3][3];
	int	i;
	vec3_t vr, vup, vf;
	float	rad;

	assert(dir[0] || dir[1] || dir[2]);

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector( dir, vr );
	Vec3Cross( vr, vf, vup );

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy( im, m, sizeof( im ) );

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset( zrot, 0, sizeof( zrot ) );
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0f;

	rad = DEG_TO_RAD;
	assert(!IS_NAN(rad));

	FastSinCos( rad, &zrot[0][1], &zrot[0][0] );
	assert(!IS_NAN(zrot[0][1]));
	assert(!IS_NAN(zrot[0][0]));

	zrot[1][0] = -zrot[0][1];
	zrot[1][1] = zrot[0][0];

	MatrixMultiply( m, zrot, tmpmat );
	MatrixMultiply( tmpmat, im, rot );

	for ( i = 0; i < 3; i++ )
	{
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}
}

/*
==================
Vec3UnpackNormalized
==================
*/
unsigned int Vec3PackNormalized( const vec3_t vec )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==================
DiffTrack
==================
*/
float DiffTrack( float tgt, float cur, float rate, float deltaTime )
{
	float step, err;

	err = tgt - cur;
	step = rate * err * deltaTime;

	if ( I_fabs( err ) <= VEC_EPSILON )
	{
		return tgt;
	}

	if ( I_fabs( step ) > I_fabs( err ) )
	{
		return tgt;
	}

	return cur + step;
}

/*
==================
LinearTrack
==================
*/
float LinearTrack( float tgt, float cur, float rate, float deltaTime )
{
	float step, err;

	err = tgt - cur;

	if ( err > 0.0f )
		step = rate * deltaTime;
	else
		step = -rate * deltaTime;

	if ( I_fabs(err) <= VEC_EPSILON )
	{
		return tgt;
	}

	if ( I_fabs( step ) > I_fabs( err ) )
	{
		return tgt;
	}

	return cur + step;
}

/*
==================
PointOnHemisphereFromUniformDeviates
==================
*/
void PointOnHemisphereFromUniformDeviates( float azimuthDeviate, float yawDeviate, vec3_t unitVec )
{
	UNIMPLEMENTED(__FUNCTION__);
}


/*
==================
PointOnSphereFromUniformDeviates
==================
*/
void PointOnSphereFromUniformDeviates( float azimuthDeviate, float yawDeviate, vec3_t unitVec )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
PointInCircleFromUniformDeviates
==================
*/
void PointInCircleFromUniformDeviates( float radiusDeviate, float yawDeviate, vec3_t point )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
PitchForYawOnNormal
==================
*/
float PitchForYawOnNormal( const float fYaw, const vec3_t normal )
{
	vec3_t forward, dst;

	assert(normal[0] || normal[1] || normal[2]);

	YawVectors( fYaw, forward, 0 );
	ProjectPointOnPlane( forward, normal, dst );

	return vectopitch( dst );
}

/*
===============
RotateAroundDirection
===============
*/
void RotateAroundDirection( vec3_t axis[3], float yaw )
{
	// create an arbitrary axis[1]
	PerpendicularVector( axis[1], axis[0] );

	// rotate it around axis[0] by yaw
	if ( yaw )
	{
		vec3_t	temp;

		VectorCopy( axis[1], temp );
		RotatePointAroundVector( axis[1], axis[0], temp, yaw );
	}

	// cross to get axis[2]
	Vec3Cross( axis[0], axis[1], axis[2] );
}

/*
===============
DiffTrackAngle
===============
*/
float DiffTrackAngle( float tgt, float cur, float rate, float deltaTime )
{
	while ( tgt - cur > 180 )
	{
		tgt -= 360;
	}

	while ( tgt - cur < -180 )
	{
		tgt += 360;
	}

	return AngleNormalize180( DiffTrack( tgt, cur, rate, deltaTime ) );
}

/*
===============
LinearTrackAngle
===============
*/
float LinearTrackAngle( float tgt, float cur, float rate, float deltaTime )
{
	while ( tgt - cur > 180 )
	{
		tgt -= 360;
	}

	while ( tgt - cur < -180 )
	{
		tgt += 360;
	}

	return AngleNormalize180( LinearTrack( tgt, cur, rate, deltaTime ) );
}

/*
===============
MatrixTransformVector
===============
*/
void MatrixTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out )
{
	assert(in1 != out);

	out[ 0 ] = ( in1[ 0 ] * in2[ 0 ][ 0 ] ) + ( in1[ 1 ] * in2[ 1 ][ 0 ] ) + ( in1[ 2 ] * in2[ 2 ][ 0 ] );
	out[ 1 ] = ( in1[ 0 ] * in2[ 0 ][ 1 ] ) + ( in1[ 1 ] * in2[ 1 ][ 1 ] ) + ( in1[ 2 ] * in2[ 2 ][ 1 ] );
	out[ 2 ] = ( in1[ 0 ] * in2[ 0 ][ 2 ] ) + ( in1[ 1 ] * in2[ 1 ][ 2 ] ) + ( in1[ 2 ] * in2[ 2 ][ 2 ] );
}

/*
===============
MatrixInverse
===============
*/
void MatrixInverse( const float in1[3][3], float out[3][3] )
{
	float determinant;
	float revdet;

	assert(in1 != out);

	determinant = (in1[2][2] * in1[1][1] - in1[2][1] * in1[1][2]) * in1[0][0]
	              - (in1[2][2] * in1[0][1] - in1[2][1] * in1[0][2]) * in1[1][0]
	              + (in1[1][2] * in1[0][1] - in1[1][1] * in1[0][2]) * in1[2][0];

	assert(determinant);
	revdet = 1.0f / determinant;

	out[0][0] =  (in1[2][2] * in1[1][1] - in1[2][1] * in1[1][2]) * revdet;
	out[0][1] = -(in1[2][2] * in1[0][1] - in1[2][1] * in1[0][2]) * revdet;
	out[0][2] =  (in1[1][2] * in1[0][1] - in1[1][1] * in1[0][2]) * revdet;
	out[1][0] = -(in1[2][2] * in1[1][0] - in1[2][0] * in1[1][2]) * revdet;
	out[1][1] =  (in1[2][2] * in1[0][0] - in1[2][0] * in1[0][2]) * revdet;
	out[1][2] = -(in1[1][2] * in1[0][0] - in1[1][0] * in1[0][2]) * revdet;
	out[2][0] =  (in1[2][1] * in1[1][0] - in1[2][0] * in1[1][1]) * revdet;
	out[2][1] = -(in1[2][1] * in1[0][0] - in1[2][0] * in1[0][1]) * revdet;
	out[2][2] =  (in1[1][1] * in1[0][0] - in1[1][0] * in1[0][1]) * revdet;
}

/*
===============
MatrixMultiply
===============
*/
void MatrixMultiply( const float in1[3][3], const float in2[3][3], float out[3][3] )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
}

/*
===============
MatrixTranspose
===============
*/
void MatrixTranspose( const float in[3][3], float out[3][3] )
{
	assert(in != out);

	out[0][0] = in[0][0];
	out[0][1] = in[1][0];
	out[0][2] = in[2][0];
	out[1][0] = in[0][1];
	out[1][1] = in[1][1];
	out[1][2] = in[2][1];
	out[2][0] = in[0][2];
	out[2][1] = in[1][2];
	out[2][2] = in[2][2];
}

/*
===============
MatrixMultiply43
===============
*/
void MatrixMultiply43( const float in1[4][3], const float in2[4][3], float out[4][3] )
{
	assert(in1 != out);
	assert(in2 != out);

	out[0][0] = ((in1[0][0] * in2[0][0]) + (in1[0][1] * in2[1][0])) + (in1[0][2] * in2[2][0]);
	out[1][0] = ((in1[1][0] * in2[0][0]) + (in1[1][1] * in2[1][0])) + (in1[1][2] * in2[2][0]);
	out[2][0] = ((in1[2][0] * in2[0][0]) + (in1[2][1] * in2[1][0])) + (in1[2][2] * in2[2][0]);
	out[0][1] = ((in1[0][0] * in2[0][1]) + (in1[0][1] * in2[1][1])) + (in1[0][2] * in2[2][1]);
	out[1][1] = ((in1[1][0] * in2[0][1]) + (in1[1][1] * in2[1][1])) + (in1[1][2] * in2[2][1]);
	out[2][1] = ((in1[2][0] * in2[0][1]) + (in1[2][1] * in2[1][1])) + (in1[2][2] * in2[2][1]);
	out[0][2] = ((in1[0][0] * in2[0][2]) + (in1[0][1] * in2[1][2])) + (in1[0][2] * in2[2][2]);
	out[1][2] = ((in1[1][0] * in2[0][2]) + (in1[1][1] * in2[1][2])) + (in1[1][2] * in2[2][2]);
	out[2][2] = ((in1[2][0] * in2[0][2]) + (in1[2][1] * in2[1][2])) + (in1[2][2] * in2[2][2]);
	out[3][0] = (((in1[3][0] * in2[0][0]) + (in1[3][1] * in2[1][0])) + (in1[3][2] * in2[2][0])) + in2[3][0];
	out[3][1] = (((in1[3][0] * in2[0][1]) + (in1[3][1] * in2[1][1])) + (in1[3][2] * in2[2][1])) + in2[3][1];
	out[3][2] = (((in1[3][0] * in2[0][2]) + (in1[3][1] * in2[1][2])) + (in1[3][2] * in2[2][2])) + in2[3][2];
}

/*
===============
MatrixInverseOrthogonal43
===============
*/
void MatrixInverseOrthogonal43( const float in[4][3], float out[4][3] )
{
	vec3_t origin;

	MatrixTranspose(in, out);
	VectorSubtract(vec3_origin, in[3], origin);
	MatrixTransformVector(origin, out, out[3]);
}

/*
===============
AngleVectors
===============
*/
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up )
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * ( M_PI * 2 / 360 );
	FastSinCos( angle, &sy, &cy );

	angle = angles[PITCH] * ( M_PI * 2 / 360 );
	FastSinCos( angle, &sp, &cp );

	if ( forward )
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}

	if ( !right && !up )
	{
		return;
	}

	angle = angles[ROLL] * ( M_PI * 2 / 360 );
	FastSinCos( angle, &sr, &cr );

	if ( right )
	{
		right[0] = ( -1 * sr * sp * cy + - 1 * cr * -sy );
		right[1] = ( -1 * sr * sp * sy + - 1 * cr * cy );
		right[2] = -1 * sr * cp;
	}

	if ( up )
	{
		up[0] = ( cr * sp * cy + - sr * -sy );
		up[1] = ( cr * sp * sy + - sr * cy );
		up[2] = cr * cp;
	}
}

/*
===============
AnglesToAxis
===============
*/
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] )
{
	vec3_t right;

	AngleVectors( angles, axis[0], right, axis[2] );
	VectorSubtract( vec3_origin, right, axis[1] );
}

/*
===============
Vec2Normalize
===============
*/
float Vec2Normalize( vec2_t v )
{
	float length = Vec2Length(v);

	if ( length != 0 )
	{
		Vec2Scale( v, 1.0f / length, v );
	}
	return length;
}

/*
=================
Vec2NormalizeTo
=================
*/
float Vec2NormalizeTo( const vec2_t v, vec2_t out )
{
	float length = Vec2Length(v);

	if ( length == 0 )
	{
		Vector2Clear( out );
	}
	else
	{
		Vec2Scale( v, 1.0f / length, out );
	}
	return length;
}

/*
===============
Vec3Normalize
===============
*/
float Vec3Normalize( vec3_t v )
{
	float length = VectorLength(v);

	if ( length != 0 )
	{
		VectorScale( v, 1.0f / length, v );
	}
	return length;
}

/*
===============
Vec3NormalizeTo
===============
*/
float Vec3NormalizeTo( const vec3_t v, vec3_t out )
{
	float length = VectorLength(v);

	if ( length == 0 )
	{
		VectorClear( out );
	}
	else
	{
		VectorScale( v, 1.0f / length, out );
	}
	return length;
}

/*
=================
Vec4Normalize
=================
*/
float Vec4Normalize( vec4_t v )
{
	float length = Vec4Length(v);

	if ( length != 0 )
	{
		VectorScale4( v, 1.0f / length, v );
	}
	return length;
}

/*
=================
Vec3Cross
=================
*/
void Vec3Cross( const vec3_t v0, const vec3_t v1, vec3_t cross )
{
	assert(v0 != cross);
	assert(v1 != cross);

	cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
	cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
	cross[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

/*
=================
SnapAngles
=================
*/
void SnapAngles( vec3_t angles )
{
	float delta;
	int rounded, i;

	for ( i = 0; i < 3; i++ )
	{
		rounded = Q_rint( angles[i] );
		delta = rounded - angles[i];

		if ( Square( delta ) >= FLT_EPSILON )
			continue;

		angles[i] = rounded;
	}
}

/*
=================
AxisToAngles
=================
*/
void AxisToAngles( vec3_t axis[3], vec3_t angles )
{
	float rad;
	vec3_t right;
	float temp;
	float pitch;
	float fCos;
	float fSin;

	// first get the pitch and yaw from the forward vector
	vectoangles( axis[0], angles );

	// now get the roll from the right vector
	VectorCopy( axis[1], right );

	// get the angle difference between the tmpAxis[2] and axis[2] after they have been reverse-rotated
	rad = (-angles[1] * DEG_TO_RAD);
	FastSinCos(rad, &fSin, &fCos);

	temp = fCos * right[0] - fSin * right[1];
	right[1] = fSin * right[0] + fCos * right[1];

	rad = -angles[0] * DEG_TO_RAD;
	FastSinCos(rad, &fSin, &fCos);

	right[0] = (fSin * right[2]) + (fCos * temp);
	right[2] = (fCos * right[2]) - (fSin * temp);

	// now find the angles, the PITCH is effectively our ROLL
	pitch = vectosignedpitch(right);

	// if the yaw is more than 90 degrees difference, we should adjust the pitch
	if ( right[1] < 0 )
	{
		if ( pitch < 0 )
			angles[2] = pitch + 180;
		else
			angles[2] = pitch - 180;
	}
	else
	{
		angles[2] = -pitch;
	}
}

/*
=================
Vec3Distance
=================
*/
float Vec3Distance( const vec3_t v1, const vec3_t v2 )
{
	vec3_t dir;

	VectorSubtract( v2, v1, dir );
	return VectorLength( dir );
}

/*
=================
Vec3DistanceSq
=================
*/
float Vec3DistanceSq( const vec3_t v1, const vec3_t v2 )
{
	vec3_t dir;

	VectorSubtract( v2, v1, dir );
	return VectorLengthSquared( dir );
}

/*
=================
Vec3Lerp
=================
*/
void Vec3Lerp( const vec3_t start, const vec3_t end, float fraction, vec3_t endpos )
{
	endpos[0] = (end[0] - start[0]) * fraction + start[0];
	endpos[1] = (end[1] - start[1]) * fraction + start[1];
	endpos[2] = (end[2] - start[2]) * fraction + start[2];
}

/*
=================
YawToAxis
=================
*/
void YawToAxis( float yaw, vec3_t axis[3] )
{
	vec3_t right;

	YawVectors( yaw, axis[0], right );

	axis[2][0] = 0.0f;
	axis[2][1] = 0.0f;
	axis[2][2] = 1.0f;

	VectorSubtract( vec3_origin, right, axis[1] );
}

/*
=================
RoundFloatArray
=================
*/
void RoundFloatArray( vec3_t x, vec3_t y )
{
	for ( int i = 0; i < 3; i++ )
	{
		if ( x[i] < y[i] )
			x[i] = ceil(x[i]);
		else
			x[i] = floor(x[i]);
	}
}

/*
=================
ClearBounds
=================
*/
void ClearBounds( vec3_t mins, vec3_t maxs )
{
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}

/*
=================
AddPointToBounds
=================
*/
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs )
{
	if ( mins[0] > v[0] )
		mins[0] = v[0];
	if ( v[0] > maxs[0] )
		maxs[0] = v[0];
	if ( mins[1] > v[1] )
		mins[1] = v[1];
	if ( v[1] > maxs[1] )
		maxs[1] = v[1];
	if ( mins[2] > v[2] )
		mins[2] = v[2];
	if ( v[2] > maxs[2] )
		maxs[2] = v[2];
}

/*
=================
AddPointToBounds2D
=================
*/
void AddPointToBounds2D( const vec2_t v, vec2_t mins, vec2_t maxs )
{
	if ( mins[0] > v[0] )
		mins[0] = v[0];
	if ( v[0] > maxs[0] )
		maxs[0] = v[0];
	if ( mins[1] > v[1] )
		mins[1] = v[1];
	if ( v[1] > maxs[1] )
		maxs[1] = v[1];
}

/*
=================
BoxOnPlaneSide
=================
*/
int BoxOnPlaneSide( vec3_t emins, vec3_t emaxs, const cplane_t *p )
{
	float dist1, dist2;
	int sides;

	// fast axial cases
	if ( p->type < 3 )
	{
		if ( p->dist <= emins[p->type] )
		{
			return 1;
		}
		if ( p->dist >= emaxs[p->type] )
		{
			return 2;
		}
		return 3;
	}

	// general case
	switch ( p->signbits )
	{
	case 0:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 1:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 2:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 3:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 4:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 5:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 6:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	case 7:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;      // shut up compiler
		break;
	}

	sides = 0;

	if ( dist1 >= p->dist )
	{
		sides = 1;
	}

	if ( dist2 < p->dist )
	{
		sides |= 2;
	}

	return sides;
}

/*
================
CreateRotationMatrix
================
*/
void CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] )
{
	AngleVectors( angles, matrix[0], matrix[1], matrix[2] );
	VectorInverse( matrix[1] );
}

// TTimo: const vec_t ** would require explicit casts for ANSI C conformance
// see unix/const-arg.c in Wolf MP source
void TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] )
{
	int i, j;

	for ( i = 0; i < 3; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			transpose[i][j] = matrix[j][i];
		}
	}
}

// TTimo: const vec_t ** would require explicit casts for ANSI C conformance
// see unix/const-arg.c in Wolf MP source
void RotatePoint( vec3_t point, const vec3_t matrix[3] )
{
	vec3_t tvec;

	VectorCopy( point, tvec );

	point[0] = DotProduct( matrix[0], tvec );
	point[1] = DotProduct( matrix[1], tvec );
	point[2] = DotProduct( matrix[2], tvec );
}
