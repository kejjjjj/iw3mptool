#pragma once

#include <vector>

template<typename T>
struct vec3;

using fvec3 = vec3<float>;
using ivec3 = vec3<int>;

struct GfxViewParms;

void RB_DrawDebug(GfxViewParms* viewParms);

void CM_MakeInteriorRenderable(const std::vector<fvec3>& points, const float* color);
[[nodiscard]] int CM_MakeOutlinesRenderable(const std::vector<fvec3>& points, const float* color, bool depthTest, int nverts);

void CM_DrawCollisionPoly(const std::vector<fvec3>& points, const float* colorFloat, bool depthtest);
void CM_DrawCollisionEdges(const std::vector<fvec3>& points, const float* colorFloat, bool depthtest);
void CM_ShowCollision(GfxViewParms* viewParms);

void CM_DrawBrushBounds(const cbrush_t* brush, bool depthTest);
