#include "pch.h"
#include "vec.h"



float DotProduct(const vec3_t vec1, const vec3_t vec2)
{
	return vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
}

//-------------------------------------------------------------//
void VecAdd(vec3_t in, vec3_t added)
{
	in[0] += added[0];
	in[1] += added[1];
	in[2] += added[2];
}

//-------------------------------------------------------------//
void VecSub(vec3_t in, vec3_t added)
{
	in[0] -= added[0];
	in[1] -= added[1];
	in[2] -= added[2];
}

//-------------------------------------------------------------//
void VectorSubtract(const vec3_t vec1, const vec3_t vec2, vec3_t vecOut)
{
	vecOut[0] = vec1[0] - vec2[0];
	vecOut[1] = vec1[1] - vec2[1];
	vecOut[2] = vec1[2] - vec2[2];
}

//-------------------------------------------------------------//
void VecDivByNum(vec3_t in, float numDiv)
{
	in[0] = in[0] / numDiv;
	in[1] = in[1] / numDiv;
	in[2] = in[2] / numDiv;
}

//-------------------------------------------------------------//
float VecDistance(vec3_t &start, vec3_t &end)
{
	float crd0 = (end[0] - start[0]) * (end[0] - start[0]);
	float crd1 = (end[1] - start[1]) * (end[1] - start[1]);
	float crd2 = (end[2] - start[2]) * (end[2] - start[2]);

	float distance = sqrt(crd0 + crd1 + crd2);

	return distance;
}

//-------------------------------------------------------------//
float VecDistance2(vec3_t& start, vec3_t& end)
{
	float crd0 = (end[0] - start[0]) * (end[0] - start[0]);
	float crd1 = (end[1] - start[1]) * (end[1] - start[1]);

	float distance = sqrt(crd0 + crd1);

	return distance;
}

//-------------------------------------------------------------//
void VecNull(vec3_t in)
{
	in[0] = 0;
	in[1] = 0;
	in[2] = 0;
}

//-------------------------------------------------------------//
float VecMult(vec3_t out, vec3_t in)
{
	float result = 0;
	result = out[0] * in[0] + out[1] * in[1] + out[2] * in[2];
	return result;
}

//-------------------------------------------------------------//
void VecCopy(vec3_t out, vec3_t in)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

//-------------------------------------------------------------//
void VecFill(vec3_t in, float x, float y, float z)
{
	in[0] = x;
	in[1] = y;
	in[2] = z;

}

//-------------------------------------------------------------//
void AngleVectors(float yaw, float pitch, float roll, vec3_t forward, vec3_t right, vec3_t up) 
{
	float		angle;
	static float		sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	float angleDegrees = (float)(PI * 2 / 360);

	angle = yaw * angleDegrees;
	sy = sinf(angle);
	cy = cosf(angle);
	angle = pitch * angleDegrees;
	sp = sinf(angle);
	cp = cosf(angle);
	angle = roll * angleDegrees;
	sr = sinf(angle);
	cr = cosf(angle);

	if (forward)
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
		right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
		right[2] = -1 * sr * cp;
	}
	if (up)
	{
		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}
}