#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cm_brush.hpp"
#include "cm_typedefs.hpp"
#include "com/com_channel.hpp"
#include "com/com_error.hpp"
#include "fs/fs_globals.hpp"
#include "fs/fs_io.hpp"

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
static void CM_WriteAllBrushes(std::stringstream& o)
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
		}

		brushIndex = geom->map_export(o, brushIndex);
	});



	if (!entity_start) {
		o << "}\n";
	}

}

void CM_WriteInOrder(std::stringstream& o)
{
	CM_WriteHeader(o);

	std::unique_lock<std::mutex> lock(CClipMap::GetLock());
	CM_WriteAllBrushes(o);



}

void CM_MapExport()
{

	if (!cm->name)
		return;

	CM_LoadMap();

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
	}
	else {
		Com_Printf("^1Failed!\n");
	}


}