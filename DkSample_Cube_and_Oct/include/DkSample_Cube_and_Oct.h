#ifndef DK_SAMPLE_CUBE_AND_OCT
#define DK_SAMPLE_CUBE_AND_OCT

#include "DkApplication.h"
#include "DkDescriptorPool.h"
#include "DkDescriptorSet.h"

class DkSample_Cube_and_Oct : public DkApplication {
public:
	virtual bool init();
	virtual bool draw();
	virtual void finalize();
	virtual bool resize();

	// Setters
	void setFrameCount(uint count);

	DkSample_Cube_and_Oct();
	virtual ~DkSample_Cube_and_Oct() { finalize(); }
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
	DkMesh* m_octahedron;
	DkDescriptorPool m_descPool;
	DkDescriptorSet* m_descSet;

	// State
	bool m_initialized;
	uint m_curFrame;

};

#endif//DK_SAMPLE_CUBE_AND_OCT


