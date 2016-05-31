#pragma once
#include <memory>

struct value_impl;
struct image_impl;
struct node;

struct image_node;
struct constant_node;

/*class scheduler
{
public:
	virtual void init_node(node& n) = 0;

	// immediate initialization for constant resource nodes
	virtual void init_image_node(image_node& img, std::unique_ptr<uint8_t[]> data) = 0;
	virtual void init_constant_node(constant_node& buf, std::unique_ptr<uint8_t[]> data) = 0;
};

extern scheduler* g_scheduler;*/
