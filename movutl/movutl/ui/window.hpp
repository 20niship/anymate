#pragma once
#include <movutl/core/rect.hpp>
#include <movutl/db/camera.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/render/render.hpp>
#include <movutl/ui/io.hpp>
#include <movutl/ui/widget.hpp>

namespace mu::ui {

class uiWindow {
private:
  struct Cursors {
    Cursors() = default;
    void init() {
      arrow    = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
      ibeam    = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
      crosshar = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
      hand     = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
      hresize  = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
      vresize  = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    }
    GLFWcursor *arrow, *ibeam, *crosshar, *hand, *hresize, *vresize;
  } cursors;

  render::Render render;
  uint64_t currentFrame;
  std::string name;
  Vec2d pos, size;
  core::Rect clipping_rect{0, 0, 99999, 999999};
  void updateUniformBuffer(); //< update uniform buffers for windows
                              //
                              //
#ifdef WITH_VULKAN
  vk::SurfaceKHR surface;
  vk::SwapchainKHR swapChain;
  std::vector<vk::Image> swapChainImages;
  vk::Format swapChainImageFormat;
  vk::Extent2D swapChainExtent;
  std::vector<vk::ImageView> swapChainImageViews;
  std::vector<vk::Framebuffer> swapChainFramebuffers;
  vk::RenderPass renderPass;
  vk::PipelineLayout pipelineLayout, pipelineLayout_ui;
  vk::UniquePipeline graphicsPipeline, graphicsPipeline_ui;
  std::vector<vk::DescriptorSet> descriptorSet;
  vk::DescriptorSetLayout descriptorSetLayout;
  std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;
  render::uiBuffer uniformBuffer, vertexBuffer, vertexBuffer_ui;
  void cleanupSwapChain();
  void recreateSwapChain();

  void createUniformBuffer();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createDescriptorSetLayout();
  void createGraphicsPipeline();
  // TODO:
  /* void createCommandBuffers(const render::DrawData*); */
  void createFramebuffers();
  void createGraphicsPipeline_UI();
  vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
  vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
  vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);


  static vk::VertexInputBindingDescription getBindingDescription() {
    vk::VertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding                           = 0;
    bindingDescription.stride                            = sizeof(db::MeshCol::Vertex);
    bindingDescription.inputRate                         = vk::VertexInputRate::eVertex;
    return bindingDescription;
  }

  static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
    std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions = {};
    attributeDescriptions[0].binding                                         = 0;
    attributeDescriptions[0].location                                        = 0;
#ifdef VKUI_ENGINE_USE_FLOAT_VERTEX
    attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
#else
    attributeDescriptions[0].format = vk::Format::eR16G16B16Sint;
#endif
    attributeDescriptions[0].offset = db::MeshCol::offset_pos();

    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = vk::Format::eR8G8B8Uint;
    attributeDescriptions[1].offset   = db::MeshCol::offset_col();
    return attributeDescriptions;
  }


  static vk::VertexInputBindingDescription getBindingDescription_UI() {
    vk::VertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(db::Mesh2D::Vertex);
    bindingDescription.inputRate = vk::VertexInputRate::eVertex;
    return bindingDescription;
  }

  static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions_UI() {
    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = {};

    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = vk::Format::eR16G16Sint;
    attributeDescriptions[0].offset   = db::Mesh2D::offset_pos();

    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = vk::Format::eR8G8B8Uint;
    attributeDescriptions[1].offset   = db::Mesh2D::offset_col();

    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = vk::Format::eR16G16Uint;
    attributeDescriptions[2].offset   = db::Mesh2D::offset_uv();
    return attributeDescriptions;
  }
#endif

public:
  struct _wndStyle {
    unsigned char EnableTitleBar : 1;    // Enable title bar
    unsigned char EnableChildWindow : 1; // Enable child window and show child windows
    unsigned char EnablePopups : 1;      // Enable popup Window
    unsigned char isFullScreen : 1;
  } wndStyle;

  uiRootWidget root_widget;
  uiRootWidget2D root_widget_ui;
  GLFWwindow* window;
  bool framebufferResized;
  render::Render* get_renderer() { return &render; }

public:
  uiWindow(const std::string& _name, uint16_t width, uint16_t height);
  ~uiWindow();
  void drawFrame();
  void drawDevelopperHelps();
  void drawDevFontTexture();
  void fill(const core::Vec3b& col);
  void draw_ui();
  void init();
  inline auto getGLFWwindow() const { return window; }
  inline bool should_close() const { return glfwWindowShouldClose(window); }
  inline void should_close(bool should) { glfwSetWindowShouldClose(window, should); }
  core::Vec2d getWindowSize() const {
    int x, y;
    glfwGetWindowSize(window, &x, &y);
    return {x, y};
  }
  core::Vec2d getWindowPos() const { return pos; }

  // cursors
  inline void setHandCursor() const { glfwSetCursor(window, cursors.hand); }
  inline void setArrowCursor() const { glfwSetCursor(window, cursors.arrow); }
  inline void setHResizeCursor() const { glfwSetCursor(window, cursors.hresize); }
  inline void setVResizeCursor() const { glfwSetCursor(window, cursors.vresize); }
  inline void setCrossHairCursor() const { glfwSetCursor(window, cursors.crosshar); }
  inline void setIbeamCursor() const { glfwSetCursor(window, cursors.ibeam); }
  inline void setDefaultCursor() const { glfwSetCursor(window, NULL); }

  // --------------     rendering functions     -------------
  inline void setClippingRect(const core::Rect& r) { clipping_rect = r; }
  inline core::Rect getClippingRect() const { return clipping_rect; }

  inline void resizeCB(int w, int h) {
    framebufferResized = true;
    size[0]            = w;
    size[1]            = h;
    root_widget_ui.needRendering(true);
    render.update_wndsize(); // draw()の最後似合ったのを変更
  }

  void addWidgetUI(uiWidget* w) { root_widget_ui.AddWidget(w); }
  void addWidget(uiWidget* w) { root_widget_ui.AddWidget(w); }

  inline db::CameraPosition* getCameraPtr() { return &render.camera; }
  inline void setCameraPos(const Vec3 pos) { render.camera.pos = pos; }
  inline void setCameraTarget(const Vec3 target) { render.camera.dir = target; }
  inline auto getCameraPos() const { return render.camera.pos; }
  inline auto getCameraTarget() const { return render.camera.dir; }
  inline void setCameraUpVec(const Vec3 up) { render.camera.u = up; }
  inline void setCameraScale(const float scale) { render.camera.scale = scale; }
  inline void setCameraZclip(const double znear, const double zfar) {
    render.camera.zNear = znear;
    render.camera.zFar  = zfar;
  }
  inline void setCameraAspect(const double aspect) { render.camera.aspect = aspect; }
  Mat4x4f get_camera_martix() { return render.camera.getCameraProjMatrix(); }
  void terminate();
  void process_event(); //! きーぼードやマウスの入力に対する処理を行う
};

} // namespace mu::ui
