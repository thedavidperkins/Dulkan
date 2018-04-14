#include <fstream>
#include "DkShader.h"
#include "DkDevice.h"

DkShader::DkShader(DkDevice& device, std::string sourceFile, VkShaderStageFlagBits type) :
	m_device(device),
	m_sourceFile(sourceFile),
	m_stage(type),
	m_shader(VK_NULL_HANDLE),
	m_initialized(false),
	m_specInfo({})
{}

bool DkShader::init() {
	if (m_initialized) {
		finalize();
	}
	std::vector<char> source;
	if (!_loadSource(source)) return false;

	VkShaderModuleCreateInfo shaderInfo = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		(uint)source.size(),
		(uint*)source.data()
	};

	if (vkCreateShaderModule(m_device.get(), &shaderInfo, nullptr, &m_shader) != VK_SUCCESS || m_shader == VK_NULL_HANDLE) {
		std::cout << "Failed to create shader module." << std::endl;
		return false;
	}
	m_initialized = true;
	return true;
}

bool DkShader::_loadSource(std::vector<char>& sourceOut) {
	std::ifstream instrm(m_sourceFile, std::ios::binary);
	if (instrm.fail()) {
		std::cout << "Failed to open file: " << m_sourceFile << std::endl;
		return false;
	}
	std::streampos beg;
	std::streampos end;
	beg = instrm.tellg();
	instrm.seekg(0, std::ios::end);
	end = instrm.tellg();
	if (end - beg == 0) {
		std::cout << "File is empty: " << m_sourceFile << std::endl;
		return false;
	}
	instrm.seekg(0, std::ios::beg);
	sourceOut.resize((uint)(end - beg));
	instrm.read((char*)sourceOut.data(), end - beg);
	instrm.close();
	return true;
}

void DkShader::finalize() {
	m_device.waitIdle();
	if (m_shader != VK_NULL_HANDLE) {
		vkDestroyShaderModule(m_device.get(), m_shader, nullptr);
		m_shader = VK_NULL_HANDLE;
	}
	m_initialized = false;
}

void DkShader::getStageInfo(VkPipelineShaderStageCreateInfo& infoOut) {
	infoOut = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		m_stage,
		m_shader,
		"main",
		m_specInfo.mapEntryCount > 0 ? &m_specInfo : nullptr
	};
}