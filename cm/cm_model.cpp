#include "cm_model.hpp"
#include "cm_typedefs.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "com/com_vector.hpp"
#include "com/com_channel.hpp"

#include <iomanip>
#include <format>
#include <ranges>
#include <iostream>
#include <array>

void hmm_f()
{
	const auto num_args = cmd_args->argc[cmd_args->nesting];

	if (num_args == 1) {
		if (CClipMap::Size() == 0)
			Com_Printf(CON_CHANNEL_CONSOLEONLY, "there is no geometry to be cleared.. did you intend to use xm_load <name>?\n");
		CClipMap::ClearAllOfTypeThreadSafe(cm_geomtype::model);
		return;
	}

	CClipMap::ClearAllOfTypeThreadSafe(cm_geomtype::model);

	std::string filter;

	for (int i = 1; i < num_args; i++) {
		filter += *(cmd_args->argv[cmd_args->nesting] + i);
		filter += " ";
	}

	std::unique_lock<std::mutex> lock(CClipMap::GetLock());

	CM_LoadAllModelsToClipMap(filter);

}

void CM_LoadAllModelsToClipMap(const std::string& filter)
{
	const auto filters = CM_TokenizeFilters(filter);

	if (filters.empty())
		return;

	for (const auto i : std::views::iota(0u, gfxWorld->dpvs.smodelCount)) {
	//for (const auto i : std::views::iota(0u, 1u)) {

		if (!CM_IsMatchingFilter(filters, gfxWorld->dpvs.smodelDrawInsts[i].model->name))
			continue;

		CM_AddModel(&gfxWorld->dpvs.smodelDrawInsts[i]);
	}
}

fvec3 solve_3x3_system(const std::array<std::array<float, 3>, 3>& A, const std::array<float, 3>& b) {
	float det_A = A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) -
		A[0][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0]) +
		A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);

	if (std::abs(det_A) < 1e-6f) {
		throw std::runtime_error("Matrix is singular or nearly singular");
	}

	fvec3 x;
	for (int i = 0; i < 3; ++i) {
		std::array<std::array<float, 3>, 3> A_i = A;
		for (int j = 0; j < 3; ++j) {
			A_i[j][i] = b[j];
		}
		float det_A_i = A_i[0][0] * (A_i[1][1] * A_i[2][2] - A_i[1][2] * A_i[2][1]) -
			A_i[0][1] * (A_i[1][0] * A_i[2][2] - A_i[1][2] * A_i[2][0]) +
			A_i[0][2] * (A_i[1][0] * A_i[2][1] - A_i[1][1] * A_i[2][0]);
		x[i] = det_A_i / det_A;
	}
	return x;
}

std::array<fvec3, 3> reconstruct_triangle_vertices(const XModelCollTri_s& tri) {
	std::array<std::array<float, 3>, 3> A = { {
		{tri.plane[0], tri.plane[1], tri.plane[2]},
		{tri.svec[0], tri.svec[1], tri.svec[2]},
		{tri.tvec[0], tri.tvec[1], tri.tvec[2]}
	} };

	std::array<fvec3, 3> vertices;

	vertices[0] = solve_3x3_system(A, { tri.plane[3], tri.svec[3], tri.tvec[3] });
	vertices[1] = solve_3x3_system(A, { tri.plane[3], tri.svec[3] + 1.0f, tri.tvec[3] });
	vertices[2] = solve_3x3_system(A, { tri.plane[3], tri.svec[3], tri.tvec[3] + 1.0f });

	return vertices;
}

