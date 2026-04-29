#include "cm/cm_entity.hpp"
#include "cm/cm_brush.hpp"
#include "cm/cm_terrain.hpp"
#include "cm/cm_renderer.hpp"
#include "cm/cm_debug.hpp"

#include "g/g_spawn.hpp"

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"

#include "com/com_vector.hpp"
#include "com/com_channel.hpp"

#include "dvar/dvar.hpp"

#include "r/backend/rb_endscene.hpp"
#include "r/r_drawtools.hpp"
#include "r/r_drawactive.hpp"

#include "scr/scr_functions.hpp"

#include <cassert>
#include <ranges>
#include <array>
#include <string>
#include <vector>

using namespace std::string_literals;

std::unordered_map<gentity_s*, CStaticEntityFields::EntityKVs> CStaticEntityFields::m_oGentityFields;

void Cmd_ShowEntities_f()
{
	int num_args = cmd_args->argc[cmd_args->nesting];

	if (num_args == 1) {
		if (CGentities::Size() == 0) {
			return Com_Printf(CON_CHANNEL_CONSOLEONLY, "there are no entities to be cleared.. did you intend to use cm_showEntities <classname>?\n");
		}

		if (Dvar_FindMalleableVar("developer")->current.enabled)
			Com_Printf("clearing %i entities from the render queue\n", CGentities::Size());

		return CGentities::ClearThreadSafe();
	}

	std::string filter;
	for (int i = 1; i < num_args; i++) {
		filter += (*(cmd_args->argv[cmd_args->nesting] + i));
		filter += " ";
	}

	CGentities::CM_LoadAllEntitiesToClipMapWithFilter(filter);

	Com_Printf(CON_CHANNEL_CONSOLEONLY, "adding %i entities to the render queue\n", CGentities::Size());

}

void CGentities::CM_LoadAllEntitiesToClipMapWithFilter([[maybe_unused]]const std::string& filter)
{
	if (!Dvar_FindMalleableVar("sv_running")->current.enabled)
		return Com_Printf("^1Unsupported when sv_running is set to 0\n");

	ClearThreadSafe();
	std::unique_lock<std::mutex> lock(GetLock());

	const auto filters = CM_TokenizeFilters(filter);

	for (const auto i : std::views::iota(0, level->num_entities)) {

		auto gentity = &level->gentities[i];

		if(!CM_IsMatchingFilter(filters, Scr_GetString(gentity->classname)))
			continue;

		Insert(CGameEntity::CreateEntity(gentity));
	}

	ForEach([](GentityPtr_t& p){ 
		p->GenerateConnections(m_pLevelGentities); 
	});
}

CGameEntity::CGameEntity(gentity_s* const g) : 
	m_pOwner(g), 
	m_vecOrigin((fvec3&)g->r.currentOrigin),
	m_vecAngles((fvec3&)g->r.currentAngles)
{
	assert(m_pOwner != nullptr);

	ParseEntityFields();

}
CGameEntity::~CGameEntity()
{

}

void CGameEntity::ParseEntityFields()
{
	if (!CStaticEntityFields::m_oGentityFields.contains(m_pOwner)) {
		return;
	}

	m_oEntityFields = CStaticEntityFields::m_oGentityFields.at(m_pOwner);
}

std::unique_ptr<CGameEntity> CGameEntity::CreateEntity(gentity_s* const g)
{
	if (g->r.bmodel)
		return std::make_unique<CBrushModel>(g);

	return std::make_unique<CGameEntity>(g);
}

