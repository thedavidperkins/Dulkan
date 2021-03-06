#include <chrono>
#include "DkSample_Cube_Lighting.h"
#include "DkAttachmentDescriptionBuilder.h"
#include "DkCommandBuffer.h"

using namespace math;
using namespace std::chrono;

static time_point<system_clock> initTime;

DkSample_Cube_Lighting::DkSample_Cube_Lighting() :
	DkApplication(),
	m_frameCount(3),
	m_swapchain(getDevice(), getWindow()),
	m_renderPass(getDevice()),
	m_frames(),
	m_pushUniformSemaphores(),
	m_pipeline(getDevice(), m_renderPass),
	m_cube(nullptr),
	m_descPool(getDevice()),
	m_descSet(nullptr),
	m_initialized(false),
	m_curFrame(0)
{}

void DkSample_Cube_Lighting::setFrameCount(uint count) {
	if (m_initialized) {
		std::cout << "Cannot set frame count after initialization." << std::endl;
		return;
	}
	m_frameCount = count;
}

bool DkSample_Cube_Lighting::init() {
	getWindow().setWindowRect({ { 0, 0 }, { 1080, 1080 } });
	if (!vulkanInit()) return false;

	// Init swapchain
	if (!m_swapchain.init()) return false;

	// Init frames -- the actual framebuffers will be created later, in the animation loop
	for (uint iter = 0; iter < m_frameCount; ++iter) {
		DkFrameResources* newFrame = new DkFrameResources(getDevice(), &m_swapchain, &m_renderPass, true);
		if (!newFrame->init()) return false;
		m_frames.push_back(newFrame);

		m_pushUniformSemaphores.push_back(new DkSemaphore(getDevice()));
		m_pushUniformSemaphores.back()->init();

		m_pushUniformNormalSemaphores.push_back(new DkSemaphore(getDevice()));
		m_pushUniformNormalSemaphores.back()->init();
	}
	if (!getCommandPool(DK_GRAPHICS_QUEUE)->allocate(m_frames)) return false;

	m_renderPass.addAttachmentDescription(DkAttachmentDescriptionBuilder::basicColorOutputAttachment());
	m_renderPass.addAttachmentDescription(DkAttachmentDescriptionBuilder::basicDepthAttachment());

	VkAttachmentReference attRef = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkAttachmentReference depthRef = {
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	m_renderPass.addSubpassDescription({
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&attRef,
		nullptr,
		&depthRef,
		0,
		nullptr
		});

	m_renderPass.addSubpassDependency({
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_DEPENDENCY_BY_REGION_BIT
		});

	m_renderPass.addSubpassDependency({
		0,
		VK_SUBPASS_EXTERNAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_DEPENDENCY_BY_REGION_BIT
		});

	if (!m_renderPass.init()) return false;

	if (!m_pipeline.addShader("shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT)) return false;
	if (!m_pipeline.addShader("shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)) return false;

	m_cube = new DkMesh(nullptr);

	vec4 trf( 1.f,  1.f, -1.f,  1.f);
	vec4 trb( 1.f,  1.f,  1.f,  1.f);
	vec4 tlf(-1.f,  1.f, -1.f,  1.f);
	vec4 tlb(-1.f,  1.f,  1.f,  1.f);
	vec4 brf( 1.f, -1.f, -1.f,  1.f);
	vec4 brb( 1.f, -1.f,  1.f,  1.f);
	vec4 blf(-1.f, -1.f, -1.f,  1.f);
	vec4 blb(-1.f, -1.f,  1.f,  1.f);

	vec4 topCol(1.f, 0.f, 0.f, 1.f); // red
	vec4 botCol(1.f, 1.f, 0.f, 1.f); // yellow
	vec4 rigCol(1.f, 0.f, 1.f, 1.f); // magenta
	vec4 lefCol(0.f, 1.f, 1.f, 1.f); // cyan
	vec4 bacCol(0.f, 1.f, 0.f, 1.f); // green
	vec4 froCol(0.f, 0.f, 1.f, 1.f); // blue

	vec4 topNorm( 0.f,  1.f,  0.f, 0.f); // up
	vec4 botNorm( 0.f, -1.f,  0.f, 0.f); // down
	vec4 rigNorm( 1.f,  0.f,  0.f, 0.f); // right
	vec4 lefNorm(-1.f,  0.f,  0.f, 0.f); // left
	vec4 bacNorm( 0.f,  0.f,  1.f, 0.f); // in
	vec4 froNorm( 0.f,  0.f, -1.f, 0.f); // out

	m_cube->addVerts({
		{ trf, rigCol, rigNorm }, { trb, rigCol, rigNorm }, { brb, rigCol, rigNorm },
		{ brb, rigCol, rigNorm }, { brf, rigCol, rigNorm }, { trf, rigCol, rigNorm }, // right side
		{ tlb, lefCol, lefNorm }, { tlf, lefCol, lefNorm }, { blf, lefCol, lefNorm },
		{ blf, lefCol, lefNorm }, { blb, lefCol, lefNorm }, { tlb, lefCol, lefNorm }, // left side
		{ tlf, froCol, froNorm }, { trf, froCol, froNorm }, { brf, froCol, froNorm },
		{ brf, froCol, froNorm }, { blf, froCol, froNorm }, { tlf, froCol, froNorm }, // front side
		{ blb, bacCol, bacNorm }, { brb, bacCol, bacNorm }, { trb, bacCol, bacNorm },
		{ trb, bacCol, bacNorm }, { tlb, bacCol, bacNorm }, { blb, bacCol, bacNorm }, // back side
		{ trf, topCol, topNorm }, { tlf, topCol, topNorm }, { tlb, topCol, topNorm },
		{ tlb, topCol, topNorm }, { trb, topCol, topNorm }, { trf, topCol, topNorm }, // top side
		{ brb, botCol, botNorm }, { blb, botCol, botNorm }, { blf, botCol, botNorm },
		{ blf, botCol, botNorm }, { brf, botCol, botNorm }, { brb, botCol, botNorm }  // bottom side
	});

	DkCommandBuffer* bfr = getCommandPool(DK_GRAPHICS_QUEUE)->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	if (!m_cube->initVertBuffer(getDevice(), bfr, getQueue(DK_GRAPHICS_QUEUE))) return false;
	getCommandPool(DK_GRAPHICS_QUEUE)->freeBuffer(bfr);

	m_pipeline.addDescriptorBinding({
		0,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	});
	
	m_pipeline.addDescriptorBinding({
		1,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	});

	m_pipeline.addVertexInfo<DkMesh>();
	m_pipeline.setDepthTestEnabled(true);
	if (!m_pipeline.init()) return false;

	m_descPool.addToPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2);
	if (!m_descPool.init()) return false;
	m_descSet = m_descPool.allocate(m_pipeline.getDescriptorSetLayout());
	if (m_descSet == nullptr) return false;
	if (!m_descSet->updateBuffer(0, m_cube->getMVPBuffer(), 0, VK_WHOLE_SIZE, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)) return false;
	if (!m_descSet->updateBuffer(1, m_cube->getMVNormalBuffer(), 0, VK_WHOLE_SIZE, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)) return false;

	m_initialized = true;
	initTime = system_clock::now();
	return true;
}

bool DkSample_Cube_Lighting::draw() {
	DkFrameResources& frame = *(m_frames[m_curFrame]);
	if (!frame.reset()) return false;

	DkCommandBuffer* cmdBfr = frame.getCmdBfr();

	time_point<system_clock> thisTime = system_clock::now();
	duration<float> sinceStartDur = thisTime - initTime;
	float sinceStart = duration_cast<duration<float>>(thisTime - initTime).count();
	vec3 eye(6.f, 7.f, 6.f);
	vec3 center(0.f, 0.f, 0.f);
	vec3 up(0.f, 1.f, 0.f);
	
	VkExtent2D ext = getWindow().getExtent();
	mat4 look = lookAt(eye, center, up);
	mat3 rot3 = rotation(sinceStart * 360.f / 3.f, vec3(-1.f, 1.f, 1.f));
	mat4 persp = perspective(45.f, (float)ext.width / (float)ext.height, .1f, 100.f);
	mat4 transform = look * mat4(rot3);
	m_cube->setMV(transform);
	m_cube->setProj(persp);

	if (!m_cube->pushMVP(cmdBfr, getQueue(DK_GRAPHICS_QUEUE), { m_pushUniformSemaphores[m_curFrame] }, { m_pushUniformNormalSemaphores[m_curFrame] })) return false;

	if (!cmdBfr->beginRecording()) return false;

	if (getQueue(DK_GRAPHICS_QUEUE).getFamilyIndex() != getQueue(DK_PRESENT_QUEUE).getFamilyIndex()) {
		if (!cmdBfr->setMemoryBarrier(
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{}, {},
			{
				{
					m_swapchain.getImage(frame.getCurIndex()),
					VK_ACCESS_MEMORY_READ_BIT,
					VK_ACCESS_MEMORY_READ_BIT,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					getQueue(DK_PRESENT_QUEUE).getFamilyIndex(),
					getQueue(DK_GRAPHICS_QUEUE).getFamilyIndex(),
					VK_IMAGE_ASPECT_COLOR_BIT
				}
			}
			)) return false;
	}
	
	VkClearValue clearCol, clearDepth;
	clearCol.color = { .1f, .2f, .3f, 1.f };
	clearDepth.depthStencil = { 1.f, 0 };
	if (!cmdBfr->beginRenderPass(&m_renderPass, &frame.getFramebuffer(), { clearCol, clearDepth })) return false;
	if (!cmdBfr->bindPipeline(&m_pipeline)) return false;
	if (!cmdBfr->bindDescriptorSet(m_descSet, &m_pipeline)) return false;
	if (!cmdBfr->setViewport(0, { { 0.f, 0.f, (float)getWindow().getExtent().width, (float)getWindow().getExtent().height, 0.f, 1.f } })) return false;
	if (!cmdBfr->setScissor(0, { { { 0, 0 },{ getWindow().getExtent().width, getWindow().getExtent().height } } })) return false;
	if (!cmdBfr->bindVertexBuffer(m_cube)) return false;
	if (!cmdBfr->draw(m_cube)) return false;
	if (!cmdBfr->endRenderPass()) return false;

	if (getQueue(DK_GRAPHICS_QUEUE).getFamilyIndex() != getQueue(DK_PRESENT_QUEUE).getFamilyIndex()) {
		if (!cmdBfr->setMemoryBarrier(
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			{}, {},
			{
				{
					m_swapchain.getImage(frame.getCurIndex()),
					VK_ACCESS_MEMORY_READ_BIT,
					VK_ACCESS_MEMORY_READ_BIT,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					getQueue(DK_GRAPHICS_QUEUE).getFamilyIndex(),
					getQueue(DK_PRESENT_QUEUE).getFamilyIndex(),
					VK_IMAGE_ASPECT_COLOR_BIT
				}
			}
			)) return false;
	}

	if (!cmdBfr->endRecording()) return false;

	if (!cmdBfr->submit(getQueue(DK_GRAPHICS_QUEUE), { { &frame.getImgAcqSemaphore(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT }, { m_pushUniformSemaphores[m_curFrame], VK_PIPELINE_STAGE_VERTEX_SHADER_BIT} }, { &frame.getRdyPrsSemaphore() }, frame.getFence())) return false;

	if (!getWindow().present(getQueue(DK_PRESENT_QUEUE), { &frame.getRdyPrsSemaphore() }, { { &m_swapchain, frame.getCurIndex() } })) return false;

	m_curFrame = (m_curFrame + 1) % m_frameCount;
	return true;
}

void DkSample_Cube_Lighting::finalize() {
	m_pipeline.finalize();
	if (m_cube != nullptr) {
		delete m_cube;
		m_cube = nullptr;
	}
	m_renderPass.finalize();
	m_swapchain.finalize();
	for (auto& frame : m_frames) {
		delete frame;
		frame = nullptr;
	}
	for (auto& sem : m_pushUniformSemaphores) {
		delete sem;
		sem = nullptr;
	}
	m_descSet = nullptr;
	m_descPool.finalize();
	m_frames.clear();
	vulkanFinalize();
	m_initialized = false;
}

bool DkSample_Cube_Lighting::resize() {
	if (!m_swapchain.resize()) return false;
	for (auto& frame : m_frames) {
		if (!frame->resize()) return false;
	}
	return true;
}