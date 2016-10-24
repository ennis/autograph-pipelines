#pragma once
#include <autograph/Types.h>
#include <autograph/editor/Observable.h>
#include <autograph/editor/Input.h>

struct GLFWwindow;
class GrContext;
class SkSurface;

namespace ag 
{
//////////////////////////////////////////////
class AG_API Application
{
public:
	Application(uvec2 initSize);
	~Application();

	virtual void resize();
	virtual void render();
	virtual void onInputEvent(InputEvent& ev);
	uvec2 getFramebufferSize();
	uvec2 getWindowSize();
	GLFWwindow* getWindow() const;
	SkSurface* getSkSurface();
	Subscription& getSubscription();
	void run();

private:
	void initialize(uvec2 initSize);
	void terminate();
	void handleWindowSizeChanged(uvec2 newSize);
	void createSkiaSurface(uvec2 size);
	struct Impl;
	std::unique_ptr<Impl> impl; 

	static void appWindowSizeChanged(GLFWwindow* window, int width, int height);
};

}