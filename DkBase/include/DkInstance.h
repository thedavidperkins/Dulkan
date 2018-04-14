#ifndef DK_INSTANCE_H
#define DK_INSTANCE_H

#include "DkCommon.h"

class DkInstance {
public:
	DkInstance();
	~DkInstance() { finalize(); }

	bool init();
	void finalize();
	VkInstance get() { return m_instance; }

	DkInstance(const DkInstance& rhs) = delete;
	DkInstance& operator=(const DkInstance& rhs) = delete;

	// Customization before init
	void setAppName(std::string name);
	void setDesiredExtensions(const std::vector<const char*> desiredExts);

	// Getters
	std::string getAppName() { return m_appName; }
private:

	// Init information
	std::string m_appName;
	std::vector<const char*> m_desiredInstExts;

	// Init results
	VkInstance m_instance;
	bool m_initialized;
};

#endif//DK_INSTANCE_H
