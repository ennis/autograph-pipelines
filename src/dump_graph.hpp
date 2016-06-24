#pragma once
#include <ostream>
#include "node.hpp"
#include "value.hpp"

void dump_graph_dot(std::vector<node*> roots, std::ostream& out);