#pragma once
#include <movutl/core/vector.hpp>
#include <vector>

namespace mu::experimental {

template <typename T> void save_array(const std::vector<T>& data, const std::string& fname);

template <typename T> void save_array(const core::Vec<T>& data, const std::string& fname);

template <typename T> core::Vec<T> load_array(const std::string& fname);

template <typename T> std::vector<T> load_array_std(const std::string& fname);

} // namespace mu::experimental
