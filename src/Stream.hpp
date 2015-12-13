#ifndef STREAM_HPP
#define STREAM_HPP

#include <memory>
#include <vector>
#include <tuple>

namespace ag
{
	struct StreamNodeBase
	{
	};

	template <
		typename T,
		typename D
	>
	struct StreamNode : public StreamNodeBase
	{
		std::vector<StreamNodeBase*> successors;
		typename D::template StreamDetail<T> detail;
	};

	template <
		typename T,
		typename D
	>
	struct ConstNode : public StreamNode<T,D>
	{
		ConstNode(T&& value_) : value(std::forward<T>(value_))
		{}
		T value;
	};


	template <
		typename T,
		typename D
	>
	struct Stream
	{
		// TODO replace this with a raw pointer, with the nodes allocated in a pool
		std::shared_ptr<StreamNode<T, D> > ptr;
	};

	/////////////////////////// Operation node
	template <
		typename T,
		typename D,
		typename F,
		typename ... TDeps
	>
	struct OpNode : public StreamNode<T, D>
	{
		OpNode(F&& f, Stream<TDeps, D>... deps) :
			func(std::forward<F>(f)),
			inputs(deps...)
		{
		}

		void execute()
		{
			// call detail::fetch on each input
			//util::call(func, )
		}

		F func;
		std::tuple<Stream<TDeps, D>...> inputs;
	};

	template <typename T>
	struct is_stream_type : public std::false_type
	{};

	template <
		typename T,
		typename D
	>
	struct is_stream_type<Stream<T, D> > : public std::true_type
	{};

	template <
		typename T,
		typename D,
		typename TDep
	>
		void add_dependency(StreamNode<T, D>& node, TDep& dep)
	{
		//static_assert(is_stream_type<TDep>::value, "Non-stream parameter");
		dep.ptr->successors.push_back(&node);
	}

	template <
		typename T,
		typename D,
		typename TDep,
		typename ... TDeps
	>
	void add_dependency(StreamNode<T, D>& node, TDep& dep, TDeps&... deps)
	{
		//static_assert(is_stream_type<TDep>::value, "Non-stream parameter");
		dep.ptr->successors.push_back(&node);
		add_dependency<T, D, TDeps...>(node, deps...);
	}

	template <
		typename D,
		typename F,
		typename ... TDeps,
		typename T = std::result_of_t<F(TDeps...)>
	>
	auto apply(F&& f, Stream<TDeps, D>... deps) -> Stream<T, D>
	{
		// copy deps
		auto ptr = std::make_shared<OpNode<T, D, F, TDeps...> >(std::forward<F>(f), deps...);
		add_dependency(*ptr, deps...);
		return Stream<T, D> {std::move(ptr)};
	}

	template <
		typename T,
		typename D
	> 
	auto constant(D& ctx, T&& value) 
		-> Stream<T, D>
	{
		return Stream<T,D> {std::make_shared<ConstNode<T, D> >(std::forward<T>(value))};
	}
}


#endif
