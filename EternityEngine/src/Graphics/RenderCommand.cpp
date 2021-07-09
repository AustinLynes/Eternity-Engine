#include "RenderCommand.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanAPI.h"

namespace EternityEngine {
	
	GraphicsAPI* RenderCommand::m_graphicsAPI = nullptr;


	void RenderCommand::SetTargetSpecification(API::Specification spec)
	{
		delete m_graphicsAPI;
		m_graphicsAPI = new VulkanAPI(Window::GetInstance());
	}

}
