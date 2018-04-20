#ifndef DK_SAMPLE_APPLICATION
#define DK_SAMPLE_APPLICATION

#include "DkApplication.h"

class DkSampleApplication : public DkApplication {
public:
	virtual bool init();
	virtual bool draw();
	virtual void finalize();
	virtual bool resize();

	// Setters
	void setFrameCount(uint count);

	DkSampleApplication();
	virtual ~DkSampleApplication() { finalize(); }
private:
	// Options
	uint m_frameCount;
	
	// Managed objects
	DkSwapchain m_swapchain;
	DkRenderPass m_renderPass;
	std::vector<DkFrameResources*> m_frames;
	DkPipeline m_pipeline;
	DkMesh* m_triangle;

	// State
	bool m_initialized;
	uint m_curFrame;

};

#endif//DK_SAMPLE_APPLICATION


