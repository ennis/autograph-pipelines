return {
  version = "1.1",
  luaversion = "5.1",
  tiledversion = "0.18.0",
  orientation = "orthogonal",
  renderorder = "right-down",
  width = 40,
  height = 40,
  tilewidth = 32,
  tileheight = 32,
  nextobjectid = 1,
  properties = {},
  tilesets = {
    {
      name = "Desert",
      firstgid = 1,
      filename = "../../../../../../Program Files/Tiled/examples/desert.tsx",
      tilewidth = 32,
      tileheight = 32,
      spacing = 1,
      margin = 1,
      image = "img/tmw_desert_spacing",
      imagewidth = 265,
      imageheight = 199,
      tileoffset = {
        x = 0,
        y = 0
      },
      properties = {},
      terrains = {
        {
          name = "Desert",
          tile = 29,
          properties = {}
        },
        {
          name = "Brick",
          tile = 9,
          properties = {}
        },
        {
          name = "Cobblestone",
          tile = 33,
          properties = {}
        },
        {
          name = "Dirt",
          tile = 14,
          properties = {}
        }
      },
      tilecount = 48,
      tiles = {
        {
          id = 0,
          terrain = { 0, 0, 0, 1 }
        },
        {
          id = 1,
          terrain = { 0, 0, 1, 1 }
        },
        {
          id = 2,
          terrain = { 0, 0, 1, 0 }
        },
        {
          id = 3,
          terrain = { 3, 3, 3, 0 }
        },
        {
          id = 4,
          terrain = { 3, 3, 0, 3 }
        },
        {
          id = 5,
          terrain = { 0, 0, 0, 3 }
        },
        {
          id = 6,
          terrain = { 0, 0, 3, 3 }
        },
        {
          id = 7,
          terrain = { 0, 0, 3, 0 }
        },
        {
          id = 8,
          terrain = { 0, 1, 0, 1 }
        },
        {
          id = 9,
          terrain = { 1, 1, 1, 1 }
        },
        {
          id = 10,
          terrain = { 1, 0, 1, 0 }
        },
        {
          id = 11,
          terrain = { 3, 0, 3, 3 }
        },
        {
          id = 12,
          terrain = { 0, 3, 3, 3 }
        },
        {
          id = 13,
          terrain = { 0, 3, 0, 3 }
        },
        {
          id = 14,
          terrain = { 3, 3, 3, 3 }
        },
        {
          id = 15,
          terrain = { 3, 0, 3, 0 }
        },
        {
          id = 16,
          terrain = { 0, 1, 0, 0 }
        },
        {
          id = 17,
          terrain = { 1, 1, 0, 0 }
        },
        {
          id = 18,
          terrain = { 1, 0, 0, 0 }
        },
        {
          id = 19,
          terrain = { 1, 1, 1, 0 }
        },
        {
          id = 20,
          terrain = { 1, 1, 0, 1 }
        },
        {
          id = 21,
          terrain = { 0, 3, 0, 0 }
        },
        {
          id = 22,
          terrain = { 3, 3, 0, 0 }
        },
        {
          id = 23,
          terrain = { 3, 0, 0, 0 }
        },
        {
          id = 24,
          terrain = { 0, 0, 0, 2 }
        },
        {
          id = 25,
          terrain = { 0, 0, 2, 2 }
        },
        {
          id = 26,
          terrain = { 0, 0, 2, 0 }
        },
        {
          id = 27,
          terrain = { 1, 0, 1, 1 }
        },
        {
          id = 28,
          terrain = { 0, 1, 1, 1 }
        },
        {
          id = 29,
          terrain = { 0, 0, 0, 0 }
        },
        {
          id = 30,
          terrain = { 0, 0, 0, 0 },
          probability = 0.01
        },
        {
          id = 31,
          terrain = { 0, 0, 0, 0 },
          probability = 0.01
        },
        {
          id = 32,
          terrain = { 0, 2, 0, 2 }
        },
        {
          id = 33,
          terrain = { 2, 2, 2, 2 }
        },
        {
          id = 34,
          terrain = { 2, 0, 2, 0 }
        },
        {
          id = 35,
          terrain = { 2, 2, 2, 0 }
        },
        {
          id = 36,
          terrain = { 2, 2, 0, 2 }
        },
        {
          id = 37,
          terrain = { 0, 0, 0, 0 },
          probability = 0.01
        },
        {
          id = 38,
          terrain = { 0, 0, 0, 0 },
          probability = 0.01
        },
        {
          id = 39,
          terrain = { 0, 0, 0, 0 },
          probability = 0.01
        },
        {
          id = 40,
          terrain = { 0, 2, 0, 0 }
        },
        {
          id = 41,
          terrain = { 2, 2, 0, 0 }
        },
        {
          id = 42,
          terrain = { 2, 0, 0, 0 }
        },
        {
          id = 43,
          terrain = { 2, 0, 2, 2 }
        },
        {
          id = 44,
          terrain = { 0, 2, 2, 2 }
        },
        {
          id = 45,
          terrain = { 0, 0, 0, 0 },
          probability = 0
        },
        {
          id = 46,
          terrain = { 0, 0, 0, 0 },
          probability = 0.01
        },
        {
          id = 47,
          terrain = { 0, 0, 0, 0 },
          probability = 0.01
        }
      }
    }
  },
  layers = {
    {
      type = "tilelayer",
      name = "Ground",
      x = 0,
      y = 0,
      width = 40,
      height = 40,
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      properties = {},
      encoding = "lua",
      data = {
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 14, 15, 16, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 46, 14, 15, 16, 30, 31, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 14, 15, 16, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 22, 23, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 1, 2, 3, 30, 30, 31, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 40, 30, 30, 30, 30, 33, 34, 36, 42, 37, 34, 34, 34, 34, 34, 34, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 35, 30, 33, 34, 34, 34, 34, 34, 34, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 38, 30, 30, 33, 34, 35, 30, 33, 34, 34, 34, 34, 34, 34, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 48, 38, 30, 30, 30, 30, 33, 34, 44, 26, 45, 34, 34, 34, 34, 34, 34, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 40, 30, 30, 30, 40, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 36, 42, 37, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        40, 30, 30, 30, 30, 40, 38, 30, 30, 38, 30, 33, 34, 34, 34, 34, 34, 34, 34, 44, 26, 45, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 39, 30, 30, 30, 38, 30, 40, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 39, 30, 30, 30, 30, 30, 30, 30, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 9, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        7, 7, 8, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 29, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        15, 15, 16, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        23, 23, 24, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 39, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 32, 31,
        30, 30, 30, 30, 39, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 32,
        39, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 46, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 36, 42, 37, 35, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 35, 48, 33, 35, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 47, 33, 34, 34, 34, 34, 34, 34, 34, 34, 35, 48, 33, 35, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 39, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 35, 48, 33, 35, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 44, 26, 45, 35, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 47, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 48,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 32, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 32, 30, 30, 30, 31, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 32, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30
      }
    }
  }
}
