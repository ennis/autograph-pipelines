#ifndef STREAM_HPP
#define STREAM_HPP

#include <memory>
#include <vector>
#include <tuple>

namespace ag
{
	template <typename D>
	struct StreamNodeBase
	{
		virtual void execute(D& backend) = 0;

		typename D::NodeDetail nodeDetail;
		int topological_index = 0;
		std::vector<StreamNodeBase<D>*> successors;
	};

	template <
		typename T,
		typename D
	>
	struct StreamNode : public StreamNodeBase<D>
	{
		typename D::template StreamDetail<T> detail;
	};

	template <
		typename T,
		typename D
	>
	struct ConstNode : public StreamNode<T,D>
	{
		ConstNode(D& backend, T&& value_) : 
			impl(backend, std::forward<T>(value_))
		{
			backend.initialize(*this);
		}

		void execute(D& backend) override
		{
			backend.execute(*this);
		}

		typename D::template ConstNodeImpl<T> impl;
	};
	
	template <
		typename T,
		typename D
	>
	struct VarNode : public StreamNode<T, D>
	{
		VarNode(D& backend, T&& init_value_) : impl(backend, std::forward<T>(init_value_))
		{
			backend.initialize(*this);
		}

		void set(T new_value)
		{
			impl.set(new_value);
		}

		void execute(D& backend) override
		{
			backend.execute(*this);
		}

		const T& get()
		{
			return impl.get();
		}

		typename D::template VarNodeImpl<T> impl;
	};


	/*template <
		typename T,
		typename D
	>
	struct Stream
	{
		// TODO replace this with a raw pointer, with the nodes allocated in a pool
		std::shared_ptr<StreamNode<T, D> > ptr;
	};*/

	/////////////////////////// Operation node
	template <
		typename T,
		typename D,
		typename F,
		typename ... TDeps
	>
	struct OpNode : public StreamNode<T, D>
	{
		OpNode(D& backend, F&& f, StreamNode<TDeps, D>&... deps) :
			func(std::forward<F>(f)),
			inputs(deps...)
		{
			backend.initialize(*this);
		}

		void execute(D& backend) override
		{
			// call detail::fetch on each input
			//util::call(func, )
			backend.execute(*this);
		}

		F func;
		std::tuple<StreamNode<TDeps, D>&...> inputs;
	};

	
}


#endif
