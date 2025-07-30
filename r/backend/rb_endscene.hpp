#pragma once

#include <vector>

template<typename T>
struct vec3;

using fvec3 = vec3<float>;

struct GfxPointVertex;
enum class MaterialTechniqueType : int;

int RB_BeginSurface(MaterialTechniqueType tech, struct Material* material);
void RB_EndTessSurface();
bool RB_CheckTessOverflow(int vertexCount, int indexCount);
void RB_BeginSurfaceInternal(bool two_sided, bool depthTest);
void RB_TessOverflow(bool two_sided, bool depthTest);


void RB_SetPolyVertice(float* xyz, const union GfxColor* color, const int vert, const int index, float* normal);
void RB_DrawPolyInteriors(const std::vector<fvec3>& points, const float* color, bool two_sided = false, bool depthTest = false);
char RB_DrawLines3D(int count, int width, const GfxPointVertex* verts, char depthTest);
int RB_AddDebugLine(GfxPointVertex* verts, char depthTest, const float* start, const float* end, const float* color, int vertCount);
void RB_DrawBoxEdges(const fvec3& mins, const fvec3& maxs, bool depthtest, const float* color);
void RB_DrawBoxPolygons(const fvec3& mins, const fvec3& maxs, bool depthtest, const float* color);

void RB_DrawLines(const std::vector<GfxPointVertex>& points, std::int32_t count, int width, bool depthTest);

void RB_DrawLine(const fvec3& start, const fvec3& end, bool depthtest, const float* color);

void R_ConvertColorToBytes(const float* in, union GfxColor* out);
void R_AddDebugBox(const float* mins, const float* maxs, struct DebugGlobals* debugGlobaksEntry, float* color);

void CM_DrawPoly(struct Poly* poly, const float* color);
void RB_DrawPoly(const std::vector<fvec3>& points, bool depthtest, float* color);
void CL_AddDebugString(int fromServer, const fvec3& xyz, float* color, float scale, const char* text, int duration);

void CG_DebugLine(const fvec3& start, const fvec3& end, float* color, int depthTest, int duration);
