#include "DkApplication.h"
#include "DkAttachmentDescriptionBuilder.h"
#include "DkMesh.h"
#include "DkCommandBuffer.h"

DkApplication::DkApplication() :
	m_frameCount(3),
	m_vkLibrary(nullptr),
	m_instance(),
	m_physDevice(),
	m_window(m_physDevice),
	m_device(m_physDevice),
	m_queues(),
	m_swapchain(m_device, m_window),
	m_commandPools(),
	m_frames(),
	m_curFrame(0),
	m_initialized(false),

	m_renderPass(m_device),
	m_triangle(nullptr),
	m_pipeline(m_device, m_renderPass)
{}

void DkApplication::setFrameCount(uint count) {
	if (m_initialized) {
		std::cout << "Cannot set frame count after initialization." << std::endl;
		return;
	}
	m_frameCount = count;
}

bool DkApplication::init() {
	// Load global functions
	if (!cnctLoaderLib(m_vkLibrary)) return false;
	if (!loadExportedFn(m_vkLibrary)) return false;
	if (!loadGlobalFns()) return false;

	// Init VkInstance
	if (!m_instance.init()) return false;

	// Init physical device
	if (!m_physDevice.init()) return false;

	// Init presentation surface and window
	if (!m_window.init()) return false;

	// Get desired queue indices
	std::vector<uint> indices;
	if (!_findQueueIndices(indices)) return false;
	m_device.setQueueIndices(indices);

	// Init logical device
	if (!m_device.init()) return false;

	// Get queues from device
	_getDeviceQueues();

	// Init command pools: one for each queue
	m_commandPools.clear();
	for (auto& queue : m_queues) {
		DkCommandPool* newPool = new DkCommandPool(m_device, queue);
		if (!newPool->init()) return false;
		m_commandPools.push_back(newPool);
	}

	// Init swapchain
	if (!m_swapchain.init()) return false;

	// Init frames -- the actual framebuffers will be created later, in the animation loop
	for (uint iter = 0; iter < m_frameCount; ++iter) {
		DkFrameResources* newFrame = new DkFrameResources(m_device, &m_swapchain, &m_renderPass);
		if (!newFrame->init()) return false;
		m_frames.push_back(newFrame);
	}
	if (!m_commandPools[DK_GRAPHICS_QUEUE]->allocate(m_frames)) return false;

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

	m_triangle = new DkMesh(nullptr);
	m_triangle->addVerts({
		{ 0.f, -.75f, 0.f },
		{ -.75f, .75f, 0.f },
		{ .75f, .75f, 0.f }
	});
	DkCommandBuffer* bfr = m_commandPools[DK_GRAPHICS_QUEUE]->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	if (!m_triangle->initBuffer(m_device, bfr, m_queues[DK_GRAPHICS_QUEUE])) return false;
	m_commandPools[DK_GRAPHICS_QUEUE]->freeBuffer(bfr);

	m_pipeline.addMesh(m_triangle);
	if (!m_pipeline.init()) return false;

	m_initialized = true;
	return true;
}

