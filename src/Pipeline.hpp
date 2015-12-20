#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <Stream.hpp>
#include <Graphics.hpp>
#include <vector>
#include <memory>
#include <set>

namespace ag
{
	// Topological sorting of StreamNodes
	template <
		typename D
	>
	struct StreamNodeCompare
	{
		bool operator()(StreamNodeBase<D>* a, StreamNodeBase<D>* b)
		{
			return a->topological_index < b->topological_index;
		}
	};

	template <
		typename D	// Backend type
	>
	class Pipeline
	{
	public:
		Pipeline(D& backend_) : backend(backend_), last_index(0)
		{}

		template <
			typename TNode,
			typename ... Args 
		>
		TNode& allocateNode(Args&&... args)
		{
			auto ptr = std::make_unique<TNode>(std::forward<Args>(args)...);
			auto raw_ptr = ptr.get();
			nodes.emplace_back(std::move(ptr));
			raw_ptr->topological_index = last_index++;
			return *raw_ptr;
		}

		using SchedulerSet = std::set<StreamNodeBase<D>*, StreamNodeCompare<D> >;

		void scheduleRec(StreamNodeBase<D>& node, SchedulerSet& sched)
		{
			sched.insert(&node);
			for (auto succ : node.successors)
			{
				scheduleRec(*succ, sched);
			}
		}

		void schedule(StreamNodeBase<D>& input)
		{
			SchedulerSet scheduled_nodes;
			scheduleRec(input, scheduled_nodes);

			for (auto node : scheduled_nodes)
			{
				node->execute(backend);
			}
		}

	//private:
		D& backend;
		int last_index;
		std::vector < std::unique_ptr<StreamNodeBase<D> > > nodes;
	};

	/////////////////////////////////////////// add_dependency<TDep>
	template <
		typename T,
		typename D,
		typename TDep
	>
	void add_dependency(StreamNode<T, D>& node, TDep& dep)
	{
		dep.successors.push_back(&node);
	}

	/////////////////////////////////////////// add_dependency<TDeps...>
	template <
		typename T,
		typename D,
		typename TDep,
		typename ... TDeps
	>
	void add_dependency(StreamNode<T, D>& node, TDep& dep, TDeps&... deps)
	{
		//static_assert(is_stream_type<TDep>::value, "Non-stream parameter");
		dep.successors.push_back(&node);
		add_dependency<T, D, TDeps...>(node, deps...);
	}

	/////////////////////////////////////////// apply
	template <
		typename D,
		typename F,
		typename ... TDeps,
		typename T = std::result_of_t<F(TDeps...)>
	>
	auto apply(Pipeline<D>& pipeline, F&& f, StreamNode<TDeps, D>&... deps) -> OpNode<T, D, F, TDeps...>&
	{
		// copy deps
		auto& node = pipeline. template allocateNode<OpNode<T, D, F, TDeps...> >(pipeline.backend, std::forward<F>(f), deps...);
		add_dependency(node, deps...);
		return node;
	}

	/////////////////////////////////////////// constant
	template <
		typename T,
		typename D
	>
	auto constant(Pipeline<D>& pipeline, T&& value)
		-> ConstNode<T, D>&
	{
		auto& node = pipeline. template allocateNode<ConstNode<T, D> >(
			pipeline.backend, 
			std::forward<T>(value));
		return node;
	}


	/////////////////////////////////////////// variable input node
	template <
		typename T,
		typename D
	>
	auto variable(
		Pipeline<D>& pipeline, 
		T&& init_value)
		-> VarNode<T, D>&
	{
		auto& node = pipeline. template allocateNode<VarNode<T, D> >(pipeline.backend, std::forward<T>(init_value));
		return node;
	}


	/////////////////////////////////////////// clearRT
	template <
		typename PixelType,
		typename D
	>
	auto clearRT(
		Pipeline<D>& pipeline,
		StreamNode<glm::uvec2, D>& size_,
		StreamNode<glm::vec4, D>& color_)
		-> StreamNode<Surface<PixelType>, D>&
	{
		auto& node = pipeline. template allocateNode<ag::graphics::ClearNode<PixelType, D> >(
			pipeline.backend,
			size_,
			color_);
		node.size.successors.push_back(&node);
		node.color.successors.push_back(&node);
		return node;
	}

	/////////////////////////////////////////// displayRT
	template <
		typename D
	>
	void displayRT(
		Pipeline<D>& pipeline,
		StreamNode<Surface<glm::u8vec4>, D>& rt)
	{
		auto& node = pipeline. template allocateNode<ag::graphics::DisplayNode<D> >(pipeline.backend, rt);
		node.rt.successors.push_back(&node);
		pipeline.backend.setDisplayNode(node);
	}
}

#endif // !PIPELINE_HPP
