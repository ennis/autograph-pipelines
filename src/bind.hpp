#pragma once
#include "gl_sampler.hpp"
#include "gl_shader_resources.hpp"
#include "image_impl.hpp"
#include "shader_resource.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <tuple>

namespace bind {
////////////////////////// BindContext
struct gl_bind_context {
  unsigned draw_attachement_index = 0;
  unsigned texture_index = 0;
  unsigned sampler_index = 0;
  unsigned vertex_buffer_index = 0;
  unsigned uniform_buffer_index = 0;
  unsigned image_index = 0;
  unsigned shader_storage_buffer_index = 0;
  gl_shader_resources res;
};

/////////////////////////////
struct storage_image {
  int slot;
  std::shared_ptr<image_impl> img;
  shader_resource_access access;
};

void bind_resource_gl(gl_bind_context &ctx, const storage_image &r) {}

/////////////////////////////
struct sampled_image {
  int slot;
  std::shared_ptr<image_impl> img;
  gl_sampler &sampler;
};

/////////////////////////////
struct uniform_buffer {};

/////////////////////////////
struct storage_buffer {};

/////////////////////////////
struct draw_attachement {
	int slot;
	std::shared_ptr<image_impl> img;
};

/////////////////////////////
struct depth_attachement {
	std::shared_ptr<image_impl> img;
};

template <typename T>
struct index_buffer
{};

////////////////////////// Bind<index_buffer<T> >
template <typename T>
void bind_shader_resource(gl_bind_context &context,
                          const index_buffer<T> &ibuf) {
  GLenum type;
  if (std::is_same<T, unsigned short>::value)
    type = gl::UNSIGNED_SHORT;
  if (std::is_same<T, unsigned int>::value)
    type = gl::UNSIGNED_INT;
  context.res.bind_index_buffer(ibuf.buf.obj_.get(), 0, ibuf.buf.byte_size_,
                                type);
}

////////////////////////// Bind<texture_unit<>>
inline void bind_shader_resource(gl_bind_context &context,
                                 const sampled_image &si) {
  context.texture_index = si.slot;
  context.sampler_index = si.slot;
  if (si.img->stype != storage_type::device)
    throw std::logic_error(
        "Image bound as a shader resource must be resident on device");
  context.res.bind_texture(context.texture_index++,
                           si.img->storage.device_tex->obj_.get());
  context.res.bind_sampler(context.sampler_index++, si.sampler.object());
}

////////////////////////// Bind<image_unit<>>
inline void bind_shader_resource(gl_bind_context &context,
                                 const storage_image &si) {
  context.image_index = si.slot;
  if (si.img->stype != storage_type::device)
	// TODO: host->device copy
    throw std::logic_error(
        "Image bound as a shader resource must be resident on device");
  context.res.bind_image(context.image_index++,
                         si.img->storage.device_tex->obj_.get());
}

////////////////////////// Bind<gl_buffer_slice>
inline void bind_shader_resource(gl_bind_context &context,
                                 const uniform_buffer &ubo) {
  /*g_device->bind_uniform_buffer(context.uniformBufferBindingIndex++,
                                buf_slice.obj, buf_slice.offset,
                                buf_slice.size);*/
}

////////////////////////// Bind<T>


//////////////////////////
/*template <typename T>
void bind_shader_resources_rec(BindContext &context, T &&resource) {
  bind_shader_resource(context, std::forward<T>(resource));
}*/

/*template <typename T, typename... Rest>
void bind_shader_resources_rec(BindContext &context, T &&resource,
                               Rest &&... rest) {
  bind_shader_resource(context, std::forward<T>(resource));
  bind_shader_resources_rec(context, std::forward<Rest>(rest)...);
}*/
}
