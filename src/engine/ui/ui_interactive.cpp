#include "engine/ui/ui_interactive.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/core/context.hpp"
#include "engine/render/render.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/ui/state/ui_state.hpp"
#include <entt/entity/entity.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace engine::ui {

UIInteractive::~UIInteractive() = default;

UIInteractive::UIInteractive(engine::core::Context& context, sf::Vector2f position, sf::Vector2f size)
    : UIElement{std::move(position), std::move(size)}
    , context_{context}
    , current_sprite_id_{entt::null} {
    spdlog::trace("UIInteractive 构造完成");
}

void UIInteractive::set_current_state(std::unique_ptr<engine::ui::state::UIState> state) {
    if (!state) {
        spdlog::warn("尝试设置空的状态！");
        return;
    }
    current_state_ = std::move(state);
}

void UIInteractive::set_next_state(std::unique_ptr<engine::ui::state::UIState> state) {
    next_state_ = std::move(state);
}

void UIInteractive::add_sprite(entt::id_type name_id, sf::Sprite sprite) {
    // 可交互UI元素必须有一个size用于交互检测，因此如果参数列表中没有指定，则用图片大小作为size
    if (size_.x == 0.f && size_.y == 0.f) {
        size_ = sprite.getGlobalBounds().size;
    }
    // 添加sprite (如果name_id已存在，则替换)
    sprites_.insert_or_assign(name_id, std::move(sprite));
}

void UIInteractive::set_current_sprite(entt::id_type name_id) {
    auto it = sprites_.find(name_id);
    if (it != sprites_.end()) {
        current_sprite_id_ = name_id;
    } else {
        spdlog::warn("Sprite ID '{}' 未找到", name_id);
    }
}

void UIInteractive::set_hover_sound(entt::id_type id, std::string_view path) {
    context_.get_resource_manager().load_sound(id, path);    // 确保音效资源被加载
    sounds_.emplace("ui_hover"_hs, id);
}

void UIInteractive::set_click_sound(entt::id_type id, std::string_view path) {
    context_.get_resource_manager().load_sound(id, path);    // 确保音效资源被加载
    sounds_.emplace("ui_click"_hs, id);
}

void UIInteractive::play_sound(entt::id_type name_id) {
    auto it = sounds_.find(name_id);
    if (it != sounds_.end()) {
        // 使用自定义音效
        context_.get_audio_player().play_sound(it->second);
    } else {
        // 尝试将 name_id 本身作为音效ID播放（默认音效）
        context_.get_audio_player().play_sound(name_id);
    }
}

void UIInteractive::update(sf::Time delta, engine::core::Context &context) {
    // 先更新子节点
    UIElement::update(delta, context);

    // 再更新自己（状态）
    if (current_state_ && interactive_) {
        if (next_state_) {
            set_current_state(std::move(next_state_));
        } 
        current_state_->update(delta, context);
    }
}

void UIInteractive::render(engine::core::Context& context) {
    if (!visible_ || current_sprite_id_ == entt::null)
        return;

    auto it = sprites_.find(current_sprite_id_);
    if (it == sprites_.end()) return;
    auto& sprite = it->second;

    // 计算缩放比例以适应目标尺寸（使用原有的 componentWiseDiv）
    sf::Vector2f original_size(static_cast<float>(sprite.getTextureRect().size.x),
                              static_cast<float>(sprite.getTextureRect().size.y));
    if (original_size.x > 0.f && original_size.y > 0.f) {
        sf::Vector2f scale = size_.componentWiseDiv(original_size);
        sprite.setScale(scale);
    }
    sprite.setPosition(get_screen_position());

    context.get_renderer().draw_ui_sprite(context.get_camera(), sprite);

    // 再渲染子元素（调用基类方法）
    UIElement::render(context);
}
} // namespace engine::ui