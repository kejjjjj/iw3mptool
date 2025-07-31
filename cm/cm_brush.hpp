#pragma once

#include "utils/typedefs.hpp"

struct cm_geometry;
struct cbrush_t;
struct SimplePlaneIntersection;
struct adjacencyWinding_t;
struct cbrushside_t;
struct GfxViewParms;

void CM_ShowCollisionFilter();

[[nodiscard]] std::unique_ptr<cm_geometry> CM_GetBrushPoints(const cbrush_t* brush, const fvec3& poly_col);
[[nodiscard]] adjacencyWinding_t* BuildBrushdAdjacencyWindingForSide(int ptCount, SimplePlaneIntersection* pts, float* sideNormal, int planeIndex, adjacencyWinding_t* optionalOutWinding);
[[nodiscard]] int GetPlaneIntersections(const float** planes, int planeCount, SimplePlaneIntersection* OutPts);
[[nodiscard]] int BrushToPlanes(const cbrush_t* brush, float(*outPlanes)[4]);
[[nodiscard]] char* CM_MaterialForNormal(const cbrush_t* target, const fvec3& normals);
[[nodiscard]] bool CM_BrushHasCollision(const cbrush_t* brush);
[[nodiscard]] std::vector<std::string> CM_GetBrushMaterials(const cbrush_t* brush);
[[nodiscard]] bool CM_BrushInView(const cbrush_t* brush, struct cplane_s* frustumPlanes, int numPlanes = 5);
[[nodiscard]] bool CM_BoundsInView(const fvec3& mins, const fvec3& maxs, struct cplane_s* frustumPlanes, int numPlanes);
[[nodiscard]] std::vector<fvec3> CM_CreateHitbox(const fvec3& mins, const fvec3& maxs);

void CM_LoadAllBrushWindingsToClipMapWithFilter(const std::string& filter);
void CM_LoadBrushWindingsToClipMap(const cbrush_t* brush);
void CM_BuildAxialPlanes(float(*planes)[6][4], const cbrush_t* brush);
void CM_GetPlaneVec4Form(const cbrushside_t* sides, const float(*axialPlanes)[4], unsigned int index, float* expandedPlane);

namespace __brush
{
	void __asm_adjacency_winding();
}