void DkApplication::render() {
	ShowWindow(m_window.getHWnd(), SW_SHOWNORMAL);
	UpdateWindow(m_window.getHWnd());

	MSG message;
	bool loop = true;

	while (loop) {
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			switch (message.message) {
			case USER_MESSAGE_MOUSE_CLICK:
			case USER_MESSAGE_MOUSE_MOVE:
			case USER_MESSAGE_MOUSE_WHEEL:
			case USER_MESSAGE_RESIZE:
				break;
			case USER_MESSAGE_QUIT:
				loop = false;
				break;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else {
			if (!draw()) loop = false;
		}
	}
}

//void DkApplication::MouseClick(size_t button_index, bool state) {
//	if (2 > button_index) {
//		
//	}
//}

// Assume one queue from each family
bool DkApplication::_findQueueIndices(std::vector<uint>& indices) {
	indices.clear();

	int test = m_physDevice.findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
	if (test < 0) return false;
	m_queues[DK_GRAPHICS_QUEUE].setFamIndex((uint)test);
	indices.push_back((uint)test);

	test = m_physDevice.findQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
	if (test < 0) return false;
	m_queues[DK_COMPUTE_QUEUE].setFamIndex((uint)test);
	if (indices[0] != (uint)test) indices.push_back((uint)test);

	test = m_physDevice.findQueueFamilyPresentIndex(m_window);
	if (test < 0) return false;
	m_queues[DK_PRESENT_QUEUE].setFamIndex((uint)test);
	if (indices[0] != (uint)test && indices[1] != (uint)test) indices.push_back((uint)test);

	for (uint iter = 0; iter < (uint)DK_NUM_QUEUE_TYPES; ++iter) {
		m_queues[iter].setType((DkQueueType)iter);
	}
	return true;
}

void DkApplication::_getDeviceQueues() {
	for (uint iter = 0; iter < (uint)DK_NUM_QUEUE_TYPES; ++iter) {
		vkGetDeviceQueue(m_device.get(), m_queues[iter].getFamilyIndex(), 0, &(m_queues[iter].get()));
	}
}

void DkApplication::finalize() {
	m_pipeline.finalize();
	if (m_triangle != nullptr) {
		delete m_triangle;
		m_triangle = nullptr;
	}
	m_renderPass.finalize();
	m_swapchain.finalize();
	for (auto& frame : m_frames) {
		frame->finalize();
		delete frame;
		frame = nullptr;
	}
	m_frames.clear();
	for (auto& pool : m_commandPools) {
		pool->finalize();
		delete pool;
		pool = nullptr;
	}
	m_commandPools.clear();
	m_device.finalize();
	m_physDevice.finalize();
	m_window.finalize();
	m_instance.finalize();
	FreeLibrary(m_vkLibrary);
	m_initialized = false;
}

bool DkApplication::draw() {
	DkFrameResources& frame = *(m_frames[m_curFrame]);
	if (!frame.reset()) return false;

	DkCommandBuffer* cmdBfr = frame.getCmdBfr();

	if (!cmdBfr->beginRecording()) return false;

	if (m_queues[DK_GRAPHICS_QUEUE].getFamilyIndex() != m_queues[DK_PRESENT_QUEUE].getFamilyIndex()) {
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
					m_queues[DK_PRESENT_QUEUE].getFamilyIndex(),
					m_queues[DK_GRAPHICS_QUEUE].getFamilyIndex(),
					VK_IMAGE_ASPECT_COLOR_BIT
				}
			}
		)) return false;
	}

	if (!cmdBfr->beginRenderPass(&m_renderPass, &frame.getFramebuffer(), { { .1f, .2f, .3f, 1.f } })) return false;
	if (!cmdBfr->bindPipeline(&m_pipeline)) return false;
	if (!cmdBfr->setViewport(0, { { 0.f, 0.f, (float)m_window.getExtent().width, (float)m_window.getExtent().height, 0.f, 1.f } })) return false;
	if (!cmdBfr->setScissor(0, { { { 0, 0 }, { m_window.getExtent().width, m_window.getExtent().height } } })) return false;
	if (!cmdBfr->bindVertexBuffers({ m_triangle })) return false;
	if (!cmdBfr->draw(m_triangle)) return false;
	if (!cmdBfr->endRenderPass()) return false;

	if (m_queues[DK_GRAPHICS_QUEUE].getFamilyIndex() != m_queues[DK_PRESENT_QUEUE].getFamilyIndex()) {
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
					m_queues[DK_GRAPHICS_QUEUE].getFamilyIndex(),
					m_queues[DK_PRESENT_QUEUE].getFamilyIndex(),
					VK_IMAGE_ASPECT_COLOR_BIT
				}
			}
		)) return false;
	}

	if (!cmdBfr->endRecording()) return false;

	if (!cmdBfr->submit(m_queues[DK_GRAPHICS_QUEUE], { {&frame.getImgAcqSemaphore(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT} }, { &frame.getRdyPrsSemaphore() }, frame.getFence())) return false;

	if (!m_window.present(m_queues[DK_PRESENT_QUEUE], { &frame.getRdyPrsSemaphore() }, { { &m_swapchain, frame.getCurIndex() } })) return false;

	m_curFrame = (m_curFrame + 1) % m_frameCount;
	return true;
}

// Helper functions for commonly sought objects
VkInstance getVkInstance() {
	return DkApplication::getInstance().getDkInstance().get();
}

VkPhysicalDevice getPhysDevice() {
	return DkApplication::getInstance().getPhysDevice().get();
}

VkDevice getDevice() {
	return DkApplication::getInstance().getDevice().get();
}


