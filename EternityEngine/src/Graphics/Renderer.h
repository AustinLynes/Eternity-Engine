#pragma once

#include "RenderCommand.h"
#include "GLM/glm.hpp"

namespace EternityEngine {

	

	class Renderer
	{
	public:
		static void Initilize();
	
		static GraphicsAPI* GetAPI() { return RenderCommand::GetAPI(); }


		// WORK UP TO THIS...
		static void DrawQuad();

	};


}