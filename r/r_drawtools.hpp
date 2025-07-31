#pragma once

#include <optional>
#include <string>

struct Material;
struct Font_s;
struct ScreenPlacement;

template<typename T>
struct vec2;

template<typename T>
struct vec3;

using fvec2 = vec2<float>;
using ivec2 = vec2<int>;

using fvec3 = vec3<float>;
using ivec3 = vec3<int>;

void CG_AdjustFrom640(float& x, float& y, float& w, float& h);
Material* R_RegisterMaterial(const char* fontname, int v);
Font_s* R_RegisterFont(const char* fontname);

char* __cdecl R_AddCmdDrawText(const char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style);

void R_AddCmdDrawStretchPic(Material* material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float* color);
void R_AddCmdDrawQuadPic(const float* verts, const float* color, Material* material);

void R_DrawRect(const char* material, float x, float y, float w, float h, const float* color);
void R_DrawText(const char* text, const char* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style);
void R_DrawText(const char* text, float x, float y, float xScale, float yScale, float rotation, float* color, int style);

void R_DrawLine(const fvec2& a, const fvec2& b, float thickness, const float* color);

void R_AddCmdDrawTextWithEffects(char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);

void R_AddCmdDrawTextWithEffects(char* text, const char* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);

void R_AddCmdDrawTextWithEffects(const std::string& text, const std::string& font, const fvec2& pos, const fvec2& scale, float rotation, 
	int style, const float* color, const float* glowCol);

void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, Material* material);
void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, const char* material);

void R_DrawTextWithEffects(const std::string& text, const char* fontname, float x, float y, float xScale, float yScale, float rotation, const float* color, int style, const float* glowColor);


ScreenPlacement* ScrPlace_ApplyRect(float* x, float* w, float* y, float* h, ScreenPlacement* placement, int horizontal, int vertical);
void CG_DrawRotatedPicPhysical(ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, Material* material);

ScreenPlacement* CG_GetScreenPlacement();

std::optional<fvec2> WorldToScreen(const fvec3& location);
std::optional<fvec2> WorldToScreenReal(const fvec3& location);

void CG_DebugBox(float* origin, float* mins, float* maxs, float yaw, float* color, bool depthtest);
void CG_DebugLine(float* origin, float* end, float* color, int depthtest, int duration);

int R_TextWidth(const char* text, Font_s* font);
int R_TextWidth(const char* text, const char* font);

float R_ScaleByDistance(float dist);
float R_GetTextCentered(const char* buff, const char* font, float x, float xScale);