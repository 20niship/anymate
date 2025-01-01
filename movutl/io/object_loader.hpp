#pragma once 

#include <movutl/db/body.hpp>
#include <movutl/db/project.hpp>

namespace mu::io{

db::Body *impl_load_obj(const char *fname);
db::Body *impl_load_ply(const char *fname);
db::Body *impl_load_ply_happly(const char *fname);
db::Body *impl_load_pcd(const char *fname);
db::Body *impl_load_stl(const char *fname);

db::Body *load_object(const char *fname);

}
