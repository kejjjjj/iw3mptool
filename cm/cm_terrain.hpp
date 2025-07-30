#include <unordered_set>
#include <string>
#include <memory>

struct CollisionAabbTree;
struct cLeaf_t;

struct cm_terrain;
struct cm_geometry;
struct cm_triangle;

class CClipMap;

void CM_LoadAllTerrainToClipMapWithFilter(const std::string& filter);

[[nodiscard]] bool CM_AabbTreeHasCollision(const CollisionAabbTree* tree);
[[maybe_unused]] bool CM_DiscoverTerrain(const std::unordered_set<std::string>& filters);
[[nodiscard]] bool CM_TriangleInView(const cm_triangle* leaf, struct cplane_s* frustumPlanes, int numPlanes=5);
