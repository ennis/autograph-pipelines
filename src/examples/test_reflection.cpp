#include <iostream>
#include "test_reflection_include.hpp"

namespace N
{
	namespace {
		void function_in_anon_namespace()
		{}

		int variable_in_anon_namespace = 0;
	}

	void func()
	{}

	void func(int i)
	{}

	void func2();

	void func2() {
		int innerdecl;
	}
	inline const bool inline_func() { return true; }

	class Opaque;

	class Opaque {
		int a;
	};

	namespace M {
		void func(int i)
		{}
	}

	struct AA {
		int i;
		float j;
		void do_stuff()
		{}

		virtual void do_stuff_virtually(int)
		{}
	};

	struct BB : public AA {
		double d;
		void do_stuff_2()
		{}

		void do_stuff_virtually(int) override
		{
			int decl_insdide_function = 0;
		}
	};

	enum class E {
		A, B, C, D
	};

	enum class E2 {
		AA, BB, CC, DD
	};
}

int main()
{
return 0;	
}