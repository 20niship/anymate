#pragma once 
#ifdef WITH_VULKAN
#include <vulkan/vulkan.hpp>

namespace mu::render{

struct uiBuffer {
  vk::Buffer buf, staging_buf;
  vk::DeviceMemory buf_mem, staging_buf_mem;
  vk::UniqueDevice* device_ptr;
  vk::MemoryPropertyFlags prop;
  vk::DeviceSize buf_size;
  vk::BufferUsageFlags usage;
  bool allocated;
  bool use_staging_buffer;

  uiBuffer();
  uiBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
  ~uiBuffer();
  inline void setUseStagingBuf(bool v) { use_staging_buffer = v; }
  void create(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
  void __create(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer* buf_p, vk::DeviceMemory* mem_p);
  uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
  static std::vector<char> readFile(const std::string& filename);
  vk::CommandBuffer __beginSingleTimeCommands();
  void __endSingleTimeCommands(vk::CommandBuffer commandBuffer);
  void __copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
  bool iscreated(){return allocated;}

  void copyData(const void* src, const size_t size);
  void cleanup();
};
}

#endif 