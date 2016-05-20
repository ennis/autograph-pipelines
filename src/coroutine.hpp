#pragma once
#include <boost/assert.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/context/fixedsize_stack.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>
#include <iostream>
#include <tuple>

enum class coroutine_status { suspended, running, terminated };

class coroutine {
public:
	// this number (64k) is only a vague guess and is subject to change
	// a solution would be to use segmented stacks, but this requires compiler support
  static constexpr auto default_stack_size = 0x10000u;
  template <typename F, typename... Args>
  coroutine(F &&fn_, Args &&... args_) : stack_alloc(default_stack_size) {
    using namespace boost::context;
    sctx = stack_alloc.allocate();
    context = make_fcontext(sctx.sp, sctx.size, &trampoline<F, Args...>);
    entry_function<F, Args...> entry_fn{std::forward<F>(fn_),
                                        std::forward_as_tuple(args_...)};
    auto tmp = current_task;
    current_task = this;
    jump_fcontext(&next_context, context, (intptr_t)&entry_fn, true);
    current_task = tmp;
  }

  ~coroutine() { stack_alloc.deallocate(sctx); }

  void resume() {
    auto tmp = current_task;
    current_task = this;
    // std::clog << "resume task\n";
    boost::context::jump_fcontext(&next_context, context, 0, true);
    // std::clog << "return from task\n";
    current_task = tmp;
  }

  static coroutine *current() { return current_task; }
  static void suspend() {
    // std::clog << "task context: " << current_task->next_context << "\n";
    boost::context::jump_fcontext(&current_task->context,
                                  current_task->next_context, 0, true);
  }

private:
  template <typename F, typename... Args> struct entry_function {
    F &&f;
    std::tuple<Args &&...> args;
  };

  template <typename F, typename... Args> static void trampoline(intptr_t p) {
    // move function and arguments on this stack frame
    auto entry_fn = reinterpret_cast<entry_function<F, Args...> *>(p);
    auto fn = std::move(entry_fn->f);
    auto args = std::move(entry_fn->args);
    // std::clog << "Started task\n";
    util::call(fn, args);
  }

  boost::context::protected_fixedsize_stack stack_alloc;
  boost::context::stack_context sctx;
  boost::context::fcontext_t context;
  boost::context::fcontext_t next_context;
  static thread_local coroutine *current_task;
};
