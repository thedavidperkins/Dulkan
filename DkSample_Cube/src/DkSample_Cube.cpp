#include "DkSample_Cube.h"
#include "DkAttachmentDescriptionBuilder.h"
#include "DkCommandBuffer.h"

using namespace math;

DkSample_Cube::DkSample_Cube() :
	DkApplication(),
	m_frameCount(3),
	m_swapchain(getDevice(), getWindow()),
	m_renderPass(getDevice()),
	m_frames(),
	m_pipeline(getDevice(), m_renderPass),
	m_cube(nullptr),
	m_initialized(false),
	m_curFrame(0)
{}

void DkSample_Cube::setFrameCount(uint count) {
	if (m_initialized) {
		std::cout << "Cannot set frame count after initialization." << std::endl;
		return;
	}
	m_frameCount = count;
}

bool DkSample_Cube::init() {
	if (!vulkanInit()) return false;

	// Init swapchain
	if (!m_swapchain.init()) return false;

	// Init frames -- the actual framebuffers will be created later, in the animation loop
	for (uint iter = 0; iter < m_frameCount; ++iter) {
		DkFrameResources* newFrame = new DkFrameResources(getDevice(), &m_swapchain, &m_renderPass);
		if (!newFrame->init()) return false;
		m_frames.push_back(newFrame);
	}
	if (!getCommandPool(DK_GRAPHICS_QUEUE)->allocate(m_frames)) return false;

	m_renderPass.addAttachmentDescription(DkAttachmentDescriptionBuilder::basicColorOutputAttachment());

	VkAttachmentReference attRef = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	m_renderPass.addSubpassDescription({
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&attRef,
		nullptr,
		nullptr,
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

	vec3 trf(1.f, 1.f, -1.f);
	vec3 trb(1.f, 1.f, 1.f);
	vec3 tlf(-1.f, 1.f, -1.f);
	vec3 tlb(-1.f, 1.f, 1.f);
	vec3 brf(1.f, -1.f, -1.f);
	vec3 brb(1.f, -1.f, 1.f);
	vec3 blf(-1.f, -1.f, -1.f);
	vec3 blb(-1.f, -1.f, 1.f);

	m_cube->addVerts({
		trf, trb, brb,
		brb, brf, trf,
		tlb, tlf, blf,
		blf, blb, tlb,
		tlf, trf, brf,
		brf, blf, tlf,
		blb, brb, trb,
		trb, tlb, blb,
		trf, tlf, tlb,
		tlb, trb, trf,
		brb, blb, blf,
		blf, brf, brb
	});

	DkCommandBuffer* bfr = getCommandPool(DK_GRAPHICS_QUEUE)->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	if (!m_cube->initBuffer(getDevice(), bfr, getQueue(DK_GRAPHICS_QUEUE))) return false;
	getCommandPool(DK_GRAPHICS_QUEUE)->freeBuffer(bfr);

	m_pipeline.addMesh(m_cube);
	if (!m_pipeline.init()) return false;

	m_initialized = true;
	return true;
}

bool DkSample_Cube::draw() {
	DkFrameResources& frame = *(m_frames[m_curFrame]);
	if (!frame.reset()) return false;

	DkCommandBuffer* cmdBfr = frame.getCmdBfr();

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

	if (!cmdBfr->beginRenderPass(&m_renderPass, &frame.getFramebuffer(), { { .1f, .2f, .3f, 1.f } })) return false;
	if (!cmdBfr->bindPipeline(&m_pipeline)) return false;
	if (!cmdBfr->setViewport(0, { { 0.f, 0.f, (float)getWindow().getExtent().width, (float)getWindow().getExtent().height, 0.f, 1.f } })) return false;
	if (!cmdBfr->setScissor(0, { { { 0, 0 },{ getWindow().getExtent().width, getWindow().getExtent().height } } })) return false;
	if (!cmdBfr->bindVertexBuffers({ m_cube })) return false;
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

	if (!cmdBfr->submit(getQueue(DK_GRAPHICS_QUEUE), { { &frame.getImgAcqSemaphore(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT } }, { &frame.getRdyPrsSemaphore() }, frame.getFence())) return false;

	if (!getWindow().present(getQueue(DK_PRESENT_QUEUE), { &frame.getRdyPrsSemaphore() }, { { &m_swapchain, frame.getCurIndex() } })) return false;

	m_curFrame = (m_curFrame + 1) % m_frameCount;
	return true;
}

void DkSample_Cube::finalize() {
	m_pipeline.finalize();
	if (m_cube != nullptr) {
		delete m_cube;
		m_cube = nullptr;
	}
	m_renderPass.finalize();
	m_swapchain.finalize();
	for (auto& frame : m_frames) {
		frame->finalize();
		delete frame;
		frame = nullptr;
	}
	m_frames.clear();
	vulkanFinalize();
	m_initialized = false;
}

bool DkSample_Cube::resize() {
	return m_swapchain.resize();
}