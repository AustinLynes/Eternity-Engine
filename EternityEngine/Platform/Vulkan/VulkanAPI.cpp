#include "VulkanAPI.h"
#include "src/Graphics/Renderer.h"
#include <Dependencies/imgui/imgui.h>
#include <Dependencies/imgui/imgui_impl_glfw.h>
#include <Dependencies/imgui/imgui_impl_vulkan.h>
namespace EternityEngine {

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};


#pragma region VALIDATION

	/*
	<msgSeverity> parameter can have the following flags:

		• VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:			Diagnostic message
		• VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:				Informational message like the creation of a resource
		• VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:			Message about behavior that is not necessarily an error, but very likely a bug in your application
		• VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:			Message about behavior that is invalid and may cause crashes

	<msgType> parameter can have the following values:

			• VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:			Some event has happened that is unrelated to the specification or performance
			• VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:		Something has happened that violates the specification or indicates a possible mistake
			• VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:		Potential nonoptimal use of Vulkan

	*/
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanAPI::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

		if (msgSeverity & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE; // ALWAYS RETURN VK_FALSE
	}

	void VulkanAPI::BindVertexBuffer()
	{
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			VkBuffer vertexBuffers[]{ m_VAOs[m_curVAO].vertexBuffer };
			VkDeviceSize offsets[]{ 0 };
			vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, vertexBuffers, offsets);
		}
	}

	void VulkanAPI::BindIndexBuffer()
	{
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			vkCmdBindIndexBuffer(m_commandBuffers[i], m_VAOs[m_curVAO].indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		}
	}

	void VulkanAPI::BeginSingleCommand()
	{
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Could Not Start a Buffer Recording");
			}
		}
	}

	void VulkanAPI::EndSingleCommand()
	{
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("COULD NOT DISPATCH COMMAND");
			}
		}
	}

	void VulkanAPI::BeginRenderPass(float r, float g, float b, float a)
	{


		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
	
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_renderPass;
			renderPassInfo.framebuffer = m_framebuffers[i];
			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = swapChainExtent;



			VkClearValue clearColor = { r, g, b, a };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			// RENDER PASS
			vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
			

			
		}

	}

	void VulkanAPI::EndRenderPass()
	{
		

		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{

			
			vkCmdEndRenderPass(m_commandBuffers[i]);

		}

	}

	void VulkanAPI::BindUniformBuffer()
	{
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		}

	}

	void VulkanAPI::DrawIndexed(uint32_t count)
	{
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			vkCmdDrawIndexed(m_commandBuffers[i], count, 1, 0, 0, 0);//static_cast<uint32_t>(indices.size())
		}
	}

	void VulkanAPI::CreateUniformBuffer()
	{
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
	}

	void VulkanAPI::BeginGUIRenderPass()
	{
		// Start the Dear ImGui frame
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();

		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_imGuiRenderPass;
			renderPassInfo.framebuffer = m_framebuffers[i];
			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = swapChainExtent;



			VkClearValue clearColor = { 0, 0, 0, 0 };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			// RENDER PASS
			vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		}
		

	}

	void VulkanAPI::EndGUIRenderPass()
	{
		ImGui::Render();

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		for (size_t i = 0; i < m_commandBuffers.size(); i++)
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffers[i]);
		
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			vkCmdEndRenderPass(m_commandBuffers[i]);

		}
	}


