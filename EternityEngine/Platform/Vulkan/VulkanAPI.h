#pragma once
#include <cstdint>

#include "Window/Window.h"

// iostream
#include <iostream>
#include <fstream>
// exceptions

#include <stdexcept>
#include <vector>
#include <array>
#include <optional>
#include <set>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BIT(x) (1 << x)
#define IS_AT_BIT(check, bit) check & BIT(bit)
#define BITS_MATCH(check, bit) (check & bit) == bit
#include "Util/Util.h"


#include "Graphics/GraphicsAPI.h"


namespace EternityEngine {


	struct VertexArrayObject {
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

	};

	namespace {
		struct QueueFamilyIndicies {
			std::optional<uint32_t> graphicsFamily;// need to find a queue family with a pairing of both of these
			std::optional<uint32_t> presentFamily; //

			bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};


		const uint32_t HEIGHT = 600;
		const uint32_t WIDTH = 800;

		// connection to vulkan 
		VkInstance m_vkInstance;

		// vulkan debug layer messenger
		VkDebugUtilsMessengerEXT m_debugMessenger;

		// handle to the physical device chosen
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

		// handle to logical device
		VkDevice m_device = VK_NULL_HANDLE;

		VkQueue m_graphicsQueue;
		VkQueue m_presentationQueue;
		VkQueue m_ImGUIQueue;
		// WSI surface
		VkSurfaceKHR m_surface;
		bool framebufferResized = false;
		VkSwapchainKHR m_swapchain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;


		std::vector<VkImageView> m_swapChainImageViews;
		VkRenderPass m_renderPass;
		VkRenderPass m_imGuiRenderPass;
		VkDescriptorSetLayout m_descriptorSetlayout;
		VkPipelineLayout m_pipelineLayout;

		std::vector<VertexArrayObject> m_VAOs;
		int m_curVAO;

		VkDescriptorPool m_descriptorPool;
		VkDescriptorPool m_ImGUIPool;
		std::vector<VkDescriptorSet> descriptorSets;

		VkPipeline m_pipeline;
		std::vector<VkFramebuffer> m_framebuffers;

		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;

		// Semaphores
		// Used to signal that an image has been aquired and is ready for rendering
		// each frame should have its own image avalable semaphore
		std::vector<VkSemaphore> imageAvalailableSemaphores;
		// Used to signal that rendering is finished and presentation can happen
		// each frame should have its own render finished semaphore
		std::vector<VkSemaphore> renderFinishedSemaphores;

		//To perform CPU - GPU synchronization, Vulkan offers a second type of synchronization
		//primitive called fences.Fences are similar to semaphores in the sense
		//that they can be signaledand waited for, but this time we actually wait for
		//them in our own code

		const int MAX_FRAMES_IN_FLIGHT = 2;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> inFlightImages;

		size_t currentFrame = 0;



		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
			auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (fn != nullptr)
				return fn(instance, pCreateInfo, pAllocator, pDebugMessenger);
			else
				return VK_ERROR_EXTENSION_NOT_PRESENT;

		}

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
			auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (fn != nullptr) {
				fn(instance, debugMessenger, pAllocator);
			}
		}
	}


	class VulkanAPI : public GraphicsAPI
	{
	public:
		VulkanAPI(Window* pWindow);
		~VulkanAPI();



	private:
		virtual void CreateVertexArrayObject() override;
		virtual void BindVertexArrayObject(uint32_t position) override;

		virtual void CreateVertexBuffer(void* vertexData, uint64_t size) override;
		virtual void CreateIndexBuffer(void* indexData, uint64_t size) override;

		//bool CreateWindow() {
		//	// initilize GLFW 
		//	glfwInit();
		//	// tell GLFW not to create a context, because it was designed for openGL and
		//	// automatically  creates an openGL context unless explicilty told
		//	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//	// remove resizing 
		//	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		//	// create the window pointer
		//	m_window = glfwCreateWindow(WIDTH, HEIGHT, "LearnVulkan", nullptr, nullptr);
		//	// if the window pointer is nullptr
		//	if (m_window == nullptr) {
		//		std::cout << "window is Nullptr" << std::endl;
		//		return false;
		//	}
		//	glfwSetWindowUserPointer(m_window, this);
		//	glfwSetFramebufferSizeCallback(m_window, frameBuffercallback);
		//	return true;
		//}

		void CreateInstance();

		bool EnumerateExtensions();
		std::vector<const char*> GetRequiredExtensions();
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		void PickPhysicalDevice();

		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		bool isDeviceSuitable(VkPhysicalDevice device);

		virtual std::vector<VkFramebuffer> GetFrameBuffers() override { return m_framebuffers; };


		QueueFamilyIndicies FindQueueFamilies(VkPhysicalDevice device);



		void CreateLogicalDevice();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);



		void CreateSurface();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avalFormats);

		VkPresentModeKHR ChooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& avalModes);


		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capibilitiess);

		void CreateSwapChain();

		virtual void RecreateSwapchain() override;

		void CleanupSwapchain();

		void CreateImageViews();

		void CreateDescriptorLayout();


		void CreateRenderPass();
		void CreateImGUIRenderPass();

		

		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		// describes a graphics pipeline
		virtual void CreateGraphicsPipeline(VkVertexInputBindingDescription bindingDescriptor, VkVertexInputAttributeDescription* attribDescriptors, size_t attrs_count) override;


		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkShaderModule CreateShaderModule(const std::vector<char>& code);


		void CreateFrameBuffers();

		void CreateCommandPool();

		void CreateCommandBuffers();

		void CreateSyncObjects();

		/*
		Aquire an image from swap chain
			L Execute the command buffer with that image as attachment in the frame buffer
				L return image to the swap chain for presentation
		*/
		virtual void SwapBuffers();

		void UpdateUniformBuffer(uint32_t currentImage);

#pragma region VALIDATION
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void SetupDebugMessenger();

		bool CheckValidationSupport();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
		};

#ifdef _DEBUG
		const bool enableValidation = true;
#else
		const bool enableValidation = false;
#endif // _DEBUG
#pragma endregion

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;


		Window* m_window;

		virtual void BindVertexBuffer() override;
		virtual void BindIndexBuffer() override;
		virtual void BeginSingleCommand() override;
		virtual void EndSingleCommand() override;
		virtual void BeginRenderPass(float r, float g, float b, float a) override;
		virtual void EndRenderPass() override;
		virtual void BindUniformBuffer() override;
		virtual void DrawIndexed(uint32_t count) override;
		virtual void CreateUniformBuffer() override;
		virtual void BeginGUIRenderPass() override;
		virtual void EndGUIRenderPass() override;
};
}

