#include "g_entities.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "utils/typedefs.hpp"
#include "cod4x/cod4x.hpp"

#include <ranges>

using namespace std::string_literals;

centity_s* CEntity::GetEntity(std::uint32_t entityIndex) {
	return &centity[entityIndex];
}

CEntity::CEntity(int _pIndex) :
	m_iClientNum(_pIndex),
	m_pCentity(&centity[_pIndex]),
	m_pInfo(&cinfo[_pIndex]) {
}
CEntity::CEntity(centity_s* _cent) :
	m_iClientNum(_cent->nextState.clientNum),
	m_pCentity(_cent),
	m_pInfo(&cinfo[m_iClientNum]) {
}

bool CEntity::IsValid() const noexcept
{
	if (!m_pCentity || !m_pInfo)
		return false;

	if (!m_pCentity->nextState.eventSequence)
		return false;

	//if (!m_pInfo->infoValid || !m_pInfo->nextValid)
	//	return false;

	return true;
}

bool CEntity::IsEnemy() const noexcept
{

	const bool teammate = cinfo[m_iClientNum].team == cinfo[cgs->clientNum].team;
	const bool IsFFA = "dm"s == cgs_s->gametype == 0;

	return !teammate || IsFFA;
}

std::int32_t CEntity::GetHealth() const noexcept
{
	return m_pInfo->health;
}

bool CEntity::IsAlive() const noexcept
{

	return m_pCentity->nextValid && m_pCentity->nextState.eType == ET_PLAYER;
}
bool CEntity::IsMe() const noexcept
{
	return m_iClientNum == cgs->clientNum;
}

fvec3 CEntity::GetAngles() const noexcept
{
	return m_pCentity->pose.angles;

}
fvec3 CEntity::GetOrigin() const noexcept
{
	return m_pCentity->pose.origin;

}
char* CEntity::GetName() const noexcept
{
	if (CoD4X::LegacyProtocol())
		return m_pInfo->name;

	auto offs = CoD4X::Get() + 0x443d200;
	offs += (m_iClientNum * 46);
	return reinterpret_cast<char*>(offs);
}
std::int32_t CEntity::GetWeapon() const noexcept
{
	return m_pCentity->nextState.weapon;
}
std::int32_t CEntity::GetID() const noexcept
{
	return m_iClientNum;
}

std::vector<CEntity> CG_GetAllEntities(snapshot_s* const snapshot)
{
	std::vector<CEntity> ents;
	ents.reserve(snapshot->numClients);

	for (const auto i : std::views::iota(0, snapshot->numClients)) {
		ents.emplace_back(CEntity(i));

		if (!ents.back().IsValid())
			ents.pop_back();
	}
	
	return ents;
}
