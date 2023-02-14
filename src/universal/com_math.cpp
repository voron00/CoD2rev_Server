#include "com_math.h"

vec2_t vec2_origin = {0, 0};
vec3_t vec3_origin = {0, 0, 0};
vec4_t vec4_origin = {0, 0, 0, 0};

float Q_fabs( float f )
{
	int tmp = ( *(int*)&f ) & 0x7FFFFFFF;
	return *(float*)&tmp;
}

int VectorCompare(const vec3_t v1, const vec3_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
		return 0;

	return 1;
}

int Vector2Compare(const vec2_t v1, const vec2_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1])
		return 0;

	return 1;
}

int Vector4Compare(const vec4_t v1, const vec4_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2] || v1[3] != v2[3])
		return 0;

	return 1;
}

vec_t VectorLength( const vec3_t v )
{
	return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

vec_t Vec2Length( const vec2_t v )
{
	return sqrt( v[0] * v[0] + v[1] * v[1] );
}

vec_t VectorLengthSquared( const vec3_t v )
{
	return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

vec_t Vec4LengthSq( const vec4_t v)
{
	return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3] );
}

void VectorInverse( vec3_t v )
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

void MatrixTransformVector(const float *in1, const float *in2, float *out)
{
	*out = *in1 * *in2 + in1[1] * in2[3] + in1[2] * in2[6];
	out[1] = *in1 * in2[1] + in1[1] * in2[4] + in1[2] * in2[7];
	out[2] = *in1 * in2[2] + in1[1] * in2[5] + in1[2] * in2[8];
}

void MatrixInverse(const float *in, float *out)
{
	float determinant;
	float revdet;

	determinant = (in[8] * in[4] - in[7] * in[5]) * *in
	              - (in[8] * in[1] - in[7] * in[2]) * in[3]
	              + (in[5] * in[1] - in[4] * in[2]) * in[6];

	revdet = 1.0 / determinant;

	*out = (in[8] * in[4] - in[7] * in[5]) * revdet;
	out[1] = -(in[8] * in[1] - in[7] * in[2]) * revdet;
	out[2] = (in[5] * in[1] - in[4] * in[2]) * revdet;
	out[3] = -(in[8] * in[3] - in[6] * in[5]) * revdet;
	out[4] = (in[8] * *in - in[6] * in[2]) * revdet;
	out[5] = -(in[5] * *in - in[3] * in[2]) * revdet;
	out[6] = (in[7] * in[3] - in[6] * in[4]) * revdet;
	out[7] = -(in[7] * *in - in[6] * in[1]) * revdet;
	out[8] = (in[4] * *in - in[3] * in[1]) * revdet;
}

void MatrixMultiply( float in1[3][3], float in2[3][3], float out[3][3] )
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

void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up )
{
	float angle;
	static float sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	angle = angles[YAW] * ( M_PI * 2 / 360 );
	sy = sin( angle );
	cy = cos( angle );

	angle = angles[PITCH] * ( M_PI * 2 / 360 );
	sp = sin( angle );
	cp = cos( angle );

	angle = angles[ROLL] * ( M_PI * 2 / 360 );
	sr = sin( angle );
	cr = cos( angle );

	if ( forward )
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}

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

void AnglesToAxis( const vec3_t angles, vec3_t axis[3] )
{
	vec3_t right;

	AngleVectors( angles, axis[0], right, axis[2] );
	VectorSubtract( vec3_origin, right, axis[1] );
}