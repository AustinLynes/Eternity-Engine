#pragma once
#include <cstdint>

#include <vulkan/vulkan_core.h>
#include <vector>
namespace EternityEngine {

	namespace API {
		enum class Specification {
			None,
			Vulkan,
			DirectX
		};
	}

	enum class BufferType {
		None, 
		Vertex, 
		Index, 
		Uniform
	};

	class GraphicsAPI
	{
	public:
		static API::Specification GetAPI() { return m_Specification; }

		// virtual commands to controll the graphics API

		enum class BufferUsage {
			None, 
			Static,
			Dynamic
		};


		virtual std::vector<VkFramebuffer> GetFrameBuffers() = 0;


		virtual void CreateVertexArrayObject() = 0;
		virtual void BindVertexArrayObject(uint32_t position) = 0;

		// Creates a Vertex Buffer attached to the bound vertex array object
		virtual void CreateVertexBuffer(void* vertexData, uint64_t size) = 0;
		// Creates a Index Buffer attached to the bound vertex array object
		virtual void CreateIndexBuffer(void* indexData, uint64_t size) = 0;
		
		// Creates a Uniform Buffer
		virtual void CreateUniformBuffer() = 0;
		
		virtual void CreateGraphicsPipeline(VkVertexInputBindingDescription bindingDescriptor, VkVertexInputAttributeDescription* attribDescriptors, size_t attrs_count) = 0;
		virtual void BindVertexBuffer() = 0;
		virtual void BindIndexBuffer() = 0;
		virtual void BindUniformBuffer() = 0;

		// COMMANDS 
		virtual void BeginSingleCommand() = 0;
		virtual void EndSingleCommand() = 0;
		
		// RENDER PASS 
		virtual void BeginRenderPass(float r, float g, float b, float a) = 0;
		virtual void EndRenderPass() = 0;

		virtual void BeginGUIRenderPass() = 0;
		virtual void EndGUIRenderPass() = 0;


		virtual void DrawIndexed(uint32_t count) = 0;

		virtual void SwapBuffers() = 0; 
		virtual void RecreateSwapchain() = 0;

	public:
		static API::Specification m_Specification;
	};

}

