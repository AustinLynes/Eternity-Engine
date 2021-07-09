#pragma once
#include <Window/Window.h>
#include <Graphics/Renderer.h>

namespace EternityEngine {
	class Application {
	public:
		Application();
		virtual ~Application();


		virtual void Initilize();
		virtual void Run();
		virtual void Cleanup();

		virtual void OnFrameBufferResized(int width, int height);
		bool FrameBufferInvalidated = false;


	};

	Application* CreateApplication();

}