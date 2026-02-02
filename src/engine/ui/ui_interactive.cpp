#include "engine/ui/ui_interactive.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/core/context.hpp"
#include "engine/render/render.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/ui/state/ui_state.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {
UIInteractive::~UIInteractive() = default;

UIInteractive::UIInteractive(engine::core::Context& context, sf::Vector2f position, sf::Vector2f size)
    : UIElement{std::move(position), std::move(size)}
    , context_{context} {
    spdlog::trace("UIInteractive 构造完成");
}

void UIInteractive::set_state(std::unique_ptr<engine::ui::state::UIState> state) {
    if (!state) {
        spdlog::warn("尝试设置空的状态！");
        return;
    }

    current_state_ = std::move(state);
}

void UIInteractive::add_sprite(std::string_view name, std::unique_ptr<sf::Sprite> sprite) {
    // 可交互UI元素必须有一个size用于交互检测，因此如果参数列表中没有指定，则用图片大小作为size
    if (size_.x == 0.f && size_.y == 0.f) {
        size_ = sprite->getGlobalBounds().size;
    }
    // 添加精灵
    sprites_[std::string(name)] = std::move(sprite);
}

void UIInteractive::set_sprite(std::string_view name) {
    if (sprites_.find(std::string(name)) != sprites_.end()) {
        current_sprite_ = sprites_[std::string(name)].get();
    } else {
        spdlog::warn("Sprite '{}' 未找到", name);
    }
}

void UIInteractive::add_sound(std::string_view name, std::string_view path) {
    sounds_[std::string(name)] = path;
}

void UIInteractive::play_sound(std::string_view name) {
    if (sounds_.find(std::string(name)) != sounds_.end()) {
        context_.get_audio_player().play_sound(sounds_[std::string(name)]);
    } else {
        spdlog::error("Sound '{}' 未找到", name);
    }
}

bool UIInteractive::handle_input(engine::core::Context& context) {
    // 先更新子节点
    if (UIElement::handle_input(context)) {
        return true;
    }

    // 再更新自己（状态）
    if (current_state_ && interactive_) {
        current_state_->handle_input(context);
        if (current_state_->next_state_) {
            set_state(std::move(current_state_->next_state_));
            return true;
        }
    }
    return false;
}

void UIInteractive::render(engine::core::Context& context) {
    if (!visible_)
        return;

    // 先渲染自身
    auto& camera = context.get_camera();
    auto scale = size_.componentWiseDiv(static_cast<sf::Vector2f>(current_sprite_->getTextureRect().size));
    current_sprite_->setScale(scale);
    current_sprite_->setPosition(get_screen_position());
    context.get_renderer().draw_ui_sprite(camera, *current_sprite_);

    // 再渲染子元素（调用基类方法）
    UIElement::render(context);
}
} // namespace engine::ui