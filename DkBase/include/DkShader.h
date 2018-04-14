#ifndef DK_SHADER_H
#define DK_SHADER_H

#include <string>
#include "DkCommon.h"

class DkDevice;

/*
class DkShader : constructed with all needed parameters (reference to owning
	device, name of source file, and shader stage of the five possible. No
	additional customization necessary or possible.
	
	On a call to init(), the object will attempt to open the file at the given
	name and read it in as a shader, to then be packaged into a VkShaderModule
	handle.

	The input file is assumed to be a precompiled SPIR-V shader. No check is
	made to confirm that the supplied stage flag matches the actual shader
	type.
*/
class DkShader {
public:
	bool init();
	void finalize();

	// Getters
	VkShaderModule get() { return m_shader; }
	VkShaderStageFlagBits getStage() { return m_stage; }

	void getStageInfo(VkPipelineShaderStageCreateInfo& infoOut);

	DkShader(DkDevice& device, std::string sourceFile, VkShaderStageFlagBits type);
	~DkShader() { finalize(); }
	DkShader(const DkShader& rhs) = delete;
	DkShader& operator=(const DkShader& rhs) = delete;
private:
	// Helper functions
	bool _loadSource(std::vector<char>& sourceOut);

	// Set on construction
	DkDevice& m_device;
	std::string m_sourceFile;
	VkShaderStageFlagBits m_stage;

	// Set by init
	VkShaderModule m_shader;
	bool m_initialized;

	// Set before getInfo call -- set implementation pending
	VkSpecializationInfo m_specInfo;
};

#endif//DK_SHADER_H
