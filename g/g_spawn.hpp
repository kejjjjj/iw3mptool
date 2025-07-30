#pragma once

void G_InitGame();

void G_SpawnEntitiesFromString();
void G_ResetEntityParsePoint();

struct SpawnVar* G_GetGentitySpawnVars(const struct gentity_s* gent);

