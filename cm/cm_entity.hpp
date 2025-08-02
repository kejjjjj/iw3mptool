#pragma once

#include "cm_typedefs.hpp"
#include "utils/typedefs.hpp"

#include <memory>

struct gentity_s;
struct GfxPointVertex;

struct CGentityConnection
{
	gentity_s* const m_pConnection{};
	fvec3 start, end;
};

enum class entity_info_type
{
	eit_disabled,
	eit_enabled,
	eit_verbose
};

void Cmd_ShowEntities_f();

class CGameEntity
{
public:

	CGameEntity(gentity_s* const g);
	virtual ~CGameEntity();

	//creates the appropriate derived class
	[[nodiscard]] static std::unique_ptr<CGameEntity> CreateEntity(gentity_s* const g);

	[[nodiscard]] virtual bool RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const { return false; }
	[[nodiscard]] virtual bool RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, [[maybe_unused]] int& nverts) const { return false; }
	void RB_RenderConnections(const cm_renderinfo& info, int& nverts) const;

	virtual void CG_Render2D(float drawDist, entity_info_type entType) const;

	void GenerateConnections(const LevelGentities_t& gentities);

	[[nodiscard]] virtual int GetNumVerts() const noexcept { return 0; }
	[[nodiscard]] constexpr gentity_s* GetOwner() const noexcept { return m_pOwner; }
	[[nodiscard]] constexpr auto& GetFields() const noexcept { return m_oEntityFields; }

protected:


	[[nodiscard]] bool IsBrushModel() const noexcept;

	fvec3& m_vecOrigin;
	fvec3& m_vecAngles;

	mutable fvec3 m_vecOldOrigin;
	mutable fvec3 m_vecOldAngles;

private:
	void ParseEntityFields();

	gentity_s* const m_pOwner{};

	std::unordered_map<std::string, std::string> m_oEntityFields;
	std::vector<CGentityConnection> m_oGentityConnections;
	mutable std::vector<GfxPointVertex> m_oGentityConnectionVertices;
};

class CBrushModel : public CGameEntity
{
public:
	CBrushModel(gentity_s* const g);
	~CBrushModel();


	[[nodiscard]] bool RB_MakeInteriorsRenderable([[maybe_unused]] const cm_renderinfo& info) const override;
	[[nodiscard]] bool RB_MakeOutlinesRenderable([[maybe_unused]] const cm_renderinfo& info, int& nverts) const override;

	[[nodiscard]] int GetNumVerts() const noexcept override;

	struct CIndividualBrushModel
	{
		CIndividualBrushModel(gentity_s* const g);
		virtual ~CIndividualBrushModel();

		[[nodiscard]] virtual bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const;
		[[nodiscard]] virtual bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const;

		[[nodiscard]] virtual const cm_geometry& GetSource() const noexcept = 0;
		virtual void OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles) = 0;

		[[nodiscard]] virtual int GetNumVerts() const noexcept = 0;


	protected:
		[[nodiscard]] virtual fvec3 GetCenter() const noexcept;

	//private:
		gentity_s* const m_pOwner{};
	};

	struct CBrush : public CIndividualBrushModel
	{
		CBrush(gentity_s* const g, const cbrush_t* const brush);
		~CBrush();

		[[nodiscard]] bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const override;
		[[nodiscard]] bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const override;

		void OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles) override;
		[[nodiscard]] const cm_geometry& GetSource() const noexcept override;

		[[nodiscard]] int GetNumVerts() const noexcept override { return m_oCurrentGeometry.num_verts; }

	private:
		cm_brush m_oOriginalGeometry;
		cm_brush m_oCurrentGeometry;
		const cbrush_t* const m_pSourceBrush = {};
	};

	struct CTerrain : public CIndividualBrushModel
	{
		CTerrain(gentity_s* const g, const cLeaf_t* const leaf, const cm_terrain& terrain);
		CTerrain(gentity_s* const g, const cLeaf_t* const leaf);
		~CTerrain();

		[[nodiscard]] bool RB_MakeInteriorsRenderable(const cm_renderinfo& info) const override;
		[[nodiscard]] bool RB_MakeOutlinesRenderable(const cm_renderinfo& info, int& nverts) const override;

		void OnPositionChanged(const fvec3& newOrigin, const fvec3& newAngles) override;
		[[nodiscard]] const cm_geometry& GetSource() const noexcept override;

		[[nodiscard]] int GetNumVerts() const noexcept override { return m_oCurrentGeometry.num_verts; }

	private:
		cm_terrain m_oOriginalGeometry;
		cm_terrain m_oCurrentGeometry;
		const cLeaf_t* const m_pSourceLeaf = {};
	};

private:
	std::vector<std::unique_ptr<CIndividualBrushModel>> m_oBrushModels;
};


