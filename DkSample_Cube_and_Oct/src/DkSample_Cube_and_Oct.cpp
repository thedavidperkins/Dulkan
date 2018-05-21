#include <chrono>
#include "DkSample_Cube_and_Oct.h"
#include "DkAttachmentDescriptionBuilder.h"
#include "DkCommandBuffer.h"

using namespace math;
using namespace std::chrono;

static time_point<system_clock> initTime;

DkSample_Cube_and_Oct::DkSample_Cube_and_Oct() :
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

void DkSample_Cube_and_Oct::setFrameCount(uint count) {
	if (m_initialized) {
		std::cout << "Cannot set frame count after initialization." << std::endl;
		return;
	}
	m_frameCount = count;
}

bool DkSample_Cube_and_Oct::init() {
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

	if (!m_pipeline.addShader("E:/VisualStudio/Projects/Dulkan/DkSample_Cube_and_Oct/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT)) return false;
	if (!m_pipeline.addShader("E:/VisualStudio/Projects/Dulkan/DkSample_Cube_and_Oct/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)) return false;

	m_cube = new DkMesh(nullptr, 1);

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
	vec4 whiteC(1.f, 1.f, 1.f, 1.f); // white
	vec4 blackC(0.f, 0.f, 0.f, 1.f); // black

	m_cube->addVerts({
		{ trf, rigCol }, { trb, rigCol }, { brb, rigCol },
		{ brb, rigCol }, { brf, rigCol }, { trf, rigCol }, // right side
		{ tlb, lefCol }, { tlf, lefCol }, { blf, lefCol },
		{ blf, lefCol }, { blb, lefCol }, { tlb, lefCol }, // left side
		{ tlf, froCol }, { trf, froCol }, { brf, froCol },
		{ brf, froCol }, { blf, froCol }, { tlf, froCol }, // front side
		{ blb, bacCol }, { brb, bacCol }, { trb, bacCol },
		{ trb, bacCol }, { tlb, bacCol }, { blb, bacCol }, // back side
		{ trf, topCol }, { tlf, topCol }, { tlb, topCol },
		{ tlb, topCol }, { trb, topCol }, { trf, topCol }, // top side
		{ brb, botCol }, { blb, botCol }, { blf, botCol },
		{ blf, botCol }, { brf, botCol }, { brb, botCol }  // bottom side
	});

	m_octahedron = new DkMesh(nullptr, 1);
	
	vec4 top( 0.f,  1.f,  0.f, 1.f);
	vec4 bot( 0.f, -1.f,  0.f, 1.f);
	vec4 lef( 1.f,  0.f,  0.f, 1.f);
	vec4 rig(-1.f,  0.f,  0.f, 1.f);
	vec4 fro( 0.f,  0.f,  1.f, 1.f);
	vec4 bac( 0.f,  0.f, -1.f, 1.f);

	m_octahedron->addVerts({
		{ top, topCol }, { rig, topCol }, { fro, topCol },
		{ top, froCol }, { fro, froCol }, { lef, froCol },
		{ top, lefCol }, { lef, lefCol }, { bac, lefCol },
		{ top, bacCol }, { bac, bacCol }, { rig, bacCol },
		{ bot, botCol }, { rig, botCol }, { bac, botCol },
		{ bot, whiteC }, { bac, whiteC }, { lef, whiteC },
		{ bot, rigCol }, { lef, rigCol }, { fro, rigCol },
		{ bot, blackC }, { fro, blackC }, { rig, blackC }
	});

	DkCommandBuffer* bfr = getCommandPool(DK_GRAPHICS_QUEUE)->allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	if (!m_cube->initVertBuffer(getDevice(), bfr, getQueue(DK_GRAPHICS_QUEUE), false)) return false;
	if (!m_octahedron->initVertBuffer(getDevice(), bfr, getQueue(DK_GRAPHICS_QUEUE), false)) return false;
	getCommandPool(DK_GRAPHICS_QUEUE)->freeBuffer(bfr);

	m_pipeline.addPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4));
	m_pipeline.addMesh(m_cube); // weirdness -- I originally thought that multiple vertex buffers
								//    of the same type would need to have separate bindings, but
								//    it makes more sense in simple cases to successively bind
								//    meshes to the same binding for successive draws. Multiple
								//    bindings are more for adding multiple buffers carrying
	                            //    different attributes of the vertex data.
								//
								//    Pending implementation: a more natural treatment of pipeline
								//    layouts and vertex bindings. For now, add one mesh of the
								//    format desired and make sure additional meshes match that.
	
	m_pipeline.setDepthTestEnabled(true);
	if (!m_pipeline.init()) return false;

	m_initialized = true;
	initTime = system_clock::now();
	return true;
}

bool DkSample_Cube_and_Oct::draw() {
	DkFrameResources& frame = *(m_frames[m_curFrame]);
	if (!frame.reset()) return false;

	DkCommandBuffer* cmdBfr = frame.getCmdBfr();

	time_point<system_clock> thisTime = system_clock::now();
	duration<float> sinceStartDur = thisTime - initTime;
	float sinceStart = duration_cast<duration<float>>(thisTime - initTime).count();
	vec3 eye(8.f, 5.f, 8.f);
	vec3 center(0.f, 0.f, 0.f);
	vec3 up(0.f, 1.f, 0.f);
	vec3 left = 3.f * normalize(cross(up, center - eye));
	vec3 right = -left;

	VkExtent2D ext = getWindow().getExtent();
	mat4 look = lookAt(eye, center, up);
	mat4 lTransl = translate(left[0], left[1], left[2]);
	mat4 rTransl = translate(right[0], right[1], right[2]);
	mat3 lrot3 = rotation(sinceStart * 360.f / 3.f, vec3(-1.f, 1.f, 1.f));
	mat3 rrot3 = rotation(-sinceStart * 360.f / 3.f, vec3(0.f, 1.f, 0.f));
	mat4 persp = perspective(45.f, (float)ext.width / (float)ext.height, .1f, 100.f);
	mat4 ltransform = persp * look * lTransl * mat4(lrot3);
	mat4 rtransform = persp * look * rTransl * scale(1.6f, 1.6f, 1.6f) * mat4(rrot3);
	m_cube->setMVP(ltransform, 0);
	m_octahedron->setMVP(rtransform, 0);

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
	if (!cmdBfr->setViewport(0, { { 0.f, 0.f, (float)getWindow().getExtent().width, (float)getWindow().getExtent().height, 0.f, 1.f } })) return false;
	if (!cmdBfr->setScissor(0, { { { 0, 0 },{ getWindow().getExtent().width, getWindow().getExtent().height } } })) return false;
	if (!cmdBfr->bindVertexBuffers({ m_cube })) return false;
	if (!cmdBfr->pushConstants(m_pipeline, 0, &transpose(m_cube->getMVP()))) return false;
	if (!cmdBfr->draw(m_cube)) return false;
	if (!cmdBfr->bindVertexBuffers({ m_octahedron })) return false;
	if (!cmdBfr->pushConstants(m_pipeline, 0, &transpose(m_octahedron->getMVP()))) return false;
	if (!cmdBfr->draw(m_octahedron)) return false;
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

void DkSample_Cube_and_Oct::finalize() {
	m_pipeline.finalize();
	if (m_octahedron != nullptr) {
		delete m_octahedron;
		m_octahedron = nullptr;
	}
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

bool DkSample_Cube_and_Oct::resize() {
	if (!m_swapchain.resize()) return false;
	for (auto& frame : m_frames) {
		if (!frame->resize()) return false;
	}
	return true;
}