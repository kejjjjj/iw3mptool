#pragma once

#include <optional>

template<typename T>
struct vec3;

using fvec3 = vec3<float>;

struct range_t
{
	float x1{};
	float x2{};
	bool split{};
};
enum axis_t : std::int8_t
{
	X = 0,
	Y = 1,
	Z = 2 
};
enum cardinal_dir
{
	N = 0,		//X+ 
	E = -90,	//Y- 
	S = 180,	//X- 
	W = 90		//Y+ 
};

float CG_ProjectionX(float angle, float fov);
range_t CG_AnglesToRange(float start, float end, float yaw, float fov);
void CG_FillAngleYaw(float start, float end, float yaw, float y, float h, float fov, const float* color);

void CG_SetYaw(const float ang);
void CG_SetPitch(const float ang);
void CG_SetRoll(const float ang);
void CG_SetPlayerAngles(const fvec3& target);

float CG_SmoothAngle(const float src, const float target, float smoothingFactor) noexcept;

struct usercmd_s;
struct playerState_s;
void CL_SetPlayerAngles(usercmd_s* cmd, float* delta_angles, const fvec3& target);
void CL_SetPlayerYaw(usercmd_s* cmd, float* delta_angles, const float target);
void CL_SetPlayerPitch(usercmd_s* cmd, float* delta_angles, const float target);

fvec3 CG_AnglesFromCmd(usercmd_s* cmd);
fvec3 CG_AnglesFromCmd(usercmd_s* cmd, playerState_s* ps);
fvec3 CG_GetClientAngles();

float AngleNormalizePI(float angle);
float AngleNormalize360(float angle);
float AngleNormalize180(float angle);
float AngleNormalize90(float angle);

float AngleWrap90(float angle);
int AngleWrap90(int angle);

float AngleDelta(float angle1, float angle2);
float AngularDistance(float value1, float value2);

axis_t CG_GetAxisBasedOnYaw(float yaw);
float CG_GetNearestCardinalAngle();
float CG_GetNearestCardinalAngle(float yaw);
cardinal_dir CG_RoundAngleToCardinalDirection(const float a);
