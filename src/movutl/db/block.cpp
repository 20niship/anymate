#include <movutl/db/block.hpp>
#include <movutl/instance/instance.hpp>

namespace mu::db {
void Block::name(const std::string_view name) { name.copy(m_name, std::min<size_t>(name.size(), ENTITY_NAME_MAX_LENGTH - 1)); }

} // namespace mu::db
