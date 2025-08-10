#include "cm_model.hpp"
#include "cm_typedefs.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "com/com_vector.hpp"

#include <iomanip>
#include <format>
#include <ranges>
#include <iostream>
#include <com/com_channel.hpp>

void hmm_f()
{
	const auto num_args = cmd_args->argc[cmd_args->nesting];

	if (num_args == 1) {
		if (CClipMap::Size() == 0)
			Com_Printf(CON_CHANNEL_CONSOLEONLY, "there is no geometry to be cleared.. did you intend to use xm_load <name>?\n");
		CClipMap::ClearThreadSafe();
		return;
	}

	CClipMap::ClearThreadSafe();

	std::string filter;

	for (int i = 1; i < num_args; i++) {
		filter += *(cmd_args->argv[cmd_args->nesting] + i);
		filter += " ";
	}

	CClipMap::ClearThreadSafe();

	std::unique_lock<std::mutex> lock(CClipMap::GetLock());

	CM_LoadAllModelsToClipMap(filter);

}

void CM_LoadAllModelsToClipMap(const std::string& filter)
{
	const auto filters = CM_TokenizeFilters(filter);

	if (filters.empty())
		return;

	for (const auto i : std::views::iota(0u, gfxWorld->dpvs.smodelCount)) {

		if (!CM_IsMatchingFilter(filters, gfxWorld->dpvs.smodelDrawInsts[i].model->name))
			continue;

		CM_AddModel(&gfxWorld->dpvs.smodelDrawInsts[i]);
	}
}

void XSurfaceGetVerts(XSurface* surf, float* pVert)
{
	__asm
	{
		mov ecx, pVert;
		mov eax, surf;
		mov esi, 0x62D4B0;
		call esi;
	}
}
[[nodiscard]] static std::vector<cm_triangle> CM_GetModelTriangles(const GfxStaticModelDrawInst* model, const float* color)
{
	std::vector<cm_triangle> tris;
	auto xmodel = model->model;

	if (!xmodel)
		return {};

	auto surf = xmodel->surfs;

	if (!surf)
		return {};

	auto firstTri = surf->baseTriIndex;
	const fvec3 origin = model->placement.origin;
	const fvec3 angles = AxisToAngles(model->placement.axis);
	const auto scale = model->placement.scale;

	if (firstTri < firstTri + surf->triCount) {
		auto triIndice = 3 * firstTri;

		do {
			cm_triangle tri;

			tri.a = fvec3(surf->verts0[surf->triIndices[triIndice]].xyz) * scale;
			tri.b = fvec3(surf->verts0[surf->triIndices[triIndice + 1]].xyz) * scale;
			tri.c = fvec3(surf->verts0[surf->triIndices[triIndice + 2]].xyz) * scale;

			tri.a = VectorRotate(origin + tri.a, angles, origin);
			tri.b = VectorRotate(origin + tri.b, angles, origin);
			tri.c = VectorRotate(origin + tri.c, angles, origin);

			PlaneFromPointsASM(tri.plane, tri.a, tri.b, tri.c);

			tri.color[0] = color[0];
			tri.color[1] = color[1];
			tri.color[2] = color[2];
			tri.color[3] = 0.3f;

			tris.emplace_back(tri);

			++firstTri;
			triIndice += 3;

		} while (firstTri < surf->baseTriIndex + surf->triCount);

	}

	return tris;
}

cm_model CM_MakeModel(const GfxStaticModelDrawInst* model)
{
	return cm_model
	(
		model->model->name,
		model->placement.origin,
		AxisToAngles(model->placement.axis),
		model->placement.scale,
		CM_GetModelTriangles(model, vec4_t{ 1, 0, 0, 1 })
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