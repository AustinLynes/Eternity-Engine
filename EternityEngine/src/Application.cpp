#include "Application.h"
#include <Dependencies/imgui/imgui_impl_vulkan.h>
#include <Dependencies/imgui/imgui_impl_glfw.h>
#include <vector>

namespace EternityEngine {
// REMIND MOM TO DO i9

	Application::Application()
	{
		// create the window
		if (Window::Create("Eternity Engine", 800, 600)) {

			Window::SetPointer<Application*>(this);

		}
		else {
			throw std::runtime_error("Could not create Window");
		}
	}
	Application::~Application()
	{
		// destroy vulanAPI
	}
	void Application::Initilize()
	{
		Renderer::Initilize();

	}
	void Application::Run()
	{
		const std::vector<Vertex> quad_vertices = {
			//	 X		  Y		Z		  R		 G		 B
			  { {-0.5f,	-0.5f,	0.0f},	{0.0,	1.0,	1.0 }},   // top left
			  { { 0.5f,	-0.5f,	0.0f},	{1.0,	0.0,	1.0 }},   // top right
			  { { 0.5f,	 0.5f,	0.0f},	{1.0,	1.0,	0.0 }},   // bottom right
			  { {-0.5f,	 0.5f,	0.0f},	{1.0,	0.0,	0.0 }},   // bottom left 

		};

		const std::vector<uint16_t> indices = {
			0,1,2 ,2,3,0
		};

		VertexBindingDescriptor desc;
		desc.binding = 0;
		desc.stride = sizeof(Vertex);
		desc.inputRate = VertexBindingInputRate::VERTEX;

		VertexAttributeDescriptor attrs[2]{
			{0, 0, ByteFormat::R32G32B32_SFLOAT, offsetof(Vertex, Position)},
			{1, 0, ByteFormat::R32G32B32_SFLOAT, offsetof(Vertex, Color)}
		};
		RenderCommand::CreatePipeline(&desc, attrs, 2);


		RenderCommand::CreateVertexArrayObject();

		RenderCommand::CreateBuffer(BufferType::Vertex, (void*)quad_vertices.data(), sizeof(quad_vertices[0]) * quad_vertices.size());
		RenderCommand::CreateBuffer(BufferType::Index, (void*)indices.data(), sizeof(indices[0]) * indices.size());
		RenderCommand::CreateBuffer(BufferType::Uniform, nullptr, 0);

		
		while (!Window::ShouldClose()) {
			Window::PollEvents();
			



			// Rendering
			RenderCommand::BeginSingleCommand();
			RenderCommand::BeginRenderPass(0, 0, 0, 1);
			{
				RenderCommand::BindVertexArrayObject(0);

				RenderCommand::BindBuffer(BufferType::Vertex);
				RenderCommand::BindBuffer(BufferType::Index);
				RenderCommand::BindBuffer(BufferType::Uniform);
				RenderCommand::DrawIndexed(indices.size());


				//RenderCommand::EndRenderPass();
			}
			RenderCommand::EndRenderPass();
			RenderCommand::BeginGUIRenderPass();
			{
				// * * * * * * * * * * * * 
				// ------------------------
				// >> DOCKSPACE WINDOW    
				// ------------------------
				// * * * * * * * * * * * * 

				bool open = true;
				static bool opt_fullscreen = true;
				static bool opt_padding = false;
				static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
				if (opt_fullscreen)
				{
					const ImGuiViewport* viewport = ImGui::GetMainViewport();
					ImGui::SetNextWindowPos(viewport->WorkPos);
					ImGui::SetNextWindowSize(viewport->WorkSize);
					ImGui::SetNextWindowViewport(viewport->ID);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

					window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
					window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
				}
				else
				{
					dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
				}

			
				if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
					window_flags |= ImGuiWindowFlags_NoBackground;

				
				if (!opt_padding)
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				
				if (open) {
					if (!ImGui::Begin("###MyDockspace", &open, window_flags)) 
						open = false;

					if (!opt_padding)
						ImGui::PopStyleVar();

					if (opt_fullscreen) {
						ImGui::PopStyleVar(2);
					}

					// Submit the DockSpace
					ImGuiIO& io = ImGui::GetIO();
					if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
					{
						ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
						ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
					}
		
					ImGui::End();
				}




				// * * * * * * * * * * * * 
				// ------------------------
				// >> TEST WINDOW    
				// ------------------------
				// * * * * * * * * * * * * 
				bool show = true;
				
				if (show) {
					if (!ImGui::Begin("Test", &show)) {
						show = false;
					}
					//ImGui::Text("%s", "Hello World");
					
					ImTextureID frameBufferID = Renderer::GetAPI()->GetFrameBuffers()[0];

					ImGui::Image(frameBufferID, {128, 128});
					ImGui::End();
				}
				


			}
			RenderCommand::EndGUIRenderPass();
			RenderCommand::EndSingleCommand();


			// DRAW FRAME
			RenderCommand::SwapBuffers();
		}


	}
	void Application::Cleanup()
	{
	}
	void Application::OnFrameBufferResized(int width, int height)
	{
		RenderCommand::RecreateSwapChain();
	}
}