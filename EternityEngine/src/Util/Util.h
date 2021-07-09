#pragma once
#include <vector>
#include <fstream>

namespace EternityEngine {
	static std::vector<char> ReadFile(const std::string& filename) {
		std::ifstream in(filename, std::ios::ate | std::ios::binary);
		if (!in.is_open()) {
			std::cout << "Failed to Open File" << std::endl;
			throw std::runtime_error("failed to open file");
		}
		size_t fileSize = (size_t)in.tellg();
		std::vector<char> buffer(fileSize);
		in.seekg(0);
		in.read(buffer.data(), fileSize);
		//std::cout << filename << " " << buffer.size() << " bytes " << std::endl;
		in.close();
		return buffer;
	}


}