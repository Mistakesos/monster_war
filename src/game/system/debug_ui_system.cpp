#include "game/system/debug_ui_system.hpp"
#include "engine/core/context.hpp"
#include <imgui.h>
#include <entt/entity/registry.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

DebugUISystem::DebugUISystem(entt::registry& registry, engine::core::Context& context)
    : registry_{registry}
    , context_{context} {
}

void DebugUISystem::update() {
    render_demo_ui();
}

void DebugUISystem::render_demo_ui() {
    // --- 中文显示测试 ---
    static float volume_value = 0.5f;
    ImGui::Begin("窗口1");
    ImGui::Text("这是第一个窗口");
    ImGui::SetWindowFontScale(1.5f);
    if (ImGui::Button("按钮1", ImVec2(200, 60))) {
        spdlog::info("按钮1被点击");
    }
    ImGui::SetWindowFontScale(1.f);
    if (ImGui::SliderFloat("音量", &volume_value, 0.f, 1.f)) {
        spdlog::info("音量被调整: {}", volume_value);
    }
    ImGui::End();

    // 显示 ImGui 自带的 Demo 窗口
    ImGui::ShowDemoWindow();
}

} // namespace game::system