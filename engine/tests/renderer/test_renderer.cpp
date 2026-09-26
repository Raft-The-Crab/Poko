/**
 * @file test_renderer.cpp
 * @brief Tests for Renderer core
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "renderer/renderer.h"
#include "core/testing/test.h"

using namespace poko::renderer;

POKO_TEST(RenderSettings, DefaultValues) {
    RenderSettings settings;
    POKO_ASSERT_TRUE(settings.enable_vsync);
}

POKO_TEST(RenderSettings, OtherDefaults) {
    RenderSettings settings;
    POKO_ASSERT_TRUE(settings.enable_shadows);
    POKO_ASSERT_EQ(settings.render_scale, 1.0f);
}

POKO_TEST(RenderDevice, Creation) {
    RenderDevice device;
    POKO_ASSERT_FALSE(device.is_initialized());
    POKO_ASSERT_EQ(device.get_device_name(), std::string("Not initialized"));
}

POKO_TEST(Swapchain, Creation) {
    RenderDevice device;
    Swapchain swapchain(&device);
    
    POKO_ASSERT_EQ(swapchain.width(), 0);
    POKO_ASSERT_EQ(swapchain.height(), 0);
}

POKO_TEST(Renderer, Creation) {
    Renderer renderer;
    POKO_ASSERT_NOT_NULL(renderer.get_device());
    POKO_ASSERT_NULL(renderer.get_swapchain());
}

POKO_TEST(Renderer, DefaultSettings) {
    Renderer renderer;
    const auto& settings = renderer.get_settings();
    POKO_ASSERT_TRUE(settings.enable_vsync);
}