bool CGameEntity::IsBrushModel() const noexcept
{
	assert(m_pOwner != nullptr);
	return m_pOwner->r.bmodel;
}
static std::array<std::string, 6> nonVerboseInfoStrings = {
	"classname", "targetname", "spawnflags",
	"target", "script_noteworthy", "script_flag"
};
void CGameEntity::CG_Render2D(float drawDist, entity_info_type entType) const
{
	if (entType == entity_info_type::eit_disabled)
		return;

	const auto distance = m_vecOrigin.dist(cgs->predictedPlayerState.origin);

	if (distance > drawDist || m_oEntityFields.empty())
		return;


	fvec3 center = { 
		m_pOwner->r.currentOrigin[0], 
		m_pOwner->r.currentOrigin[1], 
		m_pOwner->r.currentOrigin[2] /*+ (m_pOwner->r.maxs[2] - m_pOwner->r.mins[2]) / 2 */
	};

	std::string buff;
	for (const auto& [key, value] : m_oEntityFields) {
		if (entType == entity_info_type::eit_enabled) {
			if (std::ranges::find(nonVerboseInfoStrings, key) == nonVerboseInfoStrings.end())
				continue;
		}

		buff += std::string(key) + " - " + value + "\n";
	}

	if (buff.empty())
		return;

	if (auto op = WorldToScreen(center)) {
		const float scale = R_ScaleByDistance(distance) * 0.15f;
		R_AddCmdDrawTextWithEffects(buff, "fonts/bigdevFont", *op, scale, 0.f, 5, vec4_t{1,1,1,1}, vec4_t{1,0,0,0});
	}

}

void CGameEntity::GenerateConnections(const LevelGentities_t& lgentities)
{
	if (!m_oEntityFields.contains("target"))
		return;

	const auto& target = m_oEntityFields["target"];

	for (const auto& gent : lgentities) {

		if (gent->m_pOwner == m_pOwner || !gent->m_oEntityFields.contains("targetname"))
			continue;

		const auto& targetname = gent->m_oEntityFields["targetname"];

		if (target == targetname)
			m_oGentityConnections.push_back({ gent->m_pOwner, m_pOwner->r.currentOrigin, gent->m_pOwner->r.currentOrigin });
	}

	if(!m_oGentityConnections.empty())
		m_oGentityConnectionVertices.resize(m_oGentityConnections.size() * 2);
}
void CGameEntity::RB_RenderConnections([[maybe_unused]]const cm_renderinfo& info, [[maybe_unused]] int& nverts) const
{
	if (m_oGentityConnections.empty())
		return;

	const vec4_t RED = { 1, 0, 0, info.alpha };

	for (const auto& connection : m_oGentityConnections) {
		nverts = RB_AddDebugLine(CGDebugData::g_debugPolyVerts, info.depth_test, connection.start.As<vec_t*>(), connection.end.As<vec_t*>(), RED, nverts);
	}
}

#include <iostream>
/***********************************************************************
 > BRUSHMODELS
***********************************************************************/

std::vector<std::uint16_t> GetLeafBrushNodes(cLeafBrushNode_s* nodes, [[maybe_unused]] std::int32_t nodeIndex, [[maybe_unused]] const fvec3& pos) {

	cLeafBrushNode_s* node = &nodes[nodeIndex];

	std::vector<std::uint16_t> resultNodes;

	if (node->leafBrushCount > 0) {
		for (int i = 0; i < node->leafBrushCount; i++) {
			unsigned short brushIndex = node->data.leaf.brushes[i];
			resultNodes.push_back(brushIndex);
		}
		return resultNodes;
	}

	return resultNodes;

	//idk...
	//const auto d = pos[node->axis] - node->data.children.dist;

	//std::int32_t side{};
	//if (d < -node->data.children.range)
	//	side = 0;
	//else if (d > node->data.children.range)
	//	side = 1;
	//else {
	//	const auto v1 = GetLeafBrushNodes(nodes, node->data.children.childOffset[0], pos);
	//	const auto v2 = GetLeafBrushNodes(nodes, node->data.children.childOffset[1], pos);

	//	resultNodes.insert(resultNodes.end(), v1.begin(), v1.end());
	//	resultNodes.insert(resultNodes.end(), v2.begin(), v2.end());
	//	return resultNodes;
	//}

	//return GetLeafBrushNodes(nodes, node->data.children.childOffset[side], pos);
}

