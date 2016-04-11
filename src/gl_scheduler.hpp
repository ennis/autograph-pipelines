#pragma once
#include <gsl.h>

#include "buffer.hpp"
#include "cast_node.hpp"
#include "clear_node.hpp"
#include "compute_node.hpp"
#include "draw_node.hpp"
#include "image.hpp"
#include "upload_buffer.hpp"
#include "texture_cache.hpp"

enum class gl_surface_kind {
  texture,
  screen,
  // TODO: presentable image
};

struct gl_surface {
  gl_surface(gl_surface_kind kind) : kind_{kind} {}
  gl_surface_kind kind_;
};

struct gl_texture_surface : public gl_surface {
  gl_texture_surface(gl_texture &tex, unsigned index = 0,
                     unsigned mip_level = 0)
      : gl_surface{gl_surface_kind::texture}, tex_{tex}, index_{index},
        mip_level_{mip_level} {}

  gl_texture &tex_;
  unsigned index_;
  unsigned mip_level_;
};

struct gl_screen_surface : public gl_surface {
  gl_screen_surface() : gl_surface{gl_surface_kind::screen} {}
};


struct gl_scheduler {
  // optimize: create an optimized evaluation plan
  //	 in: values (gsl::span of pointers)
  //   out: void
  void optimize(std::shared_ptr<value_impl> v);

  // evaluate: render an image onto a surface (image data, screen, GPU texture)
  //   in: image, target surface
  //   out: void
  // side-effects: validate all predecessors of the node
  void evaluate(const image_impl &img, const gl_surface &target);

  // schedule: mark a node (and its successors) dirty
  //   in: value
  void schedule(const value_impl &img);

  // evaluation plan = list of commands
  // in each command: list of values that are realized by this command 
  // (image_impl, gl_texture, gl_buffer)

  // execution: schedule plans
  // scheduled plans = (plan + pointer)
  // when schedule() is called:
  //	
  //	reset pointers to beginning of command list

  texture_cache texcache_;
};
