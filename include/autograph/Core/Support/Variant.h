#pragma once
#include <variant>

namespace ag {
using std::variant;
using std::visit;
using std::holds_alternative;
using std::get_if;
using std::monostate;
using std::bad_variant_access;
using std::variant_size;
using std::variant_size_v;
using std::variant_alternative;
using std::variant_alternative_t;
using std::in_place_type_t;
using std::in_place_type;
using std::in_place_index_t;
using std::in_place_index;
}