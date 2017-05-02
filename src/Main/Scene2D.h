#pragma once
#include <autograph/Engine/Mesh.h>
#include <autograph/Engine/ResourcePool.h>
#include <autograph/Engine/Shader.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Framebuffer.h>

namespace ag {
class Scene2D {
public:
  struct Tileset {
    Texture *tex;
    int tileWidth;  // in pixels
    int tileHeight; // in pixels
    int width;      // in tiles
    int height;     // in tiles
  };

  struct Tilemap {
    int width;              // in tiles
    int height;             // in tiles
    int tileset;            // tileset index
    std::vector<int> data;  // list of tile IDs (max 65536 different tiles)
    Texture tileMapTex; // tile coordinates
  };

  struct Viewport {
    float x; // pixels
    float y;
    float width;
    float height;
  };

  Scene2D();

  void loadTilemap(const char *id);
  void render(Framebuffer &screen, float viewX, float viewY,
              float viewWidth, float viewHeight);
  void render(Framebuffer &screen, const Viewport &viewport);

private:
  ResourcePool resourcePool_;
  std::vector<Tileset> tilesets_;
  std::vector<Tilemap> tilemaps_;
  int tileWidth_{0};
  int tileHeight_{0};
  // VBO containing the tile grid
  Mesh2DTex tileGrid_;
  int gridWidthTiles_{0};  // in tiles
  int gridHeightTiles_{0}; // in tiles
  Shader tileMapShader_;
  Sampler sampler_;
};
}