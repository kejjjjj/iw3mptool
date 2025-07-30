#include "cg_angles.hpp"
#include "cg_local.hpp"
#include "cg_offsets.hpp"
#include "global/defs.hpp"
#include "r/r_drawtools.hpp"
#include "utils/resolution.hpp"
#include "utils/typedefs.hpp"

#include <algorithm>
#include <cassert>

float CG_ProjectionX(float angle, float fov)
{
	const float SCREEN_WIDTH = static_cast<float>(gfxCmdBufSourceState->renderTargetWidth);
	float const half_fov_x = DEG2RAD(fov) / 2;
	if (angle >= half_fov_x)
	{
		return 0;
	}
	if (angle <= -half_fov_x)
	{
		return SCREEN_WIDTH;
	}

	return SCREEN_WIDTH / 2.f * (1.f - angle / half_fov_x);

}
range_t CG_AnglesToRange(float start, float end, float yaw, float fov)
{
	start = DEG2RAD(start);
	end = DEG2RAD(end);
	yaw = DEG2RAD(yaw);
	fov = (fov);
	if (fabsf(end - start) > 2 * (float)M_PI)
	{
		return range_t{ 0, float(gfxCmdBufSourceState->renderTargetWidth), false };
	}

	bool split = end > start;
	start = AngleNormalizePI(start - yaw);
	end = AngleNormalizePI(end - yaw);

	if (end > start)
	{
		split = !split;
		std::swap(start, end);
	}

	return range_t{ CG_ProjectionX(start, fov), CG_ProjectionX(end, fov), split };

}
void CG_FillAngleYaw(float start, float end, float yaw, float y, float h, float fov, const float* color)
{
	if (start == end)
		return;

	range_t range = CG_AnglesToRange(start, end, yaw, fov);

	range.x1 = adjust_to_640(static_cast<int>(range.x1));
	range.x2 = adjust_to_640(static_cast<int>(range.x2));

	if (!range.split)
		R_DrawRect("white", range.x1, y, range.x2 - range.x1, h, color);
	else {
		R_DrawRect("white", 0, y, range.x1, h, color);
		R_DrawRect("white", range.x2, y, gfxCmdBufSourceState->renderTargetWidth - range.x2, h, color);
	}
}
void CG_SetYaw(const float ang)
{
	float ref = clients->cgameViewangles[YAW];
	//ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[1] += ref;

}
void CG_SetPitch(const float ang)
{
	float ref = clients->cgameViewangles[PITCH];
	ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[0] += ref;

}
void CG_SetRoll(const float ang)
{
	float ref = clients->cgameViewangles[ROLL];
	ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[2] += ref;

}
void CG_SetPlayerAngles(const fvec3& target)
{
	CG_SetPitch(target.x);
	CG_SetYaw(target.y);
	CG_SetRoll(target.z);
}
float CG_SmoothAngle(const float src, const float dst, float smoothingFactor) noexcept {

	smoothingFactor = 1.f - std::max(0.f, std::min(1.f, smoothingFactor));
	return src + smoothingFactor * (dst - src);
}
void CL_SetPlayerAngles(usercmd_s* cmd, float* delta_angles, const fvec3& target)
{
	for (int i = 0; i < 3; i++) {

		float cmdAngle = SHORT2ANGLE(cmd->angles[i]);

		float delta = AngleDelta(delta_angles[i], cmdAngle);
		float real_delta = AngleDelta(delta, delta_angles[i]);
		float final = AngleDelta(delta_angles[i], target[i]);

		clients->viewangles[i] += real_delta - final;
		cmd->angles[i] += ANGLE2SHORT(real_delta - final);

	}
}
void CL_SetPlayerAngle(usercmd_s* cmd, float* delta_angles, const float target, int AXIS)
{
	float cmdAngle = SHORT2ANGLE(cmd->angles[AXIS]);

	float delta = AngleDelta(delta_angles[AXIS], cmdAngle);
	float real_delta = AngleDelta(delta, delta_angles[AXIS]);
	float final = AngleDelta(delta_angles[AXIS], target);

	clients->viewangles[AXIS] += real_delta - final;
	cmd->angles[AXIS] += ANGLE2SHORT(real_delta - final);
}
void CL_SetPlayerYaw(usercmd_s* cmd, float* delta_angles, const float target)
{
	return CL_SetPlayerAngle(cmd, delta_angles, target, YAW);
}
void CL_SetPlayerPitch(usercmd_s* cmd, float* delta_angles, const float target)
{
	return CL_SetPlayerAngle(cmd, delta_angles, target, PITCH);
}
fvec3 CG_AnglesFromCmd(usercmd_s* cmd)
{
	return (ivec3(cmd->angles).from_short() + cgs->predictedPlayerState.delta_angles).normalize180();
}
fvec3 CG_AnglesFromCmd(usercmd_s* cmd, playerState_s* ps)
{
	return (ivec3(cmd->angles).from_short() + ps->delta_angles).normalize180();

}
fvec3 CG_GetClientAngles()
{
	return (fvec3(clients->viewangles) + cgs->predictedPlayerState.delta_angles).normalize180();
}
axis_t CG_GetAxisBasedOnYaw(float yaw)
{
	float nearest = CG_GetNearestCardinalAngle(yaw);
	return (nearest == 0 || nearest == 180 ? axis_t::X : axis_t::Y);
}
cardinal_dir CG_RoundAngleToCardinalDirection(const float a)
{
	const float rounded = CG_GetNearestCardinalAngle(a);

	if (rounded == 0)
		return cardinal_dir::N;
	if (rounded == 90)
		return cardinal_dir::W;
	if (rounded == 270)
		return cardinal_dir::E;
	return cardinal_dir::S;
}


