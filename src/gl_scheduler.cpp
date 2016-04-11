#include "gl_scheduler.hpp"


void gl_scheduler::optimize(std::shared_ptr<value_impl> v) {}

void gl_scheduler::evaluate(const image_impl &img, const gl_surface &target) {
  // check if img is marked dirty
  // yes: eval img
  // no: return resource associated w/ img

  // eval img:
  //	eval dependencies
  //    execute node
  //    unmark dirty

  // if there is no plan for img
  //   create plan
  //   allocate output resources
}

void gl_scheduler::schedule(const value_impl &img) 
{

}
