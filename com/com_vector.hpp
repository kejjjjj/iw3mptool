#pragma once

#include "utils/typedefs.hpp"
#include "global/defs.hpp"

[[nodiscard]] int BoxOnPlaneSide(const fvec3& emins, const fvec3& emaxs, struct cplane_s* p);

void BuildFrustumPlanes(cplane_s* frustumPlanes);
void SetPlaneSignbits(cplane_s* out);
void CreateFrustumPlanes(cplane_s* frustumPlanes);
void PlaneFromPointsASM(float* plane, float* v0, float* v1, float* v2);



[[nodiscard]] fvec3 SetSurfaceBrightness(const fvec3& color, const fvec3& normal, const fvec3& lightDirection);
[[nodiscard]] fvec3 VectorRotate(const fvec3& va, const fvec3& vb);
[[nodiscard]] fvec3 VectorRotate(const fvec3& vIn, const fvec3& vRotation, const fvec3& vOrigin);
[[nodiscard]] fvec3 PerpendicularVector(const fvec3& src);
[[nodiscard]] fvec3 RotatePointAroundVector(const fvec3& dir, const fvec3& point, float degrees);
[[nodiscard]] fvec3 AxisToAngles(const vec3_t axis[3]);
[[nodiscard]] bool BoundsInView(const fvec3& mins, const fvec3& maxs, struct cplane_s* frustumPlanes, int numPlanes);
[[nodiscard]] int Vec3PackUnitVec(float* v);

void AnglesToAxis(const vec3_t angles, vec3_t axis[3]);
void MatrixMultiply(float in1[3][3], float in2[3][3], float out[3][3]);
