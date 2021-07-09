#include "Renderer.h"
#include <vector>

#include "Window/Window.h"

//#include <Platform/Vulkan/VulkanAPI.h>

namespace EternityEngine {

	// +->  1 .  ---+
	// ^		    |
	// |		    V
	// 0 . <---- . 2



	/*
					VULKAN NDC
		(-1, -1)									(1, -1)
			+----------------------------------------+
			|										 |
			|										 |
			|			    (0,0)+					 |
			|										 |
			|										 |
			|										 |
			+----------------------------------------+
		(-1, 1)										(1, 1)
	*/
	//const std::vector<Vertex> quad_vertices = {
	//	//	 X		  Y		Z		  R		 G		 B
	//	  { {-0.5f,	-0.5f,	0.0f},	{0.0,	1.0,	1.0 }},   // top left
	//	  { { 0.5f,	-0.5f,	0.0f},	{1.0,	0.0,	1.0 }},   // top right
	//	  { { 0.5f,	 0.5f,	0.0f},	{1.0,	1.0,	0.0 }},   // bottom right
	//	  { {-0.5f,	 0.5f,	0.0f},	{1.0,	0.0,	0.0 }},   // bottom left 

	//};

	//const std::vector<uint16_t> indices = {
	//	0,1,2 ,2,3,0
	//};
	void Renderer::Initilize()
	{




		// --------------------------------------------------------------------------
		//VertexBindingDescriptor desc;
		//desc.binding = 0;
		//desc.stride = sizeof(Vertex);
		//desc.inputRate = VertexBindingInputRate::VERTEX;



		//VertexAttributeDescriptor attrs[2]{
		//	{0, 0, ByteFormat::R32G32B32_SFLOAT, offsetof(Vertex, Position)},
		//	{1, 0, ByteFormat::R32G32B32_SFLOAT, offsetof(Vertex, Color)}
		//};
		//RenderCommand::CreatePipeline(&desc, attrs, 2);





		//RenderCommand::CreateVertexArrayObject();

		//RenderCommand::CreateBuffer(BufferType::Vertex, (void*)quad_vertices.data(), sizeof(quad_vertices[0]) * quad_vertices.size());
		//RenderCommand::CreateBuffer(BufferType::Index, (void*)indices.data(), sizeof(indices[0]) * indices.size());
		//RenderCommand::CreateBuffer(BufferType::Uniform, nullptr, 0);

		//RenderCommand::BeginSingleCommand();
		//RenderCommand::BeginRenderPass(0, 0, 0, 1);
		//RenderCommand::BindVertexArrayObject(0);

		//RenderCommand::BindBuffer(BufferType::Vertex);
		//RenderCommand::BindBuffer(BufferType::Index);
		//RenderCommand::BindBuffer(BufferType::Uniform);
		//RenderCommand::DrawIndexed(indices.size());
		//RenderCommand::EndRenderPass();
		//RenderCommand::EndSingleCommand();

		
	}

	void Renderer::DrawQuad()
	{

	}
}
