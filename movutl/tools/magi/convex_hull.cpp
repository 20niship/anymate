#include <movutl/core/assert.hpp>
#include <movutl/tools/magi/convex_hull.hpp>
#include <opencv2/core/persistence.hpp>
#include <movutl/core/logger.hpp>

namespace mu::Magi {

void convex_hull([[maybe_unused]]const db::MeshCol* min, [[maybe_unused]]const db::Mesh* mout) {
  MU_ASSERT(min);
  MU_ASSERT(mout);
  LOGE << "not implemented!!";
  //TODO!!
}


} // namespace mu::Magi
