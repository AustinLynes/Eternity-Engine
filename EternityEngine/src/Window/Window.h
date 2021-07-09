#pragma once


#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>		
#include <string>

namespace EternityEngine {

	class Window
	{
	
	public:
		Window(const std::string& title, int width, int height);
		//~Window();


		static bool Create(const std::string& title, int width, int height);
		
		static bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

		static GLFWwindow* GetGLFWwindow() { return m_Window; }
		static Window* GetInstance() { return m_Instance; }
		template<typename T>
		static void SetPointer(T __ptr) { glfwSetWindowUserPointer(m_Window, __ptr); }
		
		
		static void PollEvents() { glfwPollEvents(); };
		
	private:

		static void FrameBuffercallback(GLFWwindow* window, int width, int height);


	private:
		static Window* m_Instance;

		static GLFWwindow* m_Window;
		static int m_Width;
		static int m_Height;
		static std::string m_Title;

	};
}

