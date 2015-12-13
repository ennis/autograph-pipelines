#ifndef STREAM_HPP
#define STREAM_HPP

#include <memory>
#include <vector>

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
