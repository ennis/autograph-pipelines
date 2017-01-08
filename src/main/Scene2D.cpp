#include "Scene2D.h"
#include <autograph/engine/ScriptContext.h>
#include <autograph/engine/ImageUtils.h>
#include <autograph/support/Debug.h>
#include <autograph/gl/Draw.h>
#include <autograph/gl/Device.h>

namespace ag
{
	Scene2D::Scene2D()
	{
		tileMapPass_ = DrawPass{ "shaders/tileMap:tileMap" };
		gl::SamplerDesc desc;
		desc.minFilter = GL_NEAREST;
		desc.magFilter = GL_NEAREST;
		sampler_ = gl::Sampler{ desc };
	}

	void Scene2D::loadTilemap(const char * id)
	{
		ScriptContext ctx;
		sol::table tilemap = ctx.scriptFile(id);
		sol::table tilesets = tilemap["tilesets"];
		AG_DEBUG("{}", tilesets[1]["imagewidth"].get<int>());
		// load tilesets
		// TODO support multiple tilesets
		int tw;
		int th;
		for (auto pair : tilesets)
		{
			sol::table tileset = pair.second;
			// load image
			std::string img = tileset["image"];
			AG_DEBUG("Loading tileset {}", img);
			Tileset ts;
			ts.tex = resourcePool_.get<gl::Texture>(img.c_str());
			ts.tileWidth = tileset["tilewidth"];
			ts.tileHeight = tileset["tileheight"];
			if (tileWidth_ == 0) tileWidth_ = ts.tileWidth;
			if (tileHeight_ == 0) tileHeight_ = ts.tileHeight;
			tw = tileset["imagewidth"].get<int>();
			th = tileset["imageheight"].get<int>();
			ts.width = tw / ts.tileWidth;
			ts.height = th / ts.tileHeight;
			tilesets_.push_back(ts);
		}
		// Load layers
		sol::table layers = tilemap["layers"];
		auto& ts = tilesets_[0];
		for (auto pair : layers)
		{
			sol::table layer = pair.second;
			Tilemap map;
			map.width = layer["width"];
			map.height = layer["height"];
			// load data
			sol::table data = layer["data"];
			std::vector<vec2> texcoords;
			for (auto pair : data) {
				int tileID = pair.second.as<int>();
				map.data.push_back(tileID);
				vec2 t;
				// tile X,Y
				float tx = float((tileID - 1) % ts.width);
				float ty = float((tileID - 1) / ts.width);
				t.x = tx * ts.tileWidth / float(tw);
				t.y = ty * ts.tileHeight / float(th);
				texcoords.push_back(t);
			}
			// create texture
			map.tileMapTex = gl::Texture::create2D(ImageFormat::R32G32_SFLOAT, map.width, map.height);
			map.tileMapTex.upload(texcoords.data());
			tilemaps_.push_back(std::move(map));
		}
	}

	static Mesh2DTex createTileGrid(int w, int h)
	{
		int numVertices = 4 * (w+1) * (h+1);	// 4 verts per tile
		int numIndices = 6 * (w+1) * (h+1);	// 6 indices per tile
		std::vector<Vertex2DTex> vertices(numVertices);
		std::vector<unsigned> indices(numIndices);
		for (int i = 0; i < h+1; ++i) {
			for (int j = 0; j < w+1; ++j) {
				float fx0 = (float)j;
				float fx1 = (float)(j+1);
				float fy0 = (float)i;
				float fy1 = (float)(i + 1);
				// A - B
				// |   |
				// C - D
				int base = (i*(w+1) + j)*4;
				vertices[base] = Vertex2DTex{ {fx0,fy0},{0.0f,0.0f} };	// A
				vertices[base + 1] = Vertex2DTex{ {fx1,fy0},{0.0f,0.0f} };	// B
				vertices[base + 2] = Vertex2DTex{ {fx0,fy1},{0.0f,0.0f} };	// C
				vertices[base + 3] = Vertex2DTex{ {fx1,fy1},{0.0f,0.0f} };	// D
				int ibase = (i*(w+1) + j) * 6;
				indices[ibase] = base;		// A
				indices[ibase +1] = base +1;	// B
				indices[ibase +2] = base +2;	// C
				indices[ibase +3] = base+2;	// C
				indices[ibase + 4] = base+1;	// B
				indices[ibase + 5] = base+3;	// D
			}
		}
		return Mesh2DTex{std::move(vertices), std::move(indices)};
	}

	void Scene2D::render(gl::Framebuffer& screen, const Viewport& viewport)
	{
		// screen size in tiles
		int screenWidthTiles = (int)(viewport.width + tileWidth_ - 1) / tileWidth_;
		int screenHeightTiles = (int)(viewport.height + tileHeight_ - 1) / tileHeight_;
		// check if the current vertex grid is enough
		if (screenWidthTiles +1 > gridWidthTiles_ || screenHeightTiles +1 > gridHeightTiles_) {
			// re-create the VBO
			tileGrid_ = createTileGrid(screenWidthTiles, screenHeightTiles);
			gridWidthTiles_ = screenWidthTiles +1;
			gridHeightTiles_ = screenHeightTiles +1;
		}
		
		// draw the grid
		auto& tilemap = tilemaps_[0];
		auto& tileset = tilesets_[0];
		using namespace ag::gl;
		using namespace ag::gl::bind;
		struct Params {
			float offsetX;
			float offsetY;
			int tileOffsetX;
			int tileOffsetY;
			int tileSizePixelW;
			int tileSizePixelH;
			int viewportSizeW;
			int viewportSizeH;
			int gridSizeTileW;
			int gridSizeTileH;
		} p;

		auto pfmod = [](float x, float y) {
			return x - y * std::floor(x / y);
		};

		float vtx = std::floor(viewport.x / tileWidth_);
		float vty = std::floor(viewport.y / tileHeight_);
		float xoff = viewport.x - tileWidth_ * vtx;
		float yoff = viewport.y - tileHeight_ * vty;
		// pixel offsets
		p.tileOffsetX = (int)vtx;
		p.tileOffsetY = (int)vty;
		p.offsetX = xoff;
		p.offsetY = yoff;
		p.tileSizePixelW = tileWidth_;
		p.tileSizePixelH = tileHeight_;
		p.viewportSizeW = (int)viewport.width;
		p.viewportSizeH = (int)viewport.height;
		p.gridSizeTileW = gridWidthTiles_;
		p.gridSizeTileH = gridHeightTiles_;

		draw(screen, tileGrid_, tileMapPass_,
			texture(0, *tileset.tex, sampler_),
			texture(1, tilemap.tileMapTex, sampler_),
			uniformBuffer(0, ag::gl::uploadFrameData(&p, sizeof(p)))
			);

		// draw(screen, tileGrid, tileMapPass);

	}
}