[[nodiscard]] static std::vector<cm_triangle> CM_GetModelTriangles2(const GfxStaticModelDrawInst* model, const float* color)
{
	std::vector<cm_triangle> tris;
	
	if (!model || !model->model)
		return {};

	const fvec3 origin = model->placement.origin;
	const fvec3 angles = AxisToAngles(model->placement.axis);
	const auto scale = model->placement.scale;

	for (const auto i : std::views::iota(0, model->model->numCollSurfs)) {

		const auto collSurf = &model->model->collSurfs[i];

		for (const auto j : std::views::iota(0, collSurf->numCollTris)) {	

			const auto pts = reconstruct_triangle_vertices(collSurf->collTris[j]);
			cm_triangle tri{ pts[0] * scale, pts[1] * scale, pts[2] * scale };

			tri.a = VectorRotate(origin + tri.a, angles, origin);
			tri.b = VectorRotate(origin + tri.b, angles, origin);
			tri.c = VectorRotate(origin + tri.c, angles, origin);

			PlaneFromPointsASM(tri.plane, tri.a, tri.b, tri.c);

			tri.color[0] = color[0];
			tri.color[1] = color[1];
			tri.color[2] = color[2];
			tri.color[3] = 0.3f;

			tris.emplace_back(tri);
		}

	}
	return tris;
}

//static void XSurfaceGetVerts(XSurface* surf, float* pVert)
//{
//	__asm
//	{
//		mov ecx, pVert;
//		mov eax, surf;
//		mov esi, 0x62D4B0;
//		call esi;
//	}
//}
//[[nodiscard]] static std::vector<cm_triangle> CM_GetModelTriangles(const GfxStaticModelDrawInst* model, const float* color)
//{
//	std::vector<cm_triangle> tris;
//	auto xmodel = model->model;
//
//	if (!xmodel)
//		return {};
//
//	auto surf = xmodel->surfs;
//
//	if (!surf)
//		return {};
//
//	auto firstTri = surf->baseTriIndex;
//	const fvec3 origin = model->placement.origin;
//	const fvec3 angles = AxisToAngles(model->placement.axis);
//	const auto scale = model->placement.scale;
//
//	if (firstTri < firstTri + surf->triCount) {
//		auto triIndice = 3 * firstTri;
//
//		do {
//			cm_triangle tri;
//
//			tri.a = fvec3(surf->verts0[surf->triIndices[triIndice]].xyz) * scale;
//			tri.b = fvec3(surf->verts0[surf->triIndices[triIndice + 1]].xyz) * scale;
//			tri.c = fvec3(surf->verts0[surf->triIndices[triIndice + 2]].xyz) * scale;
//
//			tri.a = VectorRotate(origin + tri.a, angles, origin);
//			tri.b = VectorRotate(origin + tri.b, angles, origin);
//			tri.c = VectorRotate(origin + tri.c, angles, origin);
//
//			PlaneFromPointsASM(tri.plane, tri.a, tri.b, tri.c);
//
//			tri.color[0] = color[0];
//			tri.color[1] = color[1];
//			tri.color[2] = color[2];
//			tri.color[3] = 0.3f;
//
//			tris.emplace_back(tri);
//
//			++firstTri;
//			triIndice += 3;
//
//		} while (firstTri < surf->baseTriIndex + surf->triCount);
//
//	}
//
//	return tris;
//}

cm_model CM_MakeModel(const GfxStaticModelDrawInst* model)
{
	return cm_model
	(
		model->model->name,
		model->placement.origin,
		AxisToAngles(model->placement.axis),
		model->placement.scale,
		CM_GetModelTriangles2(model, vec4_t{ 1, 0, 0, 1 })
	);
}

void CM_AddModel(const GfxStaticModelDrawInst* model) {
	CClipMap::Insert(std::make_unique<cm_model>(CM_MakeModel(model)));
}
int cm_model::map_export(std::stringstream& o, int index)
{

	o << "// entity " << index << '\n';
	o << "{\n";
	o << std::quoted("angles") << ' ' << std::quoted(std::format("{} {} {}", angles.x, angles.y, angles.z)) << '\n';
	o << std::quoted("modelscale") << ' ' << std::quoted(std::format("{}", modelscale)) << '\n';
	o << std::quoted("origin") << ' ' << std::quoted(std::format("{} {} {}", origin.x, origin.y, origin.z)) << '\n';
	o << std::quoted("model") << ' ' << std::quoted(name) << '\n';
	o << std::quoted("classname") << ' ' << std::quoted("misc_model") << '\n';
	o << "}\n";

	return ++index;

}