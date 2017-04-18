#pragma once
#include <autograph/engine/Plugin.h>
#include <QtWidgets>

namespace ag
{
	class QtExtension : public Plugin
	{
	public:
		virtual void initialize(QMainWindow* mainWindow) = 0;
	};
}
