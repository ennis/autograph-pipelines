#include "node.hpp"
#include "value.hpp"

size_t node::global_node_uid = 0;

void node::add_dependency(value_impl & v)
{
	v.add_successor(shared_from_this());
}
