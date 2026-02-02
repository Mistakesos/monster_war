#include "engine/ui/ui_panel.hpp"
#include "engine/render/camera.hpp"
#include "engine/core/context.hpp"
#include "engine/render/render.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {

UIPanel::UIPanel(sf::Vector2f position, sf::Vector2f size, std::optional<sf::Color> background_color)
    : UIElement{std::move(position), std::move(size)}
    , background_color_{std::move(background_color)} {
    spdlog::trace("UIPanel 构造完成。");
}

void UIPanel::render(engine::core::Context& context) {
    if (!visible_) return;

    auto& camera = context.get_camera();

    if (background_color_) {
        context.get_renderer().draw_ui_filled_rect(camera, get_bounds(), background_color_.value());
    }

    UIElement::render(context); // 调用基类渲染方法(绘制子节点)
}
} // namespace engine::ui 