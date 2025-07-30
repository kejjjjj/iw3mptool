#include "com_vector.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cg/cg_angles.hpp"

int BoxOnPlaneSide(const fvec3& emins, const fvec3& emaxs, struct cplane_s* p)
{
	float dist1, dist2;

	switch (p->signbits)
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

	return (2 * (dist2 < p->dist)) | (dist1 > p->dist);
}

void BuildFrustumPlanes(cplane_s* frustumPlanes)
{

	for (int i = 0; i < 5; i++) {

		cplane_s* plane = &frustumPlanes[i];
		auto dpvs = &dpvsGlob->views[0].frustumPlanes[i];

		VectorCopy(dpvs->coeffs, plane->normal);
		plane->dist = dpvs->coeffs[3] * -1;

		char signbit = 0;

		if (plane->normal[0] != 1.f) {
			if (plane->normal[1] == 1.f)
				signbit = 1;
			else {
				signbit = plane->normal[2] == 1.f ? 2 : 3;
			}
		}
		
		plane->type = signbit;

		SetPlaneSignbits(plane);

	}
}
void CreateFrustumPlanes(cplane_s* frustum_planes)
{
	BuildFrustumPlanes(frustum_planes);

	frustum_planes[5].normal[0] = -frustum_planes[4].normal[0];
	frustum_planes[5].normal[1] = -frustum_planes[4].normal[1];
	frustum_planes[5].normal[2] = -frustum_planes[4].normal[2];

	frustum_planes[5].dist = -frustum_planes[4].dist - 2000;
	auto plane = &frustum_planes[5];

	char signbit = 0;

	if (plane->normal[0] != 1.f) {
		if (plane->normal[1] == 1.f)
			signbit = 1;
		else {
			signbit = plane->normal[2] == 1.f ? 2 : 3;
		}
	}

	plane->type = signbit;

	SetPlaneSignbits(plane);
}
void SetPlaneSignbits(cplane_s* out)
{
	char bits, j;

	// for fast box on planeside test
	bits = 0;
	for (j = 0; j < 3; j++) {
		if (out->normal[j] < 0) {
			bits |= 1 << j;
		}
	}
	out->signbits = bits;
}

void PlaneFromPointsASM(float* plane, float* v0, float* v1, float* v2)
{
	static constexpr auto fnc = 0x562050;
	__asm
	{
		mov esi, v0;
		mov edi, plane;
		mov ecx, v1;
		mov edx, v2;
		call fnc;
	}
}


fvec3 SetSurfaceBrightness(const fvec3& color, const fvec3& normal, const fvec3& lightDirection) {

	fvec3 normalizedLight = (lightDirection).normalize();

	float dotProduct = normalizedLight.dot(normal);

	float brightnessAdjustment = 1.f - std::max(0.0f, dotProduct);
	float maxComponent = std::max({ color.x, color.y, color.z });
	brightnessAdjustment *= maxComponent;

	return color * brightnessAdjustment;

}

fvec3 VectorRotate(const fvec3& vIn, const fvec3& vRotation)
{
	fvec3 vWork, va;
	VectorCopy(vIn, va);
	VectorCopy(va, vWork);
	int nIndex[3][2] = {};
	nIndex[0][0] = 1; nIndex[0][1] = 2;
	nIndex[1][0] = 2; nIndex[1][1] = 0;
	nIndex[2][0] = 0; nIndex[2][1] = 1;

	for (int i = 0; i < 3; i++)
	{
		if (vRotation[i] != 0.000000f)
		{
			float dAngle = vRotation[i] * PI / 180.0f;
			float c = cos(dAngle);
			float s = sin(dAngle);
			vWork[nIndex[i][0]] = va[nIndex[i][0]] * c - va[nIndex[i][1]] * s;
			vWork[nIndex[i][1]] = va[nIndex[i][0]] * s + va[nIndex[i][1]] * c;
		}
		VectorCopy(vWork, va);
	}

	return vWork;
}
fvec3 VectorRotate(const fvec3& vIn, const fvec3& vRotation, const fvec3& vOrigin)
{
	fvec3 vRotation2 = vRotation;
	std::swap(vRotation2.y, vRotation2.z);
	std::swap(vRotation2.x, vRotation2.y);

	fvec3 vTemp;
	VectorSubtract(vIn, vOrigin, vTemp);
	fvec3 vTemp2 = VectorRotate(vTemp, vRotation2);

	return vTemp2 + vOrigin;
}

