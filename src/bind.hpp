#pragma once
#include "gl_sampler.hpp"
#include "gl_shader_resources.hpp"
#include "image_impl.hpp"
#include "buffer.hpp"
#include "shader_resource.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <tuple>

namespace bind {
////////////////////////// BindContext
struct bind_context {
  unsigned draw_attachement_index = 0;
  unsigned texture_index = 0;
  unsigned sampler_index = 0;
  unsigned vertex_buffer_index = 0;
  unsigned uniform_buffer_index = 0;
  unsigned image_index = 0;
  unsigned shader_storage_buffer_index = 0;

  void bind_texture(unsigned slot, std::shared_ptr<image_impl> img)
  {
      shader_resource r;
      r.access = shader_resource_access::read;
      r.slot = slot;
      r.type = shader_resource_type::sampled_image;
      r.resource = img;
      res.emplace_back(std::move(r));
      // TODO for now, all shader parameters must be images with actual device storage
      if (img->stype != storage_type::device)
        throw std::logic_error(
            "bind_context::bind_texture: Image bound as a shader resource must be resident on device");
      gl_res.textures[slot] = img->storage.device_tex->obj_.get();
  }

  void bind_image(unsigned slot, std::shared_ptr<image_impl> img, shader_resource_access access)
  {
      shader_resource r;
      r.access = access;
      r.resource = img;
      r.type = shader_resource_type::storage_image;
      r.slot = slot;
      res.emplace_back(std::move(r));
      if (img->stype != storage_type::device)
        throw std::logic_error(
            "bind_context::bind_texture: Image bound as a shader resource must be resident on device");
      gl_res.images[slot] = img->storage.device_tex->obj_.get();
  }

  void bind_sampler(unsigned slot, GLuint obj)
  {
      gl_res.samplers[slot] = obj;
  }

  void bind_vertex_buffer(unsigned slot, GLuint handle, size_t offset,
      size_t size, unsigned stride)
  {
      gl_res.vbo[slot] = handle;
      gl_res.vbo_offsets[slot] = offset;
      gl_res.vbo_strides[slot] = stride;
  }

  void bind_index_buffer(std::shared_ptr<buffer_impl> buf, GLenum type)
  {
      //i = type;
      shader_resource r;
      r.access = shader_resource_access::read;
      r.type = shader_resource_type::index_buffer;
      r.slot = 0;
      r.resource = buf;
      res.emplace_back(std::move(r));
      gl_res.ibo = handle;
  }

  void bind_uniform_buffer(unsigned slot, GLuint handle, size_t offset,
      size_t size)
  {
      shader_resource r;
      r.access = shader_resource_access::read;
      r.type = shader_resource_type::uniform_buffer;
      r.slot = 0;
      r.resource = buf;
      res.emplace_back(std::move(r));
      gl_res.ubo[slot] = handle;
      gl_res.ubo_sizes[slot] = size;
      gl_res.ubo_offsets[slot] = offset;
  }

  void bind_storage_buffer(unsigned slot, GLuint handle, size_t offset,
      size_t size)
  {
      gl_res.ssbo[slot] = handle;
      gl_res.ssbo_sizes[slot] = size;
      gl_res.ssbo_offsets[slot] = offset;
  }

  shader_resources res;
  gl_shader_resources gl_res;
};

/////////////////////////////
struct storage_image {
  int slot;
  std::shared_ptr<image_impl> img;
  shader_resource_access access;
};

/////////////////////////////
struct sampled_image {
  int slot;
  std::shared_ptr<image_impl> img;
  gl_sampler &sampler;
};

/////////////////////////////
struct uniform_buffer {
    int slot;
    std::shared_ptr<buffer_impl> buf;
};

/////////////////////////////
struct storage_buffer {
    int slot;
    std::shared_ptr<buffer_impl> buf;
    shader_resource_access access;
};

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
{
    std::shared_ptr<buffer_impl> buf;
};

template <typename T>
struct vertex_buffer
{
    std::shared_ptr<buffer_impl> buf;
};

/////////////////////////////
template <typename T>
struct binder;

template <typename U>
struct binder<index_buffer<U>>
{
  void bind(shader_resources& res, const index_buffer<U>& buf);
  void bind_gl(gl_shader_resources& gl_res, const index_buffer<U>& buf);
};

template <typename U>
struct binder<vertex_buffer<U>>
{
  void bind(shader_resources& res, const vertex_buffer<U>& buf);
  void bind_gl(gl_shader_resources& gl_res, const vertex_buffer<U>& buf);
};

template <typename U>
struct binder<uniform_buffer<U>>
{
  void bind(shader_resources& res, const uniform_buffer<U>& buf);
  void bind_gl(gl_shader_resources& gl_res, const uniform_buffer<U>& buf);
};

struct binder<sampled_image>
{
  void bind(shader_resources& res, const sampled_image& img);
  void bind_gl(gl_shader_resources& gl_res, const sampled_image& img);
};

struct binder<storage_image>
{
  void bind(shader_resources& res, const storage_image& img);
  void bind_gl(gl_shader_resources& gl_res, const storage_image& img);
};












////////////////////////// Bind<index_buffer<T> >
template <typename T>
void bind_shader_resource(bind_context &context,
                          shader_resources &res,
                          const index_buffer<T> &ibuf)
{
  GLenum type;
  if (std::is_same<T, unsigned short>::value)
    type = gl::UNSIGNED_SHORT;
  if (std::is_same<T, unsigned int>::value)
    type = gl::UNSIGNED_INT;
  context.res.bind_index_buffer(ibuf.buf.obj_.get(), 0, ibuf.buf.byte_size_,
                                type);
}

////////////////////////// Bind<texture_unit<>>
inline void bind_shader_resource(bind_context &context,
                                 shader_resources &res,
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
inline void bind_shader_resource(bind_context &context,
                                 shader_resources &res,
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
inline void bind_shader_resource(bind_context &context,
                                 shader_resources &res,
                                 const uniform_buffer &ubo) {
    // TODO named uniforms, for convenience

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
