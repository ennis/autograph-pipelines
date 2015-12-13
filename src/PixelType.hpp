#ifndef PIXEL_TYPE_HPP
#define PIXEL_TYPE_HPP

#include <glm/glm.hpp>
#include <OpenGL.hpp>

namespace ag
{
	using namespace gl;

	template <typename T>
	struct PixelTypeTraits
	{
		static constexpr bool IsPixelType = false;
	};

	template <
		GLenum ExternalFormat_, 
		unsigned NumComponents_,
		GLenum InternalFormat_>
	struct PixelTypeTraitsImpl
	{
		static constexpr bool IsPixelType = true;
		static constexpr GLenum ExternalFormat = ExternalFormat_;
		static constexpr unsigned NumComponents = NumComponents_;
		static constexpr GLenum InternalFormat = InternalFormat_;
	};

	template <> struct PixelTypeTraits<float> : public PixelTypeTraitsImpl<GL_FLOAT, 1, GL_R32F> {};
	template <> struct PixelTypeTraits<glm::u8vec3> : public PixelTypeTraitsImpl<GL_UNSIGNED_BYTE, 3, GL_RGB8> {};
	template <> struct PixelTypeTraits<glm::u8vec4> : public PixelTypeTraitsImpl<GL_UNSIGNED_BYTE, 4, GL_RGBA8> {};

	using RGBA8 = glm::u8vec4;
	using RGB8 = glm::u8vec3;
	using R32F = glm::f32;
	using R8 = glm::u8;
}

#endif
