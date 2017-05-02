#include "CurveEditor/Exports.h"
#include <autograph/Engine/All.h>
#include <autograph/Gfx/All.h>
#include <autograph/Editor/QtExtension.h>

//////////////////////////////////////////////
class CurveEditor : public ag::QtExtension
{
public:
	void initialize(QMainWindow* mainWindow) override
	{
		AG_DEBUG("Initialized QtExtension");
	}
};

//////////////////////////////////////////////
CurveEditor_PLUGIN_ENTRY{
    ag::registerClass<CurveEditor, ag::QtExtension>("CurveEditor");
}