void MatrixMultiply(float in1[3][3], float in2[3][3], float out[3][3]) {
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
fvec3 ProjectPointOnPlane(const fvec3& p, const fvec3& normal) {
	float inv_denom = {};

	inv_denom = 1.0F / DotProduct(normal, normal);

	const auto d = DotProduct(normal, p) * inv_denom;
	const fvec3 n = normal * inv_denom;
	return p - (n * d);
}
fvec3 PerpendicularVector(const fvec3& src){
	int pos = {};
	int i = {};
	float minelem = 1.0F;
	vec3_t tempvec = {};

	/*
	** find the smallest magnitude axially aligned vector
	*/
	for (pos = 0, i = 0; i < 3; i++)
	{
		if (fabs(src[i]) < minelem) {
			pos = i;
			minelem = fabs(src[i]);
		}
	}
	tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
	tempvec[pos] = 1.0F;

	/*
	** project the point onto the plane defined by src
	*/
	const auto dst = ProjectPointOnPlane(tempvec, src);

	/*
	** normalize the result
	*/
	return dst.normalize();
}

fvec3 RotatePointAroundVector(const fvec3& dir, const fvec3& point, float degrees) {
	float m[3][3] = {};
	float im[3][3] = {};
	float zrot[3][3] = {};
	float tmpmat[3][3] = {};
	float rot[3][3] = {};
	int i = {};
	float rad = {};

	const auto& vf = dir;

	const auto vr = PerpendicularVector(dir);
	const auto vup = vr.cross(vf);

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy(im, m, sizeof(im));

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset(zrot, 0, sizeof(zrot));
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	rad = DEG2RAD(degrees);
	zrot[0][0] = cos(rad);
	zrot[0][1] = sin(rad);
	zrot[1][0] = -sin(rad);
	zrot[1][1] = cos(rad);

	MatrixMultiply(m, zrot, tmpmat);
	MatrixMultiply(tmpmat, im, rot);

	fvec3 dst;
	for (i = 0; i < 3; i++) {
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}

	return dst;
}

fvec3 AxisToAngles(const vec3_t axis[3]) {
	vec3_t axisDefault[3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

	const auto angles = fvec3(axis[0]).toangles();
	
	// now get the roll from the right vector
	fvec3 right = axis[1];

	// get the angle difference between the tmpAxis[2] and axis[2] after they have been reverse-rotated
	const auto tvec = RotatePointAroundVector(axisDefault[2], right, -angles[YAW]);
	right = RotatePointAroundVector(axisDefault[1], tvec, -angles[PITCH]);

	// now find the angles, the PITCH is effectively our ROLL
	const auto roll_angles = right.toangles();

	roll_angles[PITCH] = AngleNormalize180(roll_angles[PITCH]);
	// if the yaw is more than 90 degrees difference, we should adjust the pitch
	if (DotProduct(right, axisDefault[1]) < 0) {
		if (roll_angles[PITCH] < 0) {
			roll_angles[PITCH] = -90 + (-90 - roll_angles[PITCH]);
		}
		else {
			roll_angles[PITCH] = 90 + (90 - roll_angles[PITCH]);
		}
	}

	angles[ROLL] = -roll_angles[PITCH];

	return angles;
}

void vectoangles(const vec3_t value1, vec3_t angles) {
	float forward;
	float yaw, pitch;

	if (value1[1] == 0 && value1[0] == 0) {
		yaw = 0;
		if (value1[2] > 0) {
			pitch = 90;
		}
		else {
			pitch = 270;
		}
	}
	else {
		if (value1[0]) {
			yaw = (atan2(value1[1], value1[0]) * 180 / M_PI);
		}
		else if (value1[1] > 0) {
			yaw = 90;
		}
		else {
			yaw = 270;
		}
		if (yaw < 0) {
			yaw += 360;
		}
		forward = sqrt(value1[0] * value1[0] + value1[1] * value1[1]);
		pitch = (atan2(value1[2], forward) * 180 / M_PI);
		if (pitch < 0) {
			pitch += 360;
		}
	}

	angles[PITCH] = -pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}


bool BoundsInView(const fvec3& mins, const fvec3& maxs, struct cplane_s* frustumPlanes, int numPlanes)
{
	if (numPlanes <= 0)
		return true;

	cplane_s* plane = frustumPlanes;
	auto idx = 0;
	while ((BoxOnPlaneSide(mins, maxs, plane++) & 1) != 0) {
		if (++idx >= numPlanes)
			return true;
	}

	return false;
}

int Vec3PackUnitVec(float* v) {
	int r = 0;
	__asm
	{
		mov eax, v;
		mov esi, 0x5645A0;
		call esi;
		mov r, eax;
	}
	return r;
}