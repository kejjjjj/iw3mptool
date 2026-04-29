#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cm_brush.hpp"
#include "cm_typedefs.hpp"
#include "cm_export.hpp"
#include "com/com_channel.hpp"
#include "com/com_error.hpp"
#include "fs/fs_globals.hpp"
#include "fs/fs_io.hpp"
#include "cm_entity.hpp"
#include "scr/scr_functions.hpp"

#include <fstream>


static void CM_WriteHeader(std::stringstream& f)
{
	f << "iwmap 4\n";
	f << "\"000_Global\" flags  active\n";
	f << "\"The Map\" flags\n";
	f << "// entity 0\n{\n";
	f << "\"contrastGain\" " "\"0.125\"" << '\n';
	f << "\"diffusefraction\" " "\"0.5\"" << '\n';
	f << "\"_color\" " "\"0.2 0.27 0.3 1\"" << '\n';
	f << "\"sunlight\" " "\"1\"" << '\n';
	f << "\"sundirection\" " "\"-30 -95 0\"" << '\n';
	f << "\"sundiffusecolor\" " "\"0.2 0.27 0.3 1\"" << '\n';
	f << "\"suncolor\" " "\"0.2 0.27 0.3 1\"" << '\n';
	f << "\"ambient\" " "\".1\"" << '\n';
	f << "\"bouncefraction\" " "\".7\"" << '\n';
	f << "\"classname\" \"worldspawn\"\n";
}
#include <iostream>
static void CM_WriteAllBrushes(std::stringstream& o, const std::unordered_set<std::int32_t>& ignoreBrushes)
{

	if (CClipMap::Size() == 0) {
		return Com_FatalError("CClipMap::size() == 0");
	}

	auto brushIndex = 0;
	auto entity_start = false;

	CClipMap::ForEach([&](const GeometryPtr_t& geom) {

		if (geom->type() == cm_geomtype::model && !entity_start) {
			entity_start = true;
			brushIndex = 1;
			//end brushes, start entities
			o << "}\n";
		} else if (geom->type() == cm_geomtype::brush && ignoreBrushes.contains(geom->AsBrush()->brushIndex)) {
			return;
		}

		brushIndex = geom->map_export(o, brushIndex);
	});

	if (!entity_start) {
		o << "}\n";
		brushIndex = 1;
	}

	CGentities::ForEach([&](GentityPtr_t& ptr) {
		const auto g = ptr->GetOwner();

		if (!g->classname || ptr->GetFields().empty())
			return;

		if (const auto bm = dynamic_cast<CBrushModel*>(ptr.get())) {

			for (auto& ibm : bm->m_oBrushModels) {
				o << "// entity " << brushIndex++ << '\n';
				o << "{\n";

				for (const auto& [k, v] : ptr->GetFields()) {
					if (k == "model")
						continue;

					o << std::format("\"{}\" \"{}\"\n", k, v);
				}

				const_cast<cm_geometry&>(ibm->GetSource()).map_export(o, 0);

				o << "}\n";
			}
		} else {
			o << "// entity " << brushIndex++ << '\n';
			o << "{\n";

			for (const auto& [k, v] : ptr->GetFields()) {
				if (k == "model")
					continue;

				o << std::format("\"{}\" \"{}\"\n", k, v);
			}

			o << "}\n";
		}
	});

}

void CM_WriteInOrder(std::stringstream& o)
{
	CM_WriteHeader(o);


	std::unordered_set<std::int32_t> ignoreBrushes;

	CGentities::ForEach([&ignoreBrushes](GentityPtr_t& ptr) {
		if (const auto asBrushmodel = dynamic_cast<CBrushModel*>(ptr.get())) {
			for (const auto& m : asBrushmodel->m_oBrushModels) {
				if (m->GetSource().type() == cm_geomtype::brush) {
					ignoreBrushes.insert(dynamic_cast<CBrushModel::CBrush*>(m.get())->m_uBrushIndex);
				}
			}
		}
	});

	std::unique_lock<std::mutex> lock(CClipMap::GetLock());
	CM_WriteAllBrushes(o, ignoreBrushes);

}

void CM_MapExport()
{
	if (!cm->name)
		return;

	CM_LoadMap();
	CM_MapExport_Selected();
}

void CM_MapExport_Selected()
{

	if (!cm->name)
		return;

	std::string name = cm->name;
	name = name.substr(0u, name.length() - sizeof(".d3dbsp") + 1u);
	name = name.substr(sizeof("maps/mp"));

	const std::string path = "map_source\\kej\\";
	const std::string full_path = path + name + ".map";

	const auto writer = IOWriter(__fs::exe_path() + "\\" + full_path, false);
	std::stringstream map;

	CM_WriteInOrder(map);

	if (writer.IO_Write(map.str())) {
		const std::string str = std::format("^1{} export courtesy of the great\n^2xkejj^1.\n", name);
		Com_Printf(str.c_str());
	} else {
		Com_Printf("^1Failed!\n");
	}


}