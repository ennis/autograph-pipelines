#include "SparseTextureTest/Exports.h"
#include <autograph/Engine/All.h>
#include <autograph/Gfx/All.h>

//////////////////////////////////////////////
class SparseTextureTest : public ag::Extension
{
public:
};

//////////////////////////////////////////////
SparseTextureTest_PLUGIN_ENTRY{
    ag::registerClass<SparseTextureTest, ag::Extension>("SparseTextureTest");
}