CBrushModel::CBrushModel(gentity_s* const g) : CGameEntity(g) 
{
	assert(IsBrushModel());

	auto& cmodel = cm->cmodels[g->s.index.brushmodel];
	const auto leaf = &cmodel.leaf;

	//brush
	if (leaf->brushContents) {

		const auto indices = GetLeafBrushNodes(cm->leafbrushNodes, leaf->leafBrushNode, (fvec3(g->r.absmin) + fvec3(g->r.absmax)) * 0.5f);

		if (indices.size()) {
			for (const auto brushWorldIndex : indices) {
				m_oBrushModels.emplace_back(std::make_unique<CBrush>(g, brushWorldIndex));
			}
			return;
		}
	}

	//terrain
	cm_terrain terrain;
	if (terrain.CM_LeafToGeometry(leaf, { "all" })) {
		m_oBrushModels.emplace_back(std::make_unique<CTerrain>(g, leaf, terrain));
	}

}
CBrushModel::~CBrushModel() = default;

bool CBrushModel::RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const
{
	bool state = false;

	for (auto& bmodel : m_oBrushModels) {

		if (m_vecOrigin != m_vecOldOrigin || m_vecAngles != m_vecOldAngles)
			bmodel->OnPositionChanged(m_vecOrigin, m_vecAngles);

		if (bmodel->RB_MakeInteriorsRenderable(info))
			state = true;
	}

	m_vecOldOrigin = m_vecOrigin;
	m_vecOldAngles = m_vecAngles;

	return state;
}
bool CBrushModel::RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, int& nverts) const
{
	auto state = false;

	for (auto& bmodel : m_oBrushModels) {

		if (m_vecOrigin != m_vecOldOrigin || m_vecAngles != m_vecOldAngles)
			bmodel->OnPositionChanged(m_vecOrigin, m_vecAngles);

		if (bmodel->RB_MakeOutlinesRenderable(info, nverts))
			state = true;
	}

	m_vecOldOrigin = m_vecOrigin;
	m_vecOldAngles = m_vecAngles;

	return state;
}
int CBrushModel::GetNumVerts() const noexcept
{
	int count = 0;
	for (auto& bmodel : m_oBrushModels)
		count += bmodel->GetNumVerts();

	return count;
}

CBrushModel::CIndividualBrushModel::CIndividualBrushModel(gentity_s* const g) : m_pOwner(g) { assert(g != nullptr); }
CBrushModel::CIndividualBrushModel::~CIndividualBrushModel() = default;

bool CBrushModel::CIndividualBrushModel::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const {
	return GetSource().RB_MakeInteriorsRenderable(info);
}

bool CBrushModel::CIndividualBrushModel::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const {
	return GetSource().RB_MakeOutlinesRenderable(info, nverts);
}

fvec3 CBrushModel::CIndividualBrushModel::GetCenter() const noexcept
{
	assert(m_pOwner != nullptr);
	return m_pOwner->r.currentOrigin;
}

CBrushModel::CBrush::CBrush(gentity_s* const g, std::int32_t brushIdx) 
	: CIndividualBrushModel(g), m_uBrushIndex(brushIdx), m_pSourceBrush(&cm->brushes[brushIdx]) {
	assert(m_pSourceBrush != nullptr);

	const vec3_t col = { 1.f, 0.f, 0.f };

	//questionable for sure!
	m_oOriginalGeometry = *dynamic_cast<cm_brush*>(&*CM_GetBrushPoints(brushIdx, col));
	m_oOriginalGeometry.brushIndex = brushIdx;
	m_oCurrentGeometry = m_oOriginalGeometry;

	OnPositionChanged(g->r.currentOrigin, g->r.currentAngles);


}
CBrushModel::CBrush::~CBrush() = default;

