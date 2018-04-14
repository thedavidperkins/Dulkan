#ifndef DK_COMMON_H
#define DK_COMMON_H

#include <iostream>
#include <vector>

#include <Windows.h>
#undef CreateSemaphore

#include "VulkanFunctions.h"

#ifdef MAIN_SRC
#define EXTERN
#else
#define EXTERN extern
#endif

using uint = uint32_t;
using uint64 = uint64_t;

#endif//DK_COMMON_H
