#ifndef PIXEL_TYPE_HPP
#define PIXEL_TYPE_HPP

#include <cstdint>
#include <array>

namespace ag {

// TODO: distinguish "surface format" (number of channels, number of bits) from
// "interpretation format" (float, uint, unorm, sint, snorm)

enum class PixelFormat {
  // 32x4
  Uint32x4,
  Sint32x4,
  Float4,
  // 32x3
  Uint32x3,
  Sint32x3,
  Float3,
  // 32x2
  Float2,
  // 16x4
  Uint16x4,
  Sint16x4,
  Unorm16x4,
  Snorm16x4,
  Float16x4,
  // 16x2
  Uint16x2,
  Sint16x2,
  Unorm16x2,
  Snorm16x2,
  Float16x2,
  // 8x4
  Uint8x4,
  Sint8x4,
  Unorm8x4,
  Snorm8x4,
  // 8x3
  Uint8x3,
  Sint8x3,
  Unorm8x3,
  Snorm8x3,
  // 8x2
  Uint8x2,
  Sint8x2,
  Unorm8x2,
  Snorm8x2,
  // 10_10_10_2
  Unorm10x3_1x2,
  Snorm10x3_1x2,
  // Compressed formats
  BC1, // DXT1
  BC2, // DXT3
  BC3, // DXT5
  UnormBC4,
  SnormBC4,
  UnormBC5,
  SnormBC5,
  // Single
  Uint32,
  Sint32,
  Uint16,
  Sint16,
  Unorm16,
  Snorm16,
  //
  Uint8,
  Sint8,
  Unorm8,
  Snorm8,
  // TODO
  Depth32,
  Depth24,
  Depth24_Stencil8,
  Depth16,
  Float16,
  Float,
  Max
};

const char *get_pixel_format_name(PixelFormat fmt);

template <typename T> struct PixelTypeTraits {
  static constexpr bool kIsPixelType = false;
};

template <PixelFormat Format, typename Sample, typename Storage = Sample>
struct PixelTypeTraitsImpl {
  static constexpr bool kIsPixelType = true;
  static constexpr PixelFormat kFormat = Format;
  // type of elements in raw texture data
  using storage_type = Storage;
  // interpolated/sampled pixel type (e.g. for unorm types this is float, for
  // integer types, this is (u)intX)
  using sample_type = Sample;
};

template <>
struct PixelTypeTraits<float>
    : public PixelTypeTraitsImpl<PixelFormat::Float, float> {};
template <>
struct PixelTypeTraits<std::array<float, 2>>
    : public PixelTypeTraitsImpl<PixelFormat::Float2, std::array<float, 2>> {};
template <>
struct PixelTypeTraits<std::array<float, 3>>
    : public PixelTypeTraitsImpl<PixelFormat::Float3, std::array<float, 3>> {};
template <>
struct PixelTypeTraits<std::array<float, 4>>
    : public PixelTypeTraitsImpl<PixelFormat::Float4, std::array<float, 4>> {};

template <>
struct PixelTypeTraits<uint8_t>
    : public PixelTypeTraitsImpl<PixelFormat::Uint8, uint8_t> {};
template <>
struct PixelTypeTraits<std::array<uint8_t, 2>>
    : public PixelTypeTraitsImpl<PixelFormat::Uint8x2, std::array<uint8_t, 2>> {
};
template <>
struct PixelTypeTraits<std::array<uint8_t, 3>>
    : public PixelTypeTraitsImpl<PixelFormat::Uint8x3, std::array<uint8_t, 3>> {
};
template <>
struct PixelTypeTraits<std::array<uint8_t, 4>>
    : public PixelTypeTraitsImpl<PixelFormat::Uint8x4, std::array<uint8_t, 4>> {
};

template <>
struct PixelTypeTraits<uint32_t>
    : public PixelTypeTraitsImpl<PixelFormat::Uint32, uint32_t> {};

// wrapper type for normalized pixel formats
template <typename T> struct Normalized { T value; };

template <>
struct PixelTypeTraits<Normalized<uint8_t>>
    : public PixelTypeTraitsImpl<PixelFormat::Unorm8, Normalized<uint8_t>> {};
template <>
struct PixelTypeTraits<std::array<Normalized<uint8_t>, 2>>
    : public PixelTypeTraitsImpl<PixelFormat::Unorm8x2,
                                 std::array<Normalized<uint8_t>, 2>> {};
template <>
struct PixelTypeTraits<std::array<Normalized<uint8_t>, 3>>
    : public PixelTypeTraitsImpl<PixelFormat::Unorm8x3,
                                 std::array<Normalized<uint8_t>, 3>> {};
template <>
struct PixelTypeTraits<std::array<Normalized<uint8_t>, 4>>
    : public PixelTypeTraitsImpl<PixelFormat::Unorm8x4,
                                 std::array<Normalized<uint8_t>, 4>> {};

// Packed formats
struct Snorm10x3_1x2 {
  uint32_t v;
};

template <>
struct PixelTypeTraits<Snorm10x3_1x2>
    : public PixelTypeTraitsImpl<PixelFormat::Snorm10x3_1x2, Snorm10x3_1x2> {};

struct Unorm10x3_1x2 {
  uint32_t v;
};

template <>
struct PixelTypeTraits<Unorm10x3_1x2>
    : public PixelTypeTraitsImpl<PixelFormat::Unorm10x3_1x2, Unorm10x3_1x2> {};

// depth and depth-stencil format type
struct Depth32 {
  uint32_t v;
};
struct Depth24_Stencil8 {
  uint32_t v;
};

template <>
struct PixelTypeTraits<Depth32>
    : public PixelTypeTraitsImpl<PixelFormat::Depth32, Depth32> {};

// sampling a combined depth-stencil type does not make much sense
template <>
struct PixelTypeTraits<Depth24_Stencil8>
    : public PixelTypeTraitsImpl<PixelFormat::Depth24_Stencil8,
                                 Depth24_Stencil8> {};

using RGBA8 = std::array<Normalized<uint8_t>, 4>;
using RGB8 = std::array<Normalized<uint8_t>, 3>;
using RG8 = std::array<Normalized<uint8_t>, 2>;
using R32F = float;
using RG32F = std::array<float, 2>;
using RGB32F = std::array<float, 3>;
using RGBA32F = std::array<float, 4>;
using R8 = Normalized<uint8_t>;
using R32UI = uint32_t;
}

#endif
