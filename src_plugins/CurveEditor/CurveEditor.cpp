#include "CurveEditor.Config.h"
#include <autograph/engine/All.h>
#include <autograph/gl/All.h>
#include <autograph/editor/QtExtension.h>

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
PLUGIN_ENTRY{
    ag::registerClass<CurveEditor, ag::QtExtension>("CurveEditor");
}
