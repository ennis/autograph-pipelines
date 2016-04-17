#pragma once
#include <functional>

struct value_impl;
struct node;

struct traversal_visitor
{
	virtual void visit_value(value_impl& val) {}
	virtual void visit_node(node& n) {}
};