#pragma endregion


	VulkanAPI::VulkanAPI(Window* pWindow) : m_window{pWindow}
	{
		//m_VAOs.resize(0); // give enough space to create 50 seperate VAOS before resizing again

		// STEP 1
		// create instance
		CreateInstance();
		// enable debugging
		SetupDebugMessenger();

		// create Surface to render to
		CreateSurface();

		// create device
		PickPhysicalDevice();

		// STEP 2 
		// create logical device
		CreateLogicalDevice();


		CreateSwapChain();
		CreateImageViews();



		
	
		CreateSyncObjects();




		// -------------------------------------------------------------------------------------------------------------
		// TO BE REFACTORED INTO 
		//	L BeginCommand()
		//		: 
		//	L EndCommand()
		//		:
		//	L BeginRenderPass()
		//		:
		//	L EndRenderPass()
		//		:
		//	L ClearColor()
		//	L BindVertexBuffer()
		//	L BindIndexBuffer()
		//	L Draw<...>
		//		-> Array
		//		-> Indexed
		//		-> IndexedInstanced
		// 
		// 
		//		BEGIN COMMAND:
		//			BEGIN RENDER PASS:
		//					BIND PIPELINE
		//					CLEAR FRAMEBUFFER
		//					BIND VERTEX BUFFER
		//					BIND INDEX BUFFER
		//					BIND DESCRIPTORS 
		//					DRAW
		//			END RENDER PASS
		//		END COMMAND
		// 
		// -------------------------------------------------------------------------------------------------------------
		
		//CreateVertexArrayObject();
		//CreateVertexBuffer((void*)quad_vertices.data(), sizeof(quad_vertices[0]) * quad_vertices.size());
		//CreateIndexBuffer((void*)indices.data(), sizeof(indices[0]) * indices.size());
		//CreateUniformBuffer();

		//BeginSingleCommand();
		//BeginRenderPass(1,0,0,0);
		//
		//BindVertexArrayObject(0);
		//BindVertexBuffer();
		//BindIndexBuffer();
		//BindUniformBuffer();
		//DrawIndexed();

		//EndRenderPass();
		//EndSingleCommand();






		CreateRenderPass();
		CreateImGUIRenderPass();

		CreateDescriptorLayout();
		//CreateGraphicsPipeline();
		CreateFrameBuffers();


		CreateCommandPool();
		CreateCommandBuffers();


		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		{
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
			pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;
			vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_ImGUIPool);
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(Window::GetGLFWwindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_vkInstance;
		init_info.PhysicalDevice = m_physicalDevice;
		init_info.Device = m_device;
		init_info.QueueFamily = 0;
		init_info.Queue = m_ImGUIQueue;
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = m_ImGUIPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = m_framebuffers.size();
		init_info.ImageCount = m_framebuffers.size();
		init_info.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&init_info, m_imGuiRenderPass);

		//float queuePriority = 1.0f;

		//VkDeviceQueueCreateInfo queueCreateinfo{};
		//queueCreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		//queueCreateinfo.queueFamilyIndex = 0;
		//queueCreateinfo.queueCount = 1;
		//queueCreateinfo.pQueuePriorities = &queuePriority;
		//queueInfoVector.push_back(queueCreateinfo);
		
		// Upload Fonts
		{
			// Use any command queue
			//VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
			//VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

			vkResetCommandPool(m_device, m_commandPool, 0);
			//check_vk_result(err);
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			for (size_t i = 0; i < swapChainImages.size(); i++)
			{
			vkBeginCommandBuffer(m_commandBuffers[i], &begin_info);
			//check_vk_result(err);

			ImGui_ImplVulkan_CreateFontsTexture(m_commandBuffers[i]);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &m_commandBuffers[i];
			vkEndCommandBuffer(m_commandBuffers[i]);
			//check_vk_result(err);
			vkQueueSubmit(m_ImGUIQueue, 1, &end_info, VK_NULL_HANDLE);
			//check_vk_result(err);

			vkDeviceWaitIdle(m_device);
			//check_vk_result(err);
			ImGui_ImplVulkan_DestroyFontUploadObjects();

			}

		}




	}

	VulkanAPI::~VulkanAPI()
	{
		CleanupSwapchain();


		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			vkDestroyBuffer(m_device, uniformBuffers[i], nullptr);
			vkFreeMemory(m_device, uniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);


		vkDestroyDescriptorSetLayout(m_device, m_descriptorSetlayout, nullptr);

		vkDestroyBuffer(m_device, m_VAOs[m_curVAO].indexBuffer, nullptr);
		vkFreeMemory(m_device, m_VAOs[m_curVAO].indexBufferMemory, nullptr);
		vkDestroyBuffer(m_device, m_VAOs[m_curVAO].vertexBuffer, nullptr);
		vkFreeMemory(m_device, m_VAOs[m_curVAO].vertexBufferMemory, nullptr);
		//delete m_VAOs;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_device, imageAvalailableSemaphores[i], nullptr);
			vkDestroySemaphore(m_device, renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(m_device, inFlightFences[i], nullptr);

		}

		vkDestroyCommandPool(m_device, m_commandPool, nullptr);


		vkDestroyDevice(m_device, nullptr);

		if (enableValidation) {
			DestroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
		// destroy the instance
		vkDestroyInstance(m_vkInstance, nullptr);
	}



	void  VulkanAPI::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		// specify that we want all messages to come through
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	void  VulkanAPI::SetupDebugMessenger() {
		if (!enableValidation) return;
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Set up Debug Messenger.");
		}
	}

	bool VulkanAPI::CheckValidationSupport() {
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> layerProps(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data());

		bool layer_found = false;
		for (size_t i = 0; i < validationLayers.size(); i++)
		{
			if (strcmp(validationLayers[i], layerProps[i].layerName)) {
				layer_found = true;
				break;
			}
		}

		if (!layer_found)
			return false;

		return true;
	}



	// the first method that needs to be called to initilize vulkan
	void  VulkanAPI::CreateInstance() {

		if (enableValidation && !CheckValidationSupport()) {
			throw std::runtime_error("Validation Support Request sent but not returned");
		}

		// describe the application and engine
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Vulkan Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// describe global extensions and validation layers
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		if (enableValidation) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
			createInfo.enabledLayerCount = 0;


		auto ext = GetRequiredExtensions();
		// assign the number of extensions to enable
		createInfo.enabledExtensionCount = static_cast<uint32_t>(ext.size());
		// assign the extension names to be enabled
		createInfo.ppEnabledExtensionNames = ext.data();
		if (!EnumerateExtensions()) {
			throw std::runtime_error("Could Not Verify Extensions");
		}

		if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS) {
			throw std::runtime_error("Could Not Create Vulkan Instance");
		}

	}

	bool  VulkanAPI::EnumerateExtensions() {
		// number of extions avalable for use with vulkan
		uint32_t numExtensions = 0;
		//request the number of extensions from vulkan
		vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, nullptr);
		// create a vector to store the extension propreties of each extenion
		std::vector<VkExtensionProperties> extensions(numExtensions);
		// request the extension data
		vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, extensions.data());
		// print all avalable extensions and their version
		/*std::cout << "Avalable Extensions" << std::endl;
		for (const auto& extension : extensions)
		{
			std::cout << extension.extensionName << "\t" << extension.specVersion << std::endl;
		}*/

		// enable layer 0
		return true;
	}
	std::vector<const char*>  VulkanAPI::GetRequiredExtensions() {
		uint32_t numExtensions = 0;
		// create a pointer to strings to hold the REQUIRED extensions for creating a vulkan instance
		const char** glfwExtensions;

		// request the required extensions from GLFWs configured function
		glfwExtensions = glfwGetRequiredInstanceExtensions(&numExtensions);
		//std::cout << "required extensions" << std::endl;
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + numExtensions);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		//for (size_t i = 0; i < numExtensions; i++)
		//{
		//	std::cout << glfwExtensions[i] << std::endl;
		//}

		return extensions;
	}

	void  VulkanAPI::PickPhysicalDevice() {
		// the number of devices found
		uint32_t numDevices = 0;
		// request nubmer of the devices 
		vkEnumeratePhysicalDevices(m_vkInstance, &numDevices, nullptr);
		// if there are no supported vulkan devices 
		// break
		if (numDevices == 0)
			throw std::runtime_error("NO VULKAN SUPPORTED HARDWARE... WHOMP WHOMP..");

		// create a vector of devices 
		std::vector<VkPhysicalDevice> devices(numDevices);
		// request the devices description
		vkEnumeratePhysicalDevices(m_vkInstance, &numDevices, devices.data());
		for (auto device : devices)
		{
			if (isDeviceSuitable(device)) {
				m_physicalDevice = device;
				break;
			}
		}
		if (m_physicalDevice == nullptr) {
			throw std::runtime_error("FAILED TO FIND SUITABLE GPU");
		}

	}

	bool  VulkanAPI::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		// enumerate all avalable extensions
		uint32_t extensionCount;
		// save count ^^
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		// create array to hold avalable extensions
		std::vector<VkExtensionProperties> avalableExtensions(extensionCount);
		// read all extensions into vector
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, avalableExtensions.data());

		// create a set to hold unique required extensions in case we make a mistake 
		std::set<std::string> requiredEXT(deviceExtensions.begin(), deviceExtensions.end());

		// for each extension in the avalable extensions
		// remove from the required the extension found with 
		// the current name
		for (const auto& ext : avalableExtensions)
		{
			requiredEXT.erase(ext.extensionName);
		}

		// if the required extensions are empty.. all extensions where found
		// there for device is supported	--- TRUE
		// if there are any extensions that where not added then the 
		// device is not supported			--- FALSE
		return requiredEXT.empty();
	}

	bool  VulkanAPI::isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndicies indices = FindQueueFamilies(device);
		bool extensionSupported = CheckDeviceExtensionSupport(device);
		bool swapChainAdequate = false;
		if (extensionSupported) {
			SwapChainSupportDetails deets = QuerySwapChainSupport(device);
			swapChainAdequate = !deets.formats.empty() && !deets.presentModes.empty();
		}
		return indices.isComplete() && extensionSupported && swapChainAdequate;
	}


	QueueFamilyIndicies VulkanAPI::FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndicies indices;
		// assign queue index to queue families that could be found
		uint32_t propsCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &propsCount, nullptr);
		std::vector<VkQueueFamilyProperties> properties(propsCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &propsCount, properties.data());
		int i = 0;
		for (const auto& queue : properties)
		{
			if (indices.isComplete())
				break;

			if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
			if (presentSupport)
				indices.presentFamily = i;


			i++;
		}

		return indices;
	}



	void  VulkanAPI::CreateLogicalDevice() {
		QueueFamilyIndicies indices = FindQueueFamilies(m_physicalDevice);
		std::vector<VkDeviceQueueCreateInfo> queueInfoVector;

		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		float queuePriority = 1.0f;
		for (auto queue : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateinfo{};
			queueCreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateinfo.queueFamilyIndex = indices.graphicsFamily.value();
			queueCreateinfo.queueCount = 1;
			queueCreateinfo.pQueuePriorities = &queuePriority;
			queueInfoVector.push_back(queueCreateinfo);
		}


		// define features
		VkPhysicalDeviceFeatures devFeatures{};

		// create logical device
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueInfoVector.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfoVector.size());


		createInfo.pEnabledFeatures = &devFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidation) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			throw std::runtime_error("Could Not Create Logical Device");
		}

		vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_ImGUIQueue);

	}

	void VulkanAPI::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("VERTEX BUFFER REQURIED TO RENDER OBJECTS");
		}

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(m_device, buffer, &memReqs);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReqs.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memReqs.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("Could not allocate buffer memory");
		}

		vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
	}

	void VulkanAPI::CreateSurface() {
		if (glfwCreateWindowSurface(m_vkInstance, m_window->GetGLFWwindow(), nullptr, &m_surface) != VK_SUCCESS) {
			throw std::runtime_error("could not create GLFW surface");
		}
	}

	VkSurfaceFormatKHR  VulkanAPI::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avalFormats) {
		for (const auto& format : avalFormats)
		{
			if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return format;
			}
		}
		// if non are found by my specifications 
		// just return the first one in the list..
		return avalFormats[0];
	}

	VkPresentModeKHR  VulkanAPI::ChooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& avalModes) {
		for (const auto& mode : avalModes)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return mode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	SwapChainSupportDetails  VulkanAPI::QuerySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

		// queury surface formats
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());

		uint32_t presentationModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentationModeCount, nullptr);

		if (presentationModeCount > 0) {
			details.presentModes.resize(presentationModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentationModeCount, details.presentModes.data());

		}

		return details;
	}

	VkExtent2D  VulkanAPI::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capibilitiess) {
		if (capibilitiess.currentExtent.width != UINT32_MAX) {
			return capibilitiess.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(m_window->GetGLFWwindow(), &width, &height);

			VkExtent2D actual{
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};
			actual.width = std::max(capibilitiess.minImageExtent.width, std::min(capibilitiess.maxImageExtent.width, actual.width));
			actual.height = std::max(capibilitiess.minImageExtent.height, std::min(capibilitiess.maxImageExtent.height, actual.height));
			return actual;
		}
	}

	void  VulkanAPI::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_physicalDevice);
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentationMode = ChooseSwapChainPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t swapChain_imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && swapChain_imageCount > swapChainSupport.capabilities.maxImageCount) {
			swapChain_imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;
		createInfo.minImageCount = swapChain_imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndicies indices = FindQueueFamilies(m_physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;

		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentationMode;
		//createInfo.clipped = VK_TRUE;
		//createInfo.oldSwapchain = m_swapchain;

		vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain);
		//throw std::runtime_error("Could Not Initilize SwapChain");
		//}

		vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapChain_imageCount, nullptr);
		swapChainImages.resize(swapChain_imageCount);
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapChain_imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void VulkanAPI::RecreateSwapchain() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_window->GetGLFWwindow(), &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_window->GetGLFWwindow(), &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_device);

		CleanupSwapchain();

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		//CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();

		CreateCommandBuffers();

		inFlightImages.resize(swapChainImages.size(), VK_NULL_HANDLE);
	}

	void VulkanAPI::CleanupSwapchain() {

		// Destroy all framebuffers
		for (auto buffer : m_framebuffers) {
			vkDestroyFramebuffer(m_device, buffer, nullptr);
		}

		// Free Command Buffers for Reuse
		vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());

		// Destroy The Graphics Pipeline
		vkDestroyPipeline(m_device, m_pipeline, nullptr);

		// Destroy the Pipeline Layout
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

		// Destroy the Current RenderPass
		vkDestroyRenderPass(m_device, m_renderPass, nullptr);
		vkDestroyRenderPass(m_device, m_imGuiRenderPass, nullptr);

		// Destroy all Image Views
		for (auto iv : m_swapChainImageViews) {
			vkDestroyImageView(m_device, iv, nullptr);
		}

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

	}

	void VulkanAPI::CreateImageViews() {
		m_swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create image views");
			}
		}




	}

	void VulkanAPI::CreateDescriptorLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetlayout) != VK_SUCCESS) {
			throw std::runtime_error("COULD NOT CREATE DESCRIPTOR LAYOUT");
		}
	}


	void VulkanAPI::CreateRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;


		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ;


		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &colorAttachment;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		// stage to wait on in which these operations occour
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		createInfo.dependencyCount = 1;
		createInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_device, &createInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("COULD NOT CREATE RENDER PASS");
		}
		




	}

	void VulkanAPI::CreateImGUIRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;


		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &colorAttachment;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;
		createInfo.dependencyCount = 1;
		createInfo.pDependencies = &dependency;


		if (vkCreateRenderPass(m_device, &createInfo, nullptr, &m_imGuiRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("COULD NOT CREATE RENDER PASS");
		}
	}
	
	//	+--- NO VAO IS BOUND
	//	|
	// {0} {vData, iData, UbufData } {vData, iData, UbufData } {vData, iData, UbufData }
	void VulkanAPI::CreateVertexArrayObject()
	{
		m_VAOs.push_back({ 0 });
		m_curVAO;
	}

	void VulkanAPI::BindVertexArrayObject(uint32_t position)
	{
		m_curVAO = position;
	}

	void VulkanAPI::CreateVertexBuffer(void* vertexData, uint64_t size) {
		
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(m_device, stagingBufferMemory, 0, size, 0, &data);
		memcpy(data, vertexData, (uint32_t)size);
		vkUnmapMemory(m_device, stagingBufferMemory);

		CreateBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VAOs[m_curVAO].vertexBuffer,
			m_VAOs[m_curVAO].vertexBufferMemory
		);

		CopyBuffer(stagingBuffer, m_VAOs[m_curVAO].vertexBuffer, size);

		vkDestroyBuffer(m_device, stagingBuffer, nullptr);
		vkFreeMemory(m_device, stagingBufferMemory, nullptr);

	}

	void VulkanAPI::CreateIndexBuffer(void* indexData, uint64_t size)
	{
		//VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);
		// copy data to GPU
		void* data;
		vkMapMemory(m_device, stagingBufferMemory, 0, size, 0, &data);
		memcpy(data, indexData, (uint32_t)size);
		vkUnmapMemory(m_device, stagingBufferMemory);

		CreateBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VAOs[m_curVAO].indexBuffer,
			m_VAOs[m_curVAO].indexBufferMemory
		);

		CopyBuffer(stagingBuffer, m_VAOs[m_curVAO].indexBuffer, size);

		vkDestroyBuffer(m_device, stagingBuffer, nullptr);
		vkFreeMemory(m_device, stagingBufferMemory, nullptr);
	}

	void VulkanAPI::CreateUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		uniformBuffers.resize(swapChainImages.size());
		uniformBuffersMemory.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
		}
	}
	
	void VulkanAPI::CreateDescriptorPool() {
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

		if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("COULD NOT CREATE DESCRIPTOR POOL");
		}
	}

	void VulkanAPI::CreateDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), m_descriptorSetlayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
		allocInfo.pSetLayouts = layouts.data();
		descriptorSets.resize(swapChainImages.size());
		if (vkAllocateDescriptorSets(m_device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO ALLOCATE DESCRIPTOR SETS");
		}
		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;


			vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);

		}
	}

	void VulkanAPI::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);
		// record the copy command 
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		// submit the copy command
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphicsQueue);

		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
	}

	
	void VulkanAPI::CreateGraphicsPipeline(VkVertexInputBindingDescription bindingDescriptor, VkVertexInputAttributeDescription* attribDescriptors, size_t attrs_count) {

		auto vertexShaderByteCode = ReadFile("Assets/Shaders/Basic/basic_vert.spv");
		auto fragmentShaderByteCode = ReadFile("Assets/Shaders/Basic/basic_frag.spv");

		auto vertexModule = CreateShaderModule(vertexShaderByteCode);
		auto fragModule = CreateShaderModule(fragmentShaderByteCode);

		// VERTEX SHADER
		VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
		vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageInfo.module = vertexModule;
		vertexShaderStageInfo.pName = "main";

		// FRAGMENT SHADER
		VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStageInfo.module = fragModule;
		fragmentShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {
			vertexShaderStageInfo,
			fragmentShaderStageInfo
		};

		// VERTEX INPUT
		// describes the layout of the vertex buffer
		// and describes the attributes that are being passed in

		//VkVertexInputBindingDescription bindingDescription{};
		//bindingDescription.binding = 0;
		//bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		//bindingDescription.stride = sizeof(Vertex);


		/*VkVertexInputAttributeDescription positionAttr{};
		positionAttr.binding = 0;
		positionAttr.location = 0;
		positionAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionAttr.offset = offsetof(Vertex, position);

		VkVertexInputAttributeDescription colorAttr{};
		colorAttr.binding = 0;
		colorAttr.location = 1;
		colorAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
		colorAttr.offset = offsetof(Vertex, color);*/

		//std::vector<VkVertexInputAttributeDescription> attributeDescriptions{positionAttr, colorAttr};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptor;
		vertexInputInfo.vertexAttributeDescriptionCount = attrs_count;
		vertexInputInfo.pVertexAttributeDescriptions = attribDescriptors;

		// INPUT ASSEMBLY
		// Input Assembly describes how and what kind of primtive will be drawn from the vertices 
		// and if primitive restart should be enabled
		VkPipelineInputAssemblyStateCreateInfo IA_info{};
		IA_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		IA_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		IA_info.primitiveRestartEnable = VK_FALSE;



		// Viewport State 
		// Configure a viewport 
		// Configure a Scissor Rect
		// Combine them into VieportState

		// Viewport
		// A viewport basically describes the region of the framebuffer that the output will
		// be rendered to.
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// Scissor Rect
		// While viewports define the transformation from the image to the framebuffer,
		// scissor rectangles define in which regions pixels will actually be stored.Any
		// pixels outside the scissor rectangles will be discarded by the rasterizer.They
		// function like a filter rather than a transformation
		VkRect2D scissorRect{};
		scissorRect.offset = { 0, 0 };
		scissorRect.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissorRect;

		// Rasterizer State (RS)
		//The rasterizer takes the geometry that is shaped by the vertices from the
		//vertex shaderand turns it into fragments to be colored by the fragment
		//shader.It also performs depth testing, face cullingand the scissor test,
		//and it can be configured to output fragments that fill entire polygons
		//or just the edges(wireframe rendering)
		VkPipelineRasterizationStateCreateInfo rasterizerState{};
		rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		//If depthClampEnable is set to VK_TRUE, then fragments that are beyond the
		//near and far planes are clamped to them as opposed to discarding them.This
		//is useful in some special cases like shadow maps.
		rasterizerState.depthClampEnable = VK_FALSE;
		// If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes
		// through the rasterizer stage.
		rasterizerState.rasterizerDiscardEnable = VK_FALSE;

		//The polygonMode determines how fragments are generated for geometry.The
		//	following modes are available :
		//	• VK_POLYGON_MODE_FILL : fill the area of the polygon with fragments
		//	• VK_POLYGON_MODE_LINE : polygon edges are drawn as lines
		//	• VK_POLYGON_MODE_POINT : polygon vertices are drawn as points
		// **modes other than fill require an extra GPU feature
		rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerState.lineWidth = 1.0f;
		// culling should almost always be set to culling back faces, with an exception skyboxes are rendererd inside out.
		// cull the back of faces, back face is determined by the front face wrap mode determined subsequently
		rasterizerState.cullMode = VK_CULL_MODE_NONE;
		rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerState.depthBiasEnable = VK_FALSE;
		rasterizerState.depthBiasConstantFactor = 1.0f;
		rasterizerState.depthBiasClamp = 0.0f;
		rasterizerState.depthBiasSlopeFactor = 1.0f;

		// MultiSampling 
		// It works by combining
		// the fragment shader results of multiple polygons that rasterize to the same
		// pixel.This mainly occurs along edges, which is also where the most noticeable
		// aliasing artifacts occur.Because it doesn’t need to run the fragment shader
		// multiple times if only one polygon maps to a pixel, it is significantly less
		// expensive than simply rendering to a higher resolutionand then downscaling.
		// ** Enabling it requires enabling a GPU feature.
		VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingInfo.minSampleShading = 1.0f;
		multisamplingInfo.pSampleMask = nullptr;
		multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
		multisamplingInfo.alphaToOneEnable = VK_FALSE;

		// COLOR BLENDING 
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;

		colorBlendAttachment.blendEnable = VK_TRUE;
		// color blending 
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		// alpha blending
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_descriptorSetlayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Could Not Create Pipeline Layout");
		}


		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2; // vertex, fragment 
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &IA_info;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizerState;
		pipelineInfo.pMultisampleState = &multisamplingInfo;
		pipelineInfo.pDepthStencilState = nullptr;		// to be configured later
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;			// to be configured later
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;


		if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("COULD NOT CREATE GRAPHICS PIPELINE");
		}

		vkDestroyShaderModule(m_device, vertexModule, nullptr);
		vkDestroyShaderModule(m_device, fragModule, nullptr);
	}


	uint32_t VulkanAPI::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		// query info about the avalable types of memory
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProps);

		for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
		{
			if (IS_AT_BIT(typeFilter, i) && BITS_MATCH(memProps.memoryTypes[i].propertyFlags, properties)) {
				return i;
			}
		}
		throw std::runtime_error("Failed to find Suitable Memory");

	}

	VkShaderModule VulkanAPI::CreateShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Could not create shader module");
		}
		return shaderModule;
	}


	void VulkanAPI::CreateFrameBuffers() {
		m_framebuffers.resize(m_swapChainImageViews.size());

		for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = { m_swapChainImageViews[i] };

			VkFramebufferCreateInfo frameBufferInfo{};
			frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferInfo.renderPass = m_renderPass;
			frameBufferInfo.attachmentCount = 1;
			frameBufferInfo.pAttachments = attachments;
			frameBufferInfo.width = swapChainExtent.width;
			frameBufferInfo.height = swapChainExtent.height;
			frameBufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device, &frameBufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("FAILED TO CREATE A FRAME BUFFER FOR SWAP CHAIN");
			}

		}
	}

	void VulkanAPI::CreateCommandPool() {
		QueueFamilyIndicies indices = FindQueueFamilies(m_physicalDevice);
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
		poolInfo.flags = 0;
		if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("Could not allocate command pool");
		}
	}

	void VulkanAPI::CreateCommandBuffers() {
		m_commandBuffers.resize(m_framebuffers.size());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();
		if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Could Not allocate command Buffers");
		}

	}




	void VulkanAPI::CreateSyncObjects()
	{
		imageAvalailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightImages.resize(swapChainImages.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(m_device, &createInfo, nullptr, &imageAvalailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_device, &createInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS
				) {
				throw std::runtime_error("Could Not Create Syncronization Objects for a Frame");
			}
		}


	}
	
	void VulkanAPI::SwapBuffers()
	{
		vkWaitForFences(m_device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		// aquiring the image
		uint32_t imageInd;
		VkResult res = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, imageAvalailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageInd);

		if (res == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapchain();
			return;
		}
		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to aquire swap chain image");
		}

		// check if a previous frame is using this image
		// there is no fence to wait on...
		if (inFlightImages[imageInd] != VK_NULL_HANDLE) {
			vkWaitForFences(m_device, 1, &inFlightImages[imageInd], VK_TRUE, UINT64_MAX);
		}
		inFlightImages[imageInd] = inFlightFences[currentFrame];

		UpdateUniformBuffer(imageInd);

		// submitting the command buffer
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitForSemaphores[] = { imageAvalailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitForStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		// specify how many semaphores for this 
		submitInfo.waitSemaphoreCount = 1;
		// which semaphore[s] to wait for
		submitInfo.pWaitSemaphores = waitForSemaphores;
		// which stages to wait for
		submitInfo.pWaitDstStageMask = waitForStages;

		// bind the command buffer that aquired the swapchain color attachment
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[imageInd];

		// specifies the rendereFinishedSemaphore is to be signaled once this command buffer has 
		// finished execution
		VkSemaphore signalSemaphores[]{ renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(m_device, 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame])) {
			throw std::runtime_error("FAILED TO SUBMIT DRAW COMMAND BUFFER");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[]{ m_swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageInd;

		res = vkQueuePresentKHR(m_graphicsQueue, &presentInfo);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
			framebufferResized = false;
			RecreateSwapchain();
		}
		else if (res != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}

		// advance to the next frame
		//  By using the modulo (%) operator, we ensure that the frame index loops around
		//	after every MAX_FRAMES_IN_FLIGHT enqueued frames.

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	void VulkanAPI::UpdateUniformBuffer(uint32_t currentImage)
	{

		// update the quad ubo  
		// make it spin in a circle around the y axis
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
		start_time = current_time;

		static float cam_pos[]{ 0, 0, -3 };
		float cam_moveSpeed = 1.0f;
		if (glfwGetKey(m_window->GetGLFWwindow(), GLFW_KEY_W)) {
			cam_pos[2] += cam_moveSpeed * deltaTime;
		}
		else if (glfwGetKey(m_window->GetGLFWwindow(), GLFW_KEY_S)) {
			cam_pos[2] -= cam_moveSpeed * deltaTime;
		}
		else if (glfwGetKey(m_window->GetGLFWwindow(), GLFW_KEY_D)) {
			cam_pos[0] -= cam_moveSpeed * deltaTime;
		}
		else if (glfwGetKey(m_window->GetGLFWwindow(), GLFW_KEY_A)) {
			cam_pos[0] += cam_moveSpeed * deltaTime;
		}
		else if (glfwGetKey(m_window->GetGLFWwindow(), GLFW_KEY_E)) {
			cam_pos[1] -= cam_moveSpeed * deltaTime;
		}
		else if (glfwGetKey(m_window->GetGLFWwindow(), GLFW_KEY_Q)) {
			cam_pos[1] += cam_moveSpeed * deltaTime;
		}

		UniformBufferObject quad_ubo{};

		static float rot = 0.0f;
		rot += 90.0f * deltaTime;

		quad_ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(rot), glm::vec3(0, 1, 0)) ;
		quad_ubo.view = glm::mat4(1.0f);
		quad_ubo.view = glm::translate(quad_ubo.view, glm::vec3(cam_pos[0], cam_pos[1], cam_pos[2]));
		quad_ubo.view = glm::rotate(quad_ubo.view, glm::radians(180.0f), glm::vec3(0, 1, 0));
		quad_ubo.projection = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.0f);
		quad_ubo.projection[1][1] *= -1;

		void* data;
		vkMapMemory(m_device, uniformBuffersMemory[currentImage], 0, sizeof(quad_ubo), 0, &data);
		memcpy(data, &quad_ubo, sizeof(quad_ubo));
		vkUnmapMemory(m_device, uniformBuffersMemory[currentImage]);

	}
}