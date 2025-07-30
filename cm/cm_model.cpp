#include "cm_model.hpp"
#include "cm_typedefs.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "com/com_vector.hpp"

#include <iomanip>
#include <format>

cm_model CM_MakeModel(const GfxStaticModelDrawInst* model)
{
	return cm_model
	(
		model->model->name,
		model->placement.origin,
		AxisToAngles(model->placement.axis),
		model->placement.scale
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