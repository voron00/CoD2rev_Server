#include "q_shared.h"

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

vec_t Vec2LengthSq( const vec2_t v )
{
	return ( v[0] * v[0] + v[1] * v[1] );
}

vec_t VectorLengthSquared( const vec3_t v )
{
	return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

vec_t Vec4LengthSq( const vec4_t v)
{
	return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3] );
}

vec_t VectorsLengthSquared(const vec4_t v1, const vec4_t v2)
{
	return ( v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] );
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

void MatrixTransformVector43(const float *in1, const float *in2, float *out)
{
	*out = *in1 * *in2 + in1[1] * in2[3] + in1[2] * in2[6] + in2[9];
	out[1] = *in1 * in2[1] + in1[1] * in2[4] + in1[2] * in2[7] + in2[10];
	out[2] = *in1 * in2[2] + in1[1] * in2[5] + in1[2] * in2[8] + in2[11];
}

void MatrixTransposeTransformVector(const float *in1, const vec3_t in2[3], float *out)
{
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[0][1] + in1[2] * in2[0][2];
	out[1] = in1[0] * in2[1][0] + in1[1] * in2[1][1] + in1[2] * in2[1][2];
	out[2] = in1[0] * in2[2][0] + in1[1] * in2[2][1] + in1[2] * in2[2][2];
}

void MatrixTransposeTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out)
{
	vec3_t temp;

	VectorSubtract(in1, in2[3], temp);

	out[0] = DotProduct(in2[0], temp);
	out[1] = DotProduct(in2[1], temp);
	out[2] = DotProduct(in2[2], temp);
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

vec_t Vec2Normalize( vec3_t v )
{
	float length, ilength;

	length = v[0] * v[0] + v[1] * v[1];
	length = sqrt( length );

	if ( length )
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
	}

	return length;
}


vec_t Vec3Normalize( vec3_t v )
{
	float length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt( length );

	if ( length )
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

vec_t Vec4Normalize( vec4_t v )
{
	float length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
	length = sqrt( length );

	if ( length )
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
		v[3] *= ilength;
	}

	return length;
}

double Vec2NormalizeTo(const float *v, float *out)
{
	float length;

	length = Vec2Length(v);

	if ( length == 0.0 )
	{
		length = 1.0;
	}

	Vec2Scale(v, length, out);
	return length;
}

vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out )
{
	float length, result;

	length = VectorLength(v);
	result = length;

	if ( length <= 0.0 )
	{
		length = 1.0;
	}

	VectorScale(v, 1.0 / length, out);
	return result;
}

