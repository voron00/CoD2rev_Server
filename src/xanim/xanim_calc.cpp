#include "../qcommon/qcommon.h"

void TransformToQuatRefFrame(const float *rot, float *trans)
{
	float r;
	float ra;
	float zz;
	float zza;
	float zw;
	float temp;

	zz = *rot * *rot;
	r = (float)(rot[1] * rot[1]) + zz;

	if ( r != 0.0 )
	{
		ra = 2.0 / r;
		zza = zz * ra;
		zw = (float)(*rot * rot[1]) * ra;
		temp = (float)((float)(1.0 - zza) * *trans) + (float)(zw * trans[1]);
		trans[1] = trans[1] - (float)((float)(zw * *trans) + (float)(zza * trans[1]));
		*trans = temp;
	}
}

void Short2LerpAsVec2(const short *from, const short *to, float frac, float *out)
{
	out[0] = (float)from[0] + (float)((float)(to[0] - from[0]) * frac);
	out[1] = (float)from[1] + (float)((float)(to[1] - from[1]) * frac);
}

void Short2CopyAsVec2(const short *from, float *to)
{
	to[0] = (float)from[0];
	to[1] = (float)from[1];
}

void sub_80C1122(float *from1, float *from2, float frac, float *to)
{
	to[0] = (from2[0] - from1[0]) * frac + from1[0];
	to[1] = (from2[1] - from1[1]) * frac + from1[1];
	to[2] = (from2[2] - from1[2]) * frac + from1[2];
}

void XAnim_SetTime(float time, int frameCount, XAnimTime *animTime)
{
	animTime->time = time;
	animTime->frameCount = frameCount;
	animTime->frameFrac = (float)frameCount * time;
	animTime->frameIndex = (int)animTime->frameFrac;
}