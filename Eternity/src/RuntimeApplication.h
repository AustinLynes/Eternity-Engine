#pragma once

#include "Application.h"

namespace EternityEngine {

	class RuntimeApplication : public Application
	{
	public:
		RuntimeApplication(){
			RenderCommand::SetTargetSpecification(API::Specification::Vulkan);
		}
		~RuntimeApplication(){}

	private:
	};

	Application* CreateApplication() {
		return new RuntimeApplication();
	}


}
