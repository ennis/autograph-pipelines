#pragma once
#include <autograph/Types.h>
#include <autograph/engine/Observable.h>
#include <autograph/engine/Input.h>

struct GLFWwindow;
//class GrContext;
//class SkSurface;

namespace ag 
{
//////////////////////////////////////////////
class AG_API Application
{
public:
	Application(ivec2 initSize);
	~Application();

	virtual void resize();
	virtual void render();
	virtual void onInputEvent(InputEvent& ev);
	ivec2 getFramebufferSize();
	ivec2 getWindowSize();
	GLFWwindow* getWindow() const;
	//SkSurface* getSkSurface();
	Subscription& getSubscription();
	void run();

private:
	void initialize(ivec2 initSize);
	void terminate();
	void handleWindowSizeChanged(ivec2 newSize);
	//void createSkiaSurface(uvec2 size);
	struct Impl;
	std::unique_ptr<Impl> impl; 

	static void appWindowSizeChanged(GLFWwindow* window, int width, int height);
};

}