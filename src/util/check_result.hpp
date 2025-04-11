#pragma once

#include <vulkan/vk_enum_string_helper.h>

#include <vulkan/vulkan.hpp>

#include "util/logger.hpp"

namespace hep {

#define VK_CHECK_RESULT(X) checkVulkanResult(X, __FILE__, __LINE__)

static void checkVulkanResult(vk::Result result, const char* file,
                              const char* line) {
  if (result == vk::Result::eSuccess) { return; }

  log::error("vk::Result Error: ",
             string_VkResult(static_cast<VkResult>(result)));
}

static void checkVkResult(VkResult result) {
  if (result == VK_SUCCESS) { return; }
  log::error("[vulkan] Error: vk::Result: ", result);
}

}  // namespace hep
