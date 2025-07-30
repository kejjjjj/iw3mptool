#include <array>
#include <ranges>

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "com/com_vector.hpp"
#include "rb_endscene.hpp"
#include "utils/hook.hpp"
#include "utils/typedefs.hpp"

#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])

void RB_DrawPolyInteriors(const std::vector<fvec3>& points, const float* c, bool two_sided, bool depthTest)
{

	auto n_points = points.size();

	//partly copied from iw3xo :)
	if (n_points < 3u)
		return;

	GfxColor color;
	R_ConvertColorToBytes(c, &color);

	RB_BeginSurfaceInternal(two_sided, depthTest);


	if (RB_CheckTessOverflow(n_points, 3 * (n_points - 2)))
		RB_TessOverflow(two_sided, depthTest);

	for(const auto idx : std::views::iota(std::size_t{0}, n_points))
		RB_SetPolyVertice(points[idx].As<float*>(), &color, tess->vertexCount + idx, idx, 0);
	

	for (const auto idx : std::views::iota(std::size_t{2}, n_points)) {
		tess->indices[tess->indexCount + 0] = static_cast<short>(tess->vertexCount);
		tess->indices[tess->indexCount + 1] = static_cast<short>(idx + tess->vertexCount);
		tess->indices[tess->indexCount + 2] = static_cast<short>(idx + tess->vertexCount - 1);
		tess->indexCount += 3;
	}

	tess->vertexCount += n_points;

	RB_EndTessSurface();

}
int RB_AddDebugLine(GfxPointVertex* verts, char depthTest, const float* start, const float* end, const float* color, int vertCount)
{
	int _vc = vertCount;
	if (vertCount + 2 > 2725)
	{
		RB_DrawLines3D(vertCount / 2, 1, verts, depthTest);
		_vc = 0;
	}

	GfxPointVertex* vert = &verts[_vc];
	if (color) {
		R_ConvertColorToBytes(color, (GfxColor*)&vert->color);
	}

	verts[_vc + 1].color[0] = vert->color[0];
	verts[_vc + 1].color[1] = vert->color[1];
	verts[_vc + 1].color[2] = vert->color[2];
	verts[_vc + 1].color[3] = vert->color[3];

	VectorCopy(start, vert->xyz);

	vert = &verts[_vc + 1];
	VectorCopy(end, vert->xyz);

	return _vc + 2;

}
void R_ConvertColorToBytes(const float* in, GfxColor* out)
{
	((char(__fastcall*)(const float* in, GfxColor* out))0x493530)(in, out);

	return;
}
char RB_DrawLines3D(int count, int width, const GfxPointVertex* verts, char depthTest)
{
	((char(__cdecl*)(int, int, const GfxPointVertex*, char))0x613040)(count, width, verts, depthTest);
	return 1;

}
int RB_BeginSurface(MaterialTechniqueType tech, Material* material)
{
	int rval = 0;
	const static DWORD fnc = 0x61A220;
	__asm
	{
		mov edi, tech;
		mov esi, material;
		call fnc;
		mov rval, eax;
	}
	return rval;
}
void RB_EndTessSurface()
{
	((void(*)())0x61A2F0)();
}
bool RB_CheckTessOverflow(int vertexCount, int indexCount)
{
	if (vertexCount + tess->vertexCount > 5450 || indexCount + tess->indexCount > 0x100000)
		return true;

	return false;
}
void RB_BeginSurfaceInternal(bool two_sided, bool depthTest)
{
	static Material material;

	material = *rgp->whiteMaterial;

	static unsigned int loadBits[2] = { material.stateBitsTable->loadBits[0], material.stateBitsTable->loadBits[1] };
	static GfxStateBits bits = { .loadBits = { material.stateBitsTable->loadBits[0], material.stateBitsTable->loadBits[1] } };

	memcpy(material.stateBitsTable, rgp->whiteMaterial->stateBitsTable, sizeof(GfxStateBits));
	material.stateBitsTable = &bits;

	constexpr MaterialTechniqueType tech = MaterialTechniqueType::TECHNIQUE_UNLIT;
	static uint32_t ogBits = material.stateBitsTable->loadBits[1];

	if (tess->indexCount)
		RB_EndTessSurface();

	material.stateBitsTable->loadBits[1] = depthTest ? 44 : ogBits;
	material.stateBitsTable->loadBits[0] = two_sided ? 422072677 : 422089061;

	RB_BeginSurface(tech, &material);
}
void RB_TessOverflow(bool two_sided, bool depthTest)
{
	RB_EndTessSurface();
	RB_BeginSurfaceInternal(two_sided, depthTest);
}
void RB_SetPolyVertice(float* xyz, const GfxColor* color, const int vert, const int index, float* normal)
{
	VectorCopy(xyz, tess->verts[vert].xyzw);
	tess->verts[vert].color.packed = color->packed;

	switch (index)
	{
	case 0:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	case 1:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 2:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 3:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	default:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;
	}

	tess->verts[vert].normal.packed = normal ? Vec3PackUnitVec(normal) : 1073643391;
}
void R_AddDebugBox(const float* mins, const float* maxs, DebugGlobals* debugGlobalsEntry, float* color)
{
	__asm
	{
		push color;
		push debugGlobalsEntry;
		mov edx, maxs;
		mov eax, mins;
		mov esi, 0x60DC60;
		call esi;
		add esp, 0x8;
	}
}
void CM_DrawPoly(Poly* poly, const float* color)
{
	__asm
	{
		push color;
		push poly;
		mov esi, 0x597200;
		call esi;
		add esp, 0x8;
	}
}
GfxPointVertex verts2[2075];

