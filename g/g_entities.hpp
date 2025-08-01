#pragma once
#include <cstdint>
#include <vector>

struct centity_s;
struct clientInfo_t;
struct snapshot_s;

template<typename T>
struct vec3;
using fvec3 = vec3<float>;

class CEntity
{
public:
	[[nodiscard]] static centity_s* GetEntity(std::uint32_t entityIndex);

	CEntity() = delete;
	CEntity(int pIndex);
	CEntity(centity_s* const cent);

	[[nodiscard]] bool IsValid() const noexcept;
	[[nodiscard]] bool IsEnemy() const noexcept;
	[[nodiscard]] bool IsAlive() const noexcept;
	[[nodiscard]] bool IsMe() const noexcept;
	[[nodiscard]] std::int32_t GetID() const noexcept;
	[[nodiscard]] std::int32_t GetHealth() const noexcept;
	[[nodiscard]] std::int32_t GetWeapon() const noexcept;
	[[nodiscard]] fvec3 GetAngles() const noexcept;
	[[nodiscard]] fvec3 GetOrigin() const noexcept;
	[[nodiscard]] char* GetName() const noexcept;

private:
	std::int32_t m_iClientNum{};
	centity_s* m_pCentity{};
	clientInfo_t* m_pInfo{};
};

[[nodiscard]] std::vector<CEntity> CG_GetAllEntities(snapshot_s* const snapshot);