float CG_GetNearestCardinalAngle(float yawangle)
{
	if (yawangle > 180 || yawangle < -180)
		yawangle = AngleNormalize180(yawangle);

	if (yawangle >= 135 || yawangle <= -135)
		return 180;  //X backward
	else if (yawangle <= -45 && yawangle >= -135)
		return 270; //Y backward
	else if (yawangle >= 45 && yawangle <= 135)
		return 90;  //Y forward
	return 0; // x forward


}
float CG_GetNearestCardinalAngle()
{
	const float yawangle = clients->cgameViewangles[YAW];

	if (yawangle >= 135 || yawangle <= -135)
		return 180;  //X backward
	else if (yawangle <= -45 && yawangle >= -135)
		return 270; //Y backward
	else if (yawangle >= 45 && yawangle <= 135)
		return 90;  //Y forward
	return 0; // x forward


}

float AngleNormalizePI(float angle)
{
	angle = fmodf(angle + (float)M_PI, 2 * (float)M_PI);
	return angle < 0 ? angle + (float)M_PI : angle - (float)M_PI;
}
float AngleNormalize360(float angle) {
	return (360.0f / 65536) * ((int)(angle * (65536 / 360.0f)) & 65535);
}
float AngleNormalize180(float angle) {
	angle = AngleNormalize360(angle);
	if (angle > 180.0) {
		angle -= 360.0;
	}
	return angle;
}
float AngleNormalize90(float angle)
{
	return fmodf(angle + 180 + 90, 180) - 90;
}
float AngleWrap90(float angle)
{
	angle = AngleNormalize180(angle);

	if (angle > -180 && angle <= -90)
		return angle + 180;
	else if (angle > -90 && angle < 0)
		return angle + 90;
	else if (angle >= 0 && angle <= 90)
		return angle;
	else if (angle > 90 && angle < 180)
		return angle - 90;
	
	return 0;
}
int AngleWrap90(int angle)
{
	angle = static_cast<int>(AngleNormalize180(static_cast<float>(angle)));

	if (angle > -180 && angle <= -90)
		return angle + 180;
	else if (angle > -90 && angle < 0)
		return angle + 90;
	else if (angle >= 0 && angle <= 90)
		return angle;
	else if (angle > 90 && angle < 180)
		return angle - 90;

	return 0;
}
float AngleDelta(float angle1, float angle2) {
	return AngleNormalize180(angle1 - angle2);
}
float AngularDistance(float value1, float value2) {
	float diff = fmodf(value2 - value1 + 180, 360) - 180;
	if (diff < -180) {
		diff += 360;
	}
	return std::abs(diff);
}