void Vec3Cross(const vec3_t v0, const vec3_t v1, vec3_t cross)
{
	cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
	cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
	cross[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

float AngleNormalize360( float angle )
{
	return ( 360.0 / 65536 ) * ( (int)( angle * ( 65536 / 360.0 ) ) & 65535 );
}

float AngleNormalize180( float angle )
{
	angle = AngleNormalize360( angle );

	if ( angle > 180.0 )
	{
		angle -= 360.0;
	}

	return angle;
}

float AngleDelta( float angle1, float angle2 )
{
	return AngleNormalize180( angle1 - angle2 );
}

int BoxOnPlaneSide( vec3_t emins, vec3_t emaxs, struct cplane_s *p )
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

float RadiusFromBounds( const vec3_t mins, const vec3_t maxs )
{
	int i;
	vec3_t corner;
	float a, b;

	for ( i = 0 ; i < 3 ; i++ )
	{
		a = fabs( mins[i] );
		b = fabs( maxs[i] );
		corner[i] = a > b ? a : b;
	}

	return VectorLength( corner );
}

float RadiusFromBounds2D( const vec3_t mins, const vec3_t maxs )
{
	int i;
	vec2_t corner;
	float a, b;

	for ( i = 0 ; i < 2 ; i++ )
	{
		a = fabs( mins[i] );
		b = fabs( maxs[i] );
		corner[i] = a > b ? a : b;
	}

	return Vec2Length( corner );
}

vec_t Q_rint( vec_t in )
{
	return floor( in + 0.5 );
}

void SnapAngles(vec3_t angles)
{
	float delta;
	int r;
	int i;

	for ( i = 0; i < 3; ++i )
	{
		r = Q_rint(angles[i]);
		delta = (long double)r - angles[i];

		if ( Square(delta) < 0.0000010000001 )
			angles[i] = (float)r;
	}
}

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
		forward = sqrt( value1[0] * value1[0] + value1[1] * value1[1] );
		pitch = ( -atan2( value1[2], forward ) * 180 / M_PI );
		if(pitch < 0.0)
		{
			pitch += 360.0;
		}

	}

	angles[PITCH] = pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

vec_t vectosignedpitch(const vec3_t vec)
{
	float t;

	if ( 0.0 != vec[1] || 0.0 != vec[0] )
	{
		t = atan2(vec[2], sqrt(vec[1] * vec[1] + vec[0] * vec[0]));
		return t * -180.0 / M_PI;
	}

	if ( -vec[2] < 0.0 )
	{
		return -90.0;
	}

	return 90.0;
}

void AxisToAngles( vec3_t axis[3], vec3_t angles )
{
	long double a;
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
	a = (-angles[YAW] * 0.017453292);
	fCos = cos(a);
	fSin = sin(a);

	temp = fCos * right[0] - fSin * right[1];
	right[1] = fSin * right[0] + fCos * right[1];

	a = -angles[0] * 0.017453292;
	fCos = cos(a);
	fSin = sin(a);

	right[0] = (fSin * right[2]) + (fCos * temp);
	right[2] = (fCos * right[2]) - (fSin * temp);

	// now find the angles, the PITCH is effectively our ROLL
	pitch = vectosignedpitch(right);

	if ( right[1] >= 0.0 )
	{
		angles[ROLL] = -pitch;
		return;
	}

	if ( pitch >= 0.0 )
	{
		angles[ROLL] = pitch - 180.0;
		return;
	}

	angles[ROLL] = pitch + 180.0;
}

float AngleMod( float a )
{
	return( ( 360.0 / 65536 ) * ( (int)( a * ( 65536 / 360.0 ) ) & 65535 ) );
}

float AngleSubtract( float a1, float a2 )
{
	float a = a1 - a2;

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

double vectosignedyaw(float *vec)
{
	float at;
	double yaw;

	if ( 0.0 != vec[1] || 0.0 != vec[0] )
	{
		at = atan2(vec[1], vec[0]);
		yaw = at * 180.0 / M_PI;
		return yaw;
	}

	return 0.0;
}

int BoxDistSqrdExceeds(const float *absmin, const float *absmax, const float *org, const float fogOpaqueDistSqrd)
{
	float f;
	vec3_t ma;
	vec3_t mi;
	int i;

	mi[0] = absmin[0] - org[0];
	mi[1] = absmin[1] - org[1];
	mi[2] = absmin[2] - org[2];
	ma[0] = absmax[0] - org[0];
	ma[1] = absmax[1] - org[1];
	ma[2] = absmax[2] - org[2];

	for(i = 0, f = 0.0; i < 3; ++i)
	{
		if ((float)(mi[i] * ma[i]) > 0.0)
			f = f + fminf(ma[i] * ma[i], mi[i] * mi[i]);
	}

	if(f > fogOpaqueDistSqrd)
		return 1;

	return 0;
}

float vec3Distance(const float* pointA, const float* pointB)
{
	float diffx = pointA[0]-pointB[0];
	float diffy = pointA[1]-pointB[1];
	float diffz = pointA[2]-pointB[2];
	return sqrtf(diffx*diffx + diffy*diffy + diffz*diffz);
}

void Vec3Lerp(const float *from, const float *to, float frac, float *result)
{
	result[0] = (to[0] - from[0]) * frac + from[0];
	result[1] = (to[1] - from[1]) * frac + from[1];
	result[2] = (to[2] - from[2]) * frac + from[2];
}

double DiffTrack(float tgt, float cur, float rate, float deltaTime)
{
	float step;
	float d, ad;

	d = tgt - cur;

	step = rate * d * deltaTime;
	ad = fabs(d);

	if( ad <= 0.001 || fabs(step) > ad)
	{
		return tgt;
	}

	return cur + step;
}

double DiffTrackAngle(float tgt, float cur, float rate, float deltaTime)
{
	float angle;

	while ( (tgt - cur) > 180.0 )
	{
		tgt = tgt - 360.0;
	}

	while ( (tgt - cur) < -180.0 )
	{
		tgt = tgt + 360.0;
	}

	angle = DiffTrack(tgt, cur, rate, deltaTime);
	return AngleNormalize180(angle);
}

long double AngleNormalize180Accurate(float angle)
{
	if ( angle > -180.0 )
	{
		if ( angle <= 180.0 )
		{
			return angle;
		}
		else
		{
			do
				angle = angle - 360.0;
			while ( angle > 180.0 );
			return angle;
		}
	}
	else
	{
		do
			angle = angle + 360.0;
		while ( angle <= -180.0 );
		return angle;
	}
}

long double AngleNormalize360Accurate(float angle)
{
	if ( angle >= 0.0 )
	{
		if ( angle < 360.0 )
		{
			return angle;
		}
		else
		{
			do
				angle = angle - 360.0;
			while ( angle >= 360.0 );
			return angle;
		}
	}
	else
	{
		do
			angle = angle + 360.0;
		while ( angle < 0.0 );
		return angle;
	}
}

void VectorAngleMultiply(float *vec, float angle)
{
	long double a;
	float temp;
	float x;
	float y;

	a = (angle * 0.017453292);
	x = cos(a);
	y = sin(a);

	temp = (vec[0] * x) - (vec[1] * y);
	vec[1] = (vec[1] * x) + (vec[0] * y);
	vec[0] = temp;
}

float vectoyaw( const vec3_t vec )
{
	float yaw;

	if ( vec[YAW] == 0 && vec[PITCH] == 0 )
	{
		return 0.0;
	}

	yaw = ( atan2( vec[YAW], vec[PITCH] ) * 180 / M_PI );

	if(yaw >= 0.0)
	{
		return yaw;
	}

	return yaw + 360;
}

void YawVectors2D(const float yaw, vec2_t forward, vec2_t right)
{
	float cy;
	float angle;
	float sy;

	angle = yaw * 0.017453292;
	cy = cos(angle);
	sy = sin(angle);

	if ( forward )
	{
		forward[0] = cy;
		forward[1] = sy;
	}

	if ( right )
	{
		right[0] = sy;
		right[1] = -cy;
	}
}

void YawVectors(const float yaw, vec3_t forward, vec3_t right)
{
	float cy;
	float angle;
	float sy;

	angle = yaw * 0.017453292;
	cy = cos(angle);
	sy = sin(angle);

	if ( forward )
	{
		forward[0] = cy;
		forward[1] = sy;
		forward[2] = 0.0;
	}

	if ( right )
	{
		right[0] = sy;
		right[1] = -cy;
		right[2] = 0.0;
	}
}

double PitchForYawOnNormal(const float fYaw, const vec3_t normal)
{
	vec3_t forward;

	YawVectors(fYaw, forward, 0);

	if ( normal[2] != 0 )
	{
		forward[2] = (normal[0] * forward[0] + normal[1] * forward[1]) / normal[2];
		return atan(forward[2]) * 180.0 / M_PI;
	}

	return 270.0;
}

double Abs(const float *v)
{
	return (float)sqrt((float)((float)((float)(*v * *v) + (float)(v[1] * v[1])) + (float)(v[2] * v[2])));
}

void VectorRint(float *v)
{
	v[0] = rint(v[0]);
	v[1] = rint(v[1]);
	v[2] = rint(v[2]);
}

void VectorNegCopy(float *from, float *to)
{
	to[0] = -from[0];
	to[1] = -from[1];
	to[2] = -from[2];
}