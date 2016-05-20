#pragma once
#include "element.hpp"

namespace ui
{
	class stack_layout 
	{
	public:
		stack_layout(const rect_2d& geometry) : geometry_{geometry}
		{}

		void arrange(std::vector<element*> elements)
		{

		}

	private:
		void arrange();
		rect_2d geometry_;
		struct layout_entry 
		{
			element* elem;
			rect_2d geom;
		};
		std::vector<element*> elements_;
	};

	void arrange_elements(const rect_2d& geometry, std::vector<element*> elements)
	{
		
	} 
}