#include "OpenEXR_IO.Config.h"
#include <autograph/engine/All.h>
#include <autograph/gl/All.h>

class OpenEXR_IO : public ag::Extension
{
public:
	// load into float buffer
	// save from float buffer
	// load into OpenGL texture
	// save from OpenGL texture
	virtual std::unique_ptr<char[]> loadEXR(const char* path, int& outWidth, int& outHeight) = 0;
	// UI hook: save framebuffer
};

//////////////////////////////////////////////
class OpenEXR_IO_Impl : public OpenEXR_IO
{
public:
	virtual std::unique_ptr<char[]> loadEXR(const char* path, int& outWidth, int& outHeight) override
	{
		Imf::Array2D<Imf::Rgba> data;
		Imf::RgbaInputFile inputFile{ path };
	}
};

//////////////////////////////////////////////
PLUGIN_ENTRY{
    ag::registerClass<OpenEXR_IO, ag::Extension>("OpenEXR_IO");
}
