#pragma once

#include <movutl/core/vector.hpp>
#include <movutl/db/image.hpp> 
#include <movutl/render/font.hpp>
#include <movutl/render/render.hpp>
#include <movutl/db/shader.hpp>

namespace mu::render {

struct Engine {
  bool init_finished;
  core::Vec<db::Image> textures;
  core::Vec<db::Shader> shaders;
  size_t font_texture_idx;
  core::Vec<render::Render> renderer;
  render::uiFont text_renderer;
#ifdef WITH_VULKAN
  vk::UniqueInstance instance;
  VkDebugUtilsMessengerEXT callback;
  vk::PhysicalDevice physicalDevice;
  vk::UniqueDevice device;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;
  vk::DescriptorPool descriptorPool;
  vk::CommandPool commandPool;
  vk::Image TextureImage;
  vk::DeviceMemory textureImageMemory;
  vk::ImageView textureImageView;
  vk::Sampler textureSampler;
  std::vector<vk::Semaphore> imageAvailableSemaphores;
  std::vector<vk::Semaphore> renderFinishedSemaphores;
  std::vector<vk::Fence> inFlightFences;
  vk::UniqueShaderModule vertexShader, fragmentShader, vertexShader_ui, fragmentShader_ui;

  void createInstance();
  void createCommandPool();
  std::vector<const char*> getRequiredExtensions();
  vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);
  bool isDeviceSuitable(const vk::PhysicalDevice& device);
  bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
  bool checkValidationLayerSupport();
  void createSyncObjects();
  void createDescriptorPool();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void setupDebugCallback();
  vk::UniqueShaderModule createShader(std::string filename);

  void createTextureImage();
  void transitionImageLayout(vk::Image image, [[maybe_unused]] vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();
    vk::ImageMemoryBarrier barrier{};
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
      barrier.srcAccessMask = (vk::AccessFlags)0;
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
      sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
      destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      sourceStage = vk::PipelineStageFlagBits::eTransfer;
      destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
      throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), {}, {}, barrier);
    endSingleTimeCommands(commandBuffer);
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, [[maybe_unused]] void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
  }
  vk::CommandBuffer beginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    vk::CommandBuffer cmdbuf;
    try {
      auto _commandBuffers = device->allocateCommandBuffers(allocInfo);
      assert(_commandBuffers.size() > 0);
      cmdbuf = _commandBuffers[0];
    } catch(const vk::SystemError& err) {
      throw std::runtime_error("failed to allocate command buffers!");
    }

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    auto result = cmdbuf.begin(&beginInfo);
    if(result != vk::Result::eSuccess) {
      throw std::runtime_error("failed to begine command buffer!");
    }
    std::cout << "begin" << std::endl;
    return cmdbuf;
  }

  void endSingleTimeCommands(vk::CommandBuffer commandBuffer) {
    commandBuffer.end();
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    graphicsQueue.submit(submitInfo, nullptr);
    graphicsQueue.waitIdle();
    device->freeCommandBuffers(commandPool, 1, &commandBuffer);
  }
  void createShaders() {
    vertexShader = createShader("../shaders/vert.spv");
    fragmentShader = createShader("../shaders/frag.spv");

    vertexShader_ui = createShader("../shaders/vert_ui.spv");
    fragmentShader_ui = createShader("../shaders/frag_ui.spv");
  }
#endif

  Engine(); 
  ~Engine(); 
  bool init();
  bool init_after_surface_creation();

  bool init_shaders();
  bool init_font_renderer();
  bool init_textures();
  
  bool terminate();

  bool error_check();
};

} // namespace mu::render
