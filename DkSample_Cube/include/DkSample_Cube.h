#ifndef DK_SAMPLE_CUBE
#define DK_SAMPLE_CUBE

#include "DkApplication.h"
#include "DkDescriptorPool.h"
#include "DkDescriptorSet.h"

class DkSample_Cube : public DkApplication {
public:
	virtual bool init();
	virtual bool draw();
	virtual void finalize();
	virtual bool resize();

	// Setters
	void setFrameCount(uint count);

	DkSample_Cube();
	virtual ~DkSample_Cube() { finalize(); }
private:
	// Options
	uint m_frameCount;
	
	// Managed objects
	DkSwapchain m_swapchain;
	DkRenderPass m_renderPass;
	std::vector<DkFrameResources*> m_frames;
	std::vector<DkSemaphore*> m_pushUniformSemaphores;
	DkPipeline m_pipeline;
	DkMesh* m_cube;
	DkDescriptorPool m_descPool;
	DkDescriptorSet* m_descSet;

	// State
	bool m_initialized;
	uint m_curFrame;

};

#endif//DK_SAMPLE_CUBE