constexpr std::array<std::int32_t, 24> iEdgePairs = 
{ 
  0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3,
  2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7 
};

constexpr auto iNextEdgePair = &iEdgePairs[1];
constexpr auto iota = std::views::iota;

void RB_DrawBoxEdges(const fvec3& mins, const fvec3& maxs, bool depthtest, const float* color)
{

	float v[8][3]{};
	for (const auto i : iota(0u, 8u)) {
		for (const auto j : iota(0u, 3u)){

			if ((i & (1 << j)) != 0)
				v[i][j] = maxs[j];
			else
				v[i][j] = mins[j];
		}
	}


	std::vector<fvec3> points;
	for (const auto i : iota(0u, 12u)) {
		points.emplace_back(v[iEdgePairs[i*2]]);
		points.emplace_back(v[iNextEdgePair[i*2]]);
	}

	auto vert_count = 0;
	for (const auto i : iota(0u, 12u)) {
		vert_count = RB_AddDebugLine(verts2, depthtest, points[i*2].As<float*>(), points[i*2+1].As<float*>(), color, vert_count);
	}

	RB_DrawLines3D(vert_count / 2, 1, verts2, depthtest);
}
void RB_DrawBoxPolygons(const fvec3& mins, const fvec3& maxs, bool depthtest, const float* color)
{
	float v[8][3]{};
	for (const auto i : iota(0u, 8u)) {
		for (const auto j : iota(0u, 3u)) {

			if ((i & (1 << j)) != 0)
				v[i][j] = maxs[j];
			else
				v[i][j] = mins[j];
		}
	}


	std::vector<fvec3> points;
	for (const auto i : iota(0u, 12u)) {
		points.emplace_back(v[iEdgePairs[i * 2]]);
		points.emplace_back(v[iNextEdgePair[i * 2]]);
	}

	std::vector<fvec3> points2;

	for (const auto i : iota(0u, 12u)) {
		points2.emplace_back(points[i * 2]);
		points2.emplace_back(points[i * 2 + 1]);

	}

	RB_DrawPolyInteriors(points2, color, true, depthtest);
}
GfxPointVertex poly_verts[2725];
void RB_DrawPoly(const std::vector<fvec3>& points, bool depthtest, float* color)
{

	std::int32_t vert_count = 0;
	size_t vert_index_prev = 0;

	for (auto i : std::views::iota(1u, points.size())) {
		vert_count = RB_AddDebugLine(poly_verts, depthtest, (float*)&points[i], (float*)&points[vert_index_prev], color, vert_count);
		vert_index_prev = i;
	}

	RB_DrawLines3D(vert_count / 2, 1, poly_verts, depthtest);
}
GfxPointVertex lineVerts[2];
void RB_DrawLine(const fvec3& start, const fvec3& end, bool depthtest, const float* color)
{

	const auto verts = RB_AddDebugLine(lineVerts, depthtest, start.As<float*>(), end.As<float*>(), color, 2);
	RB_DrawLines3D(verts / 2, 1, lineVerts, depthtest);
}

void RB_DrawLines(const std::vector<GfxPointVertex>& points, std::int32_t count, int width, bool depthTest)
{
	RB_DrawLines3D(count / 2, width, points.data(), depthTest);
}
void CL_AddDebugString(int fromServer, const fvec3& xyz, float* color, float scale, const char* text, int duration)
{
	static DWORD const addr = 0x462020;
	__asm
	{
		mov esi, fromServer;
		push duration;
		push text;
		push scale;
		push color;
		push xyz;
		call addr;
		add esp, 20;

	}
}
void CG_DebugLine(const fvec3& start, const fvec3& end, float* color, int depthTest, int duration)
{
	__asm
	{
		mov edi, start;
		push duration;
		push depthTest;
		push color;
		push end;
		mov esi, 0x4B6A50;
		call esi;
		add esp, 16;
	}
}
