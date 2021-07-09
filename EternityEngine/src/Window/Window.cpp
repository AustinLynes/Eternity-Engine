#include "Window.h"

#include "Application.h"
#include <iostream>

namespace EternityEngine {

	GLFWwindow* Window::m_Window = nullptr;
	Window* Window::m_Instance = nullptr;


	int m_Width = 800;
	int m_Height = 600;
	std::string m_Title = "NO TITLE";

	Window::Window(const std::string& title, int width, int height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (m_Window == nullptr) throw std::runtime_error("Could Not Create Window Insance.");

	}

	bool Window::Create(const std::string& title, int width, int height)
	{

		if (m_Instance != nullptr)
		{
			std::cout << "Instance already exists. can only have 1 window at the moment." << std::endl;
			return false;
		}

		m_Instance = new Window(title, width, height);

		return true;

	}

	void Window::FrameBuffercallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		app->OnFrameBufferResized(width, height);
		app->FrameBufferInvalidated = true;
	}
}