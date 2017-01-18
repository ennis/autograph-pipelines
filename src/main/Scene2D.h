#pragma once
#include <autograph/engine/Mesh.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/engine/Shader.h>
#include <autograph/gl/Buffer.h>
#include <autograph/gl/Framebuffer.h>

namespace ag {
class AG_API Scene2D {
public:
  struct Tileset {
    gl::Texture *tex;
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
    gl::Texture tileMapTex; // tile coordinates
  };

  struct Viewport {
    float x; // pixels
    float y;
    float width;
    float height;
  };

  Scene2D();

  void loadTilemap(const char *id);
  void render(gl::Framebuffer &screen, float viewX, float viewY,
              float viewWidth, float viewHeight);
  void render(gl::Framebuffer &screen, const Viewport &viewport);

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
  gl::Sampler sampler_;
};
}