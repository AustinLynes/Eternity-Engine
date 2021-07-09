#pragma once
#include "GraphicsAPI.h"
#include "Graphics/Structures.h"

namespace EternityEngine {

	class RenderCommand 
	{
	public:
		static void SetTargetSpecification(API::Specification spec);
		

		inline static void SwapBuffers() {
			m_graphicsAPI->SwapBuffers();
		}
		
		inline static void CreatePipeline(VertexBindingDescriptor* bindingDescriptor, VertexAttributeDescriptor* attribDescriptors, size_t attrs_count) {
			m_graphicsAPI->CreateGraphicsPipeline(*(VkVertexInputBindingDescription*)bindingDescriptor, (VkVertexInputAttributeDescription*)attribDescriptors, attrs_count);
		}
		inline static void CreateVertexArrayObject() {
			m_graphicsAPI->CreateVertexArrayObject();
		}
		inline static void BindVertexArrayObject(int position) {
			m_graphicsAPI->BindVertexArrayObject(position);
		}
		inline static void BeginSingleCommand() {
			m_graphicsAPI->BeginSingleCommand();
		}
		inline static void EndSingleCommand() {
			m_graphicsAPI->EndSingleCommand();
		}
		inline static void RecreateSwapChain() {
			m_graphicsAPI->RecreateSwapchain();
		}
		inline static void CreateBuffer(BufferType bufferType, void* data, uint64_t size) {
			switch (bufferType) {
			case  BufferType::Vertex:
				m_graphicsAPI->CreateVertexBuffer(data, size);
				break;
			case BufferType::Index:
				m_graphicsAPI->CreateIndexBuffer(data, size);
				break;
			case BufferType::Uniform:
				m_graphicsAPI->CreateUniformBuffer();
				break;
			}
		}
		inline static void BindBuffer(BufferType bufferType) {
			switch (bufferType) {
			case BufferType::Vertex:
				m_graphicsAPI->BindVertexBuffer();
				break;
			case BufferType::Index:
				m_graphicsAPI->BindIndexBuffer();
				break;
			case BufferType::Uniform:
				m_graphicsAPI->BindUniformBuffer();
				break;

			}
		}
		inline static void BeginRenderPass(float r, float g, float b, float a) {
			m_graphicsAPI->BeginRenderPass(r, g, b, a);
		}
		inline static void BeginGUIRenderPass() {
			m_graphicsAPI->BeginGUIRenderPass();
		}
		inline static void EndGUIRenderPass() {
			m_graphicsAPI->EndGUIRenderPass();
		}
		
		inline static void EndRenderPass() { m_graphicsAPI->EndRenderPass(); }
		inline static void DrawIndexed(uint32_t count) { m_graphicsAPI->DrawIndexed(count); }

		inline static GraphicsAPI* GetAPI() { return m_graphicsAPI; }
	private:
		static GraphicsAPI* m_graphicsAPI;

	};
}

