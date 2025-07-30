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


using namespace std::string_literals;


void CGentities::CM_LoadAllEntitiesToClipMapWithFilter([[maybe_unused]]const std::string& filter)
{
	if (!Dvar_FindMalleableVar("sv_running")->current.enabled)
		return Com_Printf("^1Unsupported when sv_running is set to 0\n");

	ClearThreadSafe();
	std::unique_lock<std::mutex> lock(GetLock());

	const auto filters = CM_TokenizeFilters(filter);

	//reset spawnvars
	G_ResetEntityParsePoint();

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
	const auto spawnVar = G_GetGentitySpawnVars(m_pOwner);

	if (!spawnVar)
		return;

	for (const auto index : std::views::iota(0, spawnVar->numSpawnVars)) {
		const auto [key, value] = std::tie(spawnVar->spawnVars[index][0], spawnVar->spawnVars[index][1]);

		for (auto f = ent_fields; f->name; ++f) {
			if (!strcmp(f->name, key)) {
				m_oEntityFields[key] = value;
			}
		}
	}

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

void CGameEntity::CG_Render2D(float drawDist) const
{
	const auto distance = m_vecOrigin.dist(cgs->predictedPlayerState.origin);

	if (distance > drawDist || m_oEntityFields.empty())
		return;


	fvec3 center = { 
		m_pOwner->r.currentOrigin[0], 
		m_pOwner->r.currentOrigin[1], 
		m_pOwner->r.currentOrigin[2] + (m_pOwner->r.maxs[2] - m_pOwner->r.mins[2]) / 2 
	};

	std::stringstream buff;
	for (const auto& [k, v] : m_oEntityFields) {
		if (k == "model") //useless
			continue;

		buff << k << " - " << v << '\n';
	}

	if (auto op = WorldToScreen(center)) {
		const float scale = R_ScaleByDistance(distance) * 0.15f;
		R_AddCmdDrawTextWithEffects(buff.str(), "fonts/bigdevFont", *op, scale, 0.f, 5, vec4_t{1,1,1,1}, vec4_t{1,0,0,0});
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

/***********************************************************************
 > BRUSHMODELS
***********************************************************************/
CBrushModel::CBrushModel(gentity_s* const g) : CGameEntity(g) 
{
	assert(IsBrushModel());

	const auto leaf = &cm->cmodels[g->s.index.brushmodel].leaf;
	const auto& leafBrushNode = cm->leafbrushNodes[leaf->leafBrushNode];
	const auto numBrushes = leafBrushNode.leafBrushCount;


	//brush
	if (numBrushes > 0) {
		for (const auto brushIndex : std::views::iota(0, numBrushes)) {
			const auto brushWorldIndex = leafBrushNode.data.leaf.brushes[brushIndex];
			if (brushWorldIndex > cm->numBrushes)
				break;

			m_oBrushModels.emplace_back(std::make_unique<CBrush>(g, &cm->brushes[brushWorldIndex]));
		}
		return;
	}

	//terrain
	cm_terrain terrain;
	if (terrain.CM_LeafToGeometry(leaf, { "all " })) {
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

CBrushModel::CBrush::CBrush(gentity_s* const g, const cbrush_t* const brush) : CIndividualBrushModel(g), m_pSourceBrush(brush)
{
	assert(m_pSourceBrush != nullptr);

	const vec3_t CYAN = { 0.f, 1.f, 1.f };

	//questionable for sure!
	m_oOriginalGeometry = *dynamic_cast<cm_brush*>(&*CM_GetBrushPoints(m_pSourceBrush, CYAN));
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