void CBrushModel::CBrush::OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles)
{
	m_oCurrentGeometry = m_oOriginalGeometry;

	for (auto& winding : m_oCurrentGeometry.windings) {
		for (auto& point : winding.points) {
			point = VectorRotate(point + newOrigin, newAngles, m_oCurrentGeometry.origin);
		}
	}

	const auto center = GetCenter();

	for (auto& tri : m_oCurrentGeometry.triangles) {
		tri.a = VectorRotate(tri.a + newOrigin, newAngles, center);
		tri.b = VectorRotate(tri.b + newOrigin, newAngles, center);
		tri.c = VectorRotate(tri.c + newOrigin, newAngles, center);
	}

	m_oCurrentGeometry.mins = m_pOwner->r.absmin;
	m_oCurrentGeometry.maxs = m_pOwner->r.absmax;
	m_oCurrentGeometry.origin = m_pOwner->r.currentOrigin;
}

const cm_geometry& CBrushModel::CBrush::GetSource() const noexcept
{
	return m_oCurrentGeometry;
}

bool CBrushModel::CBrush::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(cgs->predictedPlayerState.origin) > info.draw_dist)
		return false;

	const auto center = GetCenter();

	if (BoundsInView(center + m_pSourceBrush->mins, center + m_pSourceBrush->maxs, info.frustum_planes, info.num_planes) == false)
		return false;

	return m_oCurrentGeometry.RB_MakeInteriorsRenderable(info);
}
bool CBrushModel::CBrush::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(cgs->predictedPlayerState.origin) > info.draw_dist)
		return false;

	const auto center = GetCenter();

	if (BoundsInView(center + m_pSourceBrush->mins, center + m_pSourceBrush->maxs, info.frustum_planes, info.num_planes) == false)
		return false;

	return m_oCurrentGeometry.RB_MakeOutlinesRenderable(info, nverts);
}

CBrushModel::CTerrain::CTerrain(gentity_s* const g, const cLeaf_t* const leaf) : CIndividualBrushModel(g), m_pSourceLeaf(leaf) {}
CBrushModel::CTerrain::CTerrain(gentity_s* const g, const cLeaf_t* const leaf, const cm_terrain& terrain) 
	: CIndividualBrushModel(g), m_pSourceLeaf(leaf), m_oOriginalGeometry(terrain), m_oCurrentGeometry(terrain) 
{
	OnPositionChanged(g->r.currentOrigin, g->r.currentAngles);
}

CBrushModel::CTerrain::~CTerrain() = default;

bool CBrushModel::CTerrain::RB_MakeInteriorsRenderable(const cm_renderinfo& info) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(cgs->predictedPlayerState.origin) > info.draw_dist)
		return false;

	const auto center = GetCenter();

	return m_oCurrentGeometry.RB_MakeInteriorsRenderable(info);
}
bool CBrushModel::CTerrain::RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const
{
	if (((fvec3&)m_pOwner->r.currentOrigin).dist(cgs->predictedPlayerState.origin) > info.draw_dist)
		return false;

	const auto center = GetCenter();

	return m_oCurrentGeometry.RB_MakeOutlinesRenderable(info, nverts);
}
void CBrushModel::CTerrain::OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles)
{
	m_oCurrentGeometry = m_oOriginalGeometry;

	const auto center = GetCenter();

	for (auto& tri : m_oCurrentGeometry.tris) {
		tri.a = VectorRotate(tri.a + newOrigin, newAngles, center);
		tri.b = VectorRotate(tri.b + newOrigin, newAngles, center);
		tri.c = VectorRotate(tri.c + newOrigin, newAngles, center);

	}

	m_oCurrentGeometry.mins = m_pOwner->r.absmin;
	m_oCurrentGeometry.maxs = m_pOwner->r.absmax;
	m_oCurrentGeometry.origin = m_pOwner->r.currentOrigin;

}
const cm_geometry& CBrushModel::CTerrain::GetSource() const noexcept
{
	return m_oCurrentGeometry;
}