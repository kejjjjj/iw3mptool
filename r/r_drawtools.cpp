#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "r_drawtools.hpp"
#include "utils/engine.hpp"
#include "utils/typedefs.hpp"

#include <algorithm>

void CG_AdjustFrom640(float& x, float& y, float& w, float& h)
{
	const float scale = (float)cgs->refdef.width / 640.f;
	const float scaleY = (float)cgs->refdef.height / 480.f;

	x *= scale;
	y *= scaleY;
	w *= scale;
	h *= scaleY;
}
void CG_AdjustTo640(float& x, float& y, float& w, float& h)
{
	const float scale = 640.f / (float)cgs->refdef.width;
	const float scaleY = 480.f / (float)cgs->refdef.height;

	x *= scale;
	y *= scaleY;
	w *= scale;
	h *= scaleY;
}
Material* R_RegisterMaterial(const char* mtl)
{
	return ((Material * (*)(const char* mtl, int size))0x5F2A80)(mtl, sizeof(mtl));

}
Font_s* R_RegisterFont(const char* fontname)
{
	return ((Font_s * (*)(const char* fontname, int size))0x5F1EC0)(fontname, sizeof(fontname));
}
void R_DrawRect(const char* material, float x, float y, float w, float h, const float* color)
{
	CG_AdjustFrom640(x, y, w, h);
	Material* mat = R_RegisterMaterial(material);
	R_AddCmdDrawStretchPic(mat, (x), (y), (w), (h), 0, 0, 0, 0, color);


}
void R_AddCmdDrawStretchPic(Material* material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float* color)
{
	//CG_AdjustFrom640(x, y, w, h);

	const static uint32_t R_AddCmdDrawStretchPic_func = 0x5F65F0;
	__asm
	{
		pushad;
		push	color;
		mov		eax, [material];
		sub		esp, 20h;
		fld		t1;
		fstp[esp + 1Ch];
		fld		s1;
		fstp[esp + 18h];
		fld		t0;
		fstp[esp + 14h];
		fld		s0;
		fstp[esp + 10h];
		fld		h;
		fstp[esp + 0Ch];
		fld		w;
		fstp[esp + 8h];
		fld		y;
		fstp[esp + 4h];
		fld		x;
		fstp[esp];
		call	R_AddCmdDrawStretchPic_func;
		add		esp, 24h;
		popad;
	}
}
void R_AddCmdDrawQuadPic(const float* verts, const float* color, Material* material)
{
	__asm
	{
		push material;
		mov ecx, color;
		mov edx, verts;
		mov esi, 0x5F7230;
		call esi;
		add esp, 0x4;
	}
}
char* __cdecl R_AddCmdDrawText(const char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style)
{
	const static uint32_t R_AddCmdDrawText_func = 0x5F6B00;
	__asm
	{
		push	style;
		sub     esp, 14h;
		fld		rotation;
		fstp[esp + 10h];
		fld		yScale;
		fstp[esp + 0Ch];
		fld		xScale;
		fstp[esp + 8];
		fld		y;
		fstp[esp + 4];
		fld		x;
		fstp[esp];
		push	font;
		push	maxChars;
		push	text;
		mov		ecx, [color];
		call	R_AddCmdDrawText_func;
		add		esp, 24h;
	}
}
void R_AddCmdDrawTextWithEffects(char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
	const static uint32_t R_AddCmdDrawTextWithEffects_f = 0x5F6D30;
	CG_AdjustFrom640(x, y, xScale, yScale);

	return ((void(__cdecl*)(char* text, int maxChars, Font_s * font, float x, float y, float xScale, float yScale, float rotation,
		const float* color, int style, const float* glowColor, Material * fxMaterial, Material * fxMaterialGlow,
		int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration))0x5F6D30)(text, maxChars, font, x, y, xScale, yScale, rotation,
			color, style, glowColor, fxMaterial, fxMaterialGlow,
			fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);
}
void R_AddCmdDrawTextWithEffects(char* text, const char* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
	Font_s* _font = R_RegisterFont(font);
	R_AddCmdDrawTextWithEffects(text, 0x7FFFFFFF, _font, x, y, xScale, yScale, rotation, color, style, glowColor, fxMaterial, fxMaterialGlow, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);

}
void R_AddCmdDrawTextWithEffects(const std::string& text, const std::string& font, const fvec2& pos, const fvec2& scale, float rotation,
	int style, const float* color, const float* glowCol)
{
	Font_s* _font = R_RegisterFont(font.c_str());

	R_AddCmdDrawTextWithEffects((char*)text.c_str(), text.length(), _font, pos.x, pos.y, scale.x, scale.y, rotation, color, style, glowCol, nullptr, nullptr, 0, 0, 0, 0);


}
void R_DrawTextWithEffects(const std::string& text, const char* fontname, float x, float y, float xScale, float yScale, float rotation, float* color, int style, float* glowColor)
{
	Material* fxMaterial = R_RegisterMaterial("decode_characters");
	Material* fxMaterialGlow = R_RegisterMaterial("decode_characters_glow");

	if (!fxMaterial || !fxMaterialGlow)
		return;

	CG_AdjustFrom640(x, y, xScale, yScale);

	//CG_AdjustFrom640(x, y, xScale, yScale);
	return R_AddCmdDrawTextWithEffects((char*)text.c_str(), fontname, x, y, xScale, yScale, rotation, color, style, glowColor, fxMaterial, fxMaterialGlow, 0, 500, 1000, 2000);
}
ScreenPlacement* CG_GetScreenPlacement()
{
	return (ScreenPlacement*)((68 * 0 + 14894040));
}
void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* ascrPlace, float x, float y, float w, float h, float angle, float* color, const char* material)
{
	//CG_AdjustTo640(x, y, w, h);
	Material* mat = R_RegisterMaterial(material);
	return CG_DrawRotatedPic(vertical, horizontal, ascrPlace, x, y, w, h, angle, color, mat);
}
void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* ascrPlace, float x, float y, float w, float h, float angle, float* color, Material* material)
{
	ScrPlace_ApplyRect(&x, &w, &y, &h, ascrPlace, horizontal, vertical);
	return CG_DrawRotatedPicPhysical(ascrPlace, x, y, w, h, angle, color, material);
}
ScreenPlacement* ScrPlace_ApplyRect(float* x, float* w, float* y, float* h, ScreenPlacement* placement, int horizontal, int vertical)
{
	ScreenPlacement* result; // eax
	float _x; // st6
	float _w; // st6

	result = placement;
	switch (horizontal)
	{
	case 1:
		_x = placement->scaleVirtualToReal[0] * *x + placement->realViewableMin[0];
		goto LABEL_9;
	case 2:
		_x = placement->scaleVirtualToReal[0] * *x + placement->realViewportSize[0] * 0.5f;
		goto LABEL_9;
	case 3:
		_x = placement->scaleVirtualToReal[0] * *x + placement->realViewableMax[0];
		goto LABEL_9;
	case 4:
		*x = placement->scaleVirtualToFull[0] * *x;
		_w = placement->scaleVirtualToFull[0];
		goto OOF;
	case 5:
		goto LABEL_11;
	case 6:
		*x = placement->scaleRealToVirtual[0] * *x;
		_w = placement->scaleRealToVirtual[0];
		goto OOF;
	case 7:
		_x = (placement->realViewableMax[0] + placement->realViewableMin[0]) * 0.5f + placement->scaleVirtualToReal[0] * *x;
		goto LABEL_9;
	default:
		_x = placement->scaleVirtualToReal[0] * *x + placement->subScreenLeft;// crash
	LABEL_9:
		*x = _x;
		_w = placement->scaleVirtualToReal[0];
	OOF:
		*w = _w * *w;
	LABEL_11:
		switch (vertical)
		{
		case 1:
			*y = *y * placement->scaleVirtualToReal[1] + placement->realViewableMin[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		case 2:
			*y = 0.5f * placement->realViewportSize[1] + *y * placement->scaleVirtualToReal[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		case 3:
			*y = *y * placement->scaleVirtualToReal[1] + placement->realViewableMax[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		case 4:
			*y = *y * placement->scaleVirtualToFull[1];
			*h = placement->scaleVirtualToFull[1] * *h;
			break;
		case 5:
			return result;
		case 6:
			*y = *y * placement->scaleRealToVirtual[1];
			*h = placement->scaleRealToVirtual[1] * *h;
			break;
		case 7:
			*y = 0.5f * (placement->realViewableMax[1] + placement->realViewableMin[1]) + *y * placement->scaleVirtualToReal[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		default:
			*y = *y * placement->scaleVirtualToReal[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		}
		return result;
	}
}
void CG_DrawRotatedPicPhysical(ScreenPlacement* screenPlace, float x, float y, float w, float h, float angle, float* color, Material* material)
{

	float v8; // st7
	float v9; // st4
	float v10; // st3
	float v11; // st6
	float v12; // st2
	float v13; // st7
	float v14; // st5
	float v16; // [esp+0h] [ebp-44h] BYREF
	float v17; // [esp+Ch] [ebp-38h]
	float v18; // [esp+10h] [ebp-34h]
	float v19; // [esp+14h] [ebp-30h]
	float v20; // [esp+18h] [ebp-2Ch]
	float v21; // [esp+1Ch] [ebp-28h]
	float v22; // [esp+20h] [ebp-24h]
	float verts[8]{}; // [esp+24h] [ebp-20h] BYREF
	float wa; // [esp+50h] [ebp+Ch]
	float wb; // [esp+50h] [ebp+Ch]
	float wc; // [esp+50h] [ebp+Ch]
	float wd; // [esp+50h] [ebp+Ch]
	float we; // [esp+50h] [ebp+Ch]
	float wf; // [esp+50h] [ebp+Ch]
	float wg; // [esp+50h] [ebp+Ch]
	float wh; // [esp+50h] [ebp+Ch]
	float wi; // [esp+50h] [ebp+Ch]
	float wj; // [esp+50h] [ebp+Ch]
	float wk; // [esp+50h] [ebp+Ch]
	float wl; // [esp+50h] [ebp+Ch]
	float ha; // [esp+54h] [ebp+10h]

	v16 = angle;
	v17 = angle * 0.01745329238474369f;
	angle = cos(v17);
	v16 = sin(v17);
	wa = w * 0.5f;
	v17 = wa * screenPlace->scaleRealToVirtual[0];
	wb = 0.5f * h;
	ha = wb * screenPlace->scaleRealToVirtual[1];
	wc = x * screenPlace->scaleRealToVirtual[0];
	v8 = v17;
	v17 = wc + v17;
	wd = screenPlace->scaleRealToVirtual[1] * y;
	v18 = wd + ha;
	v19 = v8 * angle;
	v20 = v8 * v16;
	v21 = v16 * ha;
	v22 = angle * ha;
	v9 = v17 - v19 * 1.0f;
	v10 = v21 * -1.0f;
	we = v9 - v10;
	verts[0] = we * screenPlace->scaleVirtualToReal[0];
	v11 = v18 - 1.0f * v20;
	v12 = -1.0f * v22;
	wf = v12 + v11;
	verts[1] = wf * screenPlace->scaleVirtualToReal[1];
	v13 = v17 + v19;
	wg = v13 - v10;
	verts[2] = wg * screenPlace->scaleVirtualToReal[0];
	v14 = v20 + v18;
	wh = v12 + v14;
	verts[3] = wh * screenPlace->scaleVirtualToReal[1];
	wi = v13 - v21;
	verts[4] = wi * screenPlace->scaleVirtualToReal[0];
	wj = v14 + v22;
	verts[5] = wj * screenPlace->scaleVirtualToReal[1];
	wk = v9 - v21;
	verts[6] = wk * screenPlace->scaleVirtualToReal[0];
	wl = v22 + v11;
	verts[7] = wl * screenPlace->scaleVirtualToReal[1];

	__asm
	{
		mov edx, material;
		push material;
		lea edx, verts;
		mov ecx, color;
		mov esi, 0x5F7230;
		call esi;
		add esp, 0x4;
	}

}
void R_DrawText(const char* text, const char* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style)
{

	Font_s* _font = R_RegisterFont(font);

	CG_AdjustFrom640(x, y, xScale, yScale);

	R_AddCmdDrawText(text, 0x7FFFFFFF, _font, (x), (y), xScale, yScale, rotation, color, style);

}
void R_DrawText(const char* text, float x, float y, float xScale, float yScale, float rotation, float* color, int style)
{
	R_DrawText(text, "fonts/smalldevfont", x, y, xScale, yScale, rotation, color, style);
}
void R_DrawLine(const fvec2& a, const fvec2& b, float thickness, const float* color)
{
	struct vertex_angle {
		float v[2];
		float angle;
	};

	const fvec2 sub = b - a;
	float line_angle = (std::atan2(-sub.y, -sub.x));
	const fvec2 center = (a + b) * 0.5f;

	vertex_angle verts[4]{};

	verts[0].v[0] = a.x;
	verts[0].v[1] = a.y;
	verts[0].angle = std::atan2(verts[0].v[1] - center.y, verts[0].v[0] - center.x);

	verts[1].v[0] = b.x;
	verts[1].v[1] = b.y;
	verts[1].angle = std::atan2(verts[1].v[1] - center.y, verts[1].v[0] - center.x);

	verts[2].v[0] = b.x + (std::cos((line_angle)) + 1) * thickness;
	verts[2].v[1] = b.y - (std::sin((line_angle)) + 1) * thickness;
	verts[2].angle = std::atan2(verts[2].v[1] - center.y, verts[2].v[0] - center.x);

	verts[3].v[0] = a.x + (std::cos((line_angle)) + 1) * thickness;
	verts[3].v[1] = a.y - (std::sin((line_angle)) + 1) * thickness;
	verts[3].angle = std::atan2(verts[3].v[1] - center.y, verts[3].v[0] - center.x);

	std::sort(verts, verts + 4, [](const vertex_angle& a, const vertex_angle& b) { return a.angle < b.angle; });

	float v[4][2]{};

	for (int i = 0; i < 4; i++) {
		v[i][0] = verts[i].v[0];
		v[i][1] = verts[i].v[1];
	}

	Material* m = R_RegisterMaterial("white");
	R_AddCmdDrawQuadPic((float*)v, color, m);

}
std::optional<fvec2> WorldToScreen(const fvec3& location)
{
	const refdef_s* refdef = &cgs->refdef;

	constexpr int centerX = 640 / 2;
	constexpr int centerY = 480 / 2;

	const fvec3 vright = refdef->viewaxis[1];
	const fvec3 vup = refdef->viewaxis[2];
	const fvec3 vfwd = refdef->viewaxis[0];


	const fvec3 vLocal = location - refdef->vieworg;
	fvec3 vTransform;

	vTransform.x = vLocal.dot(vright);
	vTransform.y = vLocal.dot(vup);
	vTransform.z = vLocal.dot(vfwd);

	if (vTransform.z < 0.01) {
		return std::nullopt;
	}
	fvec2 out;

	out.x = static_cast<float>(centerX) * (1.f - (vTransform.x / refdef->tanHalfFovX / vTransform.z));
	out.y = static_cast<float>(centerY) * (1.f - (vTransform.y / refdef->tanHalfFovY / vTransform.z));


	if (vTransform.z > 0)
		return out;

	return std::nullopt;
}
std::optional<fvec2> WorldToScreenReal(const fvec3& location)
{
	const refdef_s* refdef = &cgs->refdef;

	const int centerX = cgs->refdef.width / 2;
	const int centerY = cgs->refdef.height / 2;

	const fvec3 vright = refdef->viewaxis[1];
	const fvec3 vup = refdef->viewaxis[2];
	const fvec3 vfwd = refdef->viewaxis[0];

	const fvec3 vLocal = location - refdef->vieworg;
	fvec3 vTransform;

	vTransform.x = vLocal.dot(vright);
	vTransform.y = vLocal.dot(vup);
	vTransform.z = vLocal.dot(vfwd);

	if (vTransform.z < 0.01) {
		return std::nullopt;
	}
	fvec2 out;

	out.x = static_cast<float>(centerX) * (1.f - (vTransform.x / refdef->tanHalfFovX / vTransform.z));
	out.y = static_cast<float>(centerY) * (1.f - (vTransform.y / refdef->tanHalfFovY / vTransform.z));


	if (vTransform.z > 0)
		return out;

	return std::nullopt;
}
void CG_DebugBox(float* origin, float* mins, float* maxs, float yaw, float* color, bool depthtest)
{
	static bool depth_test = false;

	if (depth_test != depthtest) {
		Engine::Tools::write_bytes(0x00431E70ul, depthtest ? "\x01" : "\x00");
		depth_test = depthtest;
	}

	__asm
	{
		mov eax, maxs;
		mov ecx, origin;
		push color;
		push yaw;
		push mins;
		mov esi, 0x431CE0;
		call esi;
		add esp, 0xC;
	}
}
void CG_DebugLine(float* origin, float* end, float* color, int depthtest, int duration)
{
	__asm
	{
		push duration;
		push depthtest;
		push color;
		push end;
		mov edi, origin;
		mov esi, 0x4B6A50;
		call esi;
		add esp, 16;
	}
}

int R_TextWidth(const char* text, Font_s* font)
{
	int result = 0;
	__asm
	{
		mov eax, text;
		push font;
		push 0x7FFFFFFF;
		mov esi, 0x5F1EE0;
		call esi;
		add esp, 8;
		mov result, eax;

	}

	return result;
}
int R_TextWidth(const char* text, const char* font)
{
	Font_s* _font = R_RegisterFont(font);
	return R_TextWidth(text, _font);
}

float R_ScaleByDistance(float dist)
{
	float d_max = 10000.0;
	float scale_max = 7.f;

	dist = std::max(0.0f, std::min(d_max, dist));

	float scale = 2.f - scale_max * (dist / (d_max));

	return std::clamp(scale, 0.1f, 2.f);
}

float R_GetTextCentered(const char* buff, const char* font, float x, float xScale)
{
	return x - (static_cast<float>(R_TextWidth(buff, font)) * xScale) / 2;
}
