#pragma once
#include <autograph/engine/Config.h>

namespace ag 
{
	template <typename T>
	class InterfacePtr 
	{
	public:
		T* get() { return ptr_; }
	private:
		int moduleIndex_;
		T* ptr_;
	};


	AG_ENGINE_API void loadDynamicModule(const char* path);
	AG_ENGINE_API void unloadDynamicModule(const char* path);
	AG_ENGINE_API void unloadDynamicModules();

	template <typename T>
	InterfacePtr<T> createClassInstance(const char* className);	

	// auto sceneEditor = createClassInstance<IGUIExtension>("SceneEditor");
}
