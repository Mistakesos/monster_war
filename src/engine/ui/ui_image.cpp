#include "engine/ui/ui_image.hpp"
#include "engine/core/context.hpp"
#include "engine/render/render.hpp"
#include "engine/resource/resource_manager.hpp"
#include <spdlog/spdlog.h> // 教程中使用了，如果不需要可以去掉

namespace engine::ui {

// --- 1. 通过路径构造 ---
UIImage::UIImage(engine::core::Context& context
               , std::string_view texture_path
               , sf::Vector2f position
               , sf::Vector2f size
               , const std::optional<sf::IntRect>& texture_rect
               , bool is_flipped) 
    : UIElement{std::move(position), std::move(size)}
    // 关键：SFML3必须在这里塞入真实的 Texture，所以我们现场用 context 获取它。
    // 【注意】请将下面的 context.get_texture 替换为你实际资源管理器获取纹理的方法
    // 比如：context.get_resource_manager().get_texture(...)
    , sprite_{*context.get_resource_manager().get_texture(entt::hashed_string{texture_path.data()}.value())}
    , texture_id_{entt::hashed_string{texture_path.data()}.value()}
{
    if (texture_rect.has_value()) {
        sprite_.setTextureRect(texture_rect.value());
    }
    set_flipped(is_flipped);
}

// --- 2. 通过ID构造 ---
UIImage::UIImage(engine::core::Context& context
               , entt::id_type texture_id
               , sf::Vector2f position
               , sf::Vector2f size
               , const std::optional<sf::IntRect>& texture_rect
               , bool is_flipped) 
    : UIElement{std::move(position), std::move(size)}
    , sprite_{*context.get_resource_manager().get_texture(texture_id)} // 现场解析哈希ID
    , texture_id_{texture_id}
{
    if (texture_rect.has_value()) {
        sprite_.setTextureRect(texture_rect.value());
    }
    set_flipped(is_flipped);
}

// --- 3. 原来的 Texture 构造 ---
UIImage::UIImage(const sf::Texture& texture
               , sf::Vector2f position
               , sf::Vector2f size
               , const std::optional<sf::IntRect>& texture_rect
               , bool is_flipped) 
    : UIElement{std::move(position), std::move(size)}
    , sprite_{texture} // 直接传入
{
    if (texture_rect.has_value()) {
        sprite_.setTextureRect(texture_rect.value());
    }
    set_flipped(is_flipped);
}


// --- 核心逻辑完全保留原样 ---
void UIImage::render(engine::core::Context& context) {
    if (!visible_) return;

    auto screen_pos = get_screen_position();
    auto& renderer = context.get_renderer();
    auto& camera = context.get_camera();

    // 关键：如果我们手动设置了 size_，就按 size_ 缩放绘制
    if (size_.x > 0.01f && size_.y > 0.01f) {
        // 计算缩放比例
        auto texture_size = sf::Vector2f(sprite_.getTextureRect().size);
        if (texture_size.x > 0 && texture_size.y > 0) {
            sf::Vector2f scale = { size_.x / texture_size.x, size_.y / texture_size.y };
            sprite_.setScale(scale);
        }
        sprite_.setPosition(screen_pos);
        renderer.draw_ui_sprite(camera, sprite_);
        // 渲染完恢复原始缩放（防止影响下次）
        sprite_.setScale({1.f, 1.f});
    } else {
        // 没设置 size_，就原始大小绘制
        sprite_.setPosition(screen_pos);
        renderer.draw_ui_sprite(camera, sprite_);
    }

    // 渲染子元素
    UIElement::render(context);
}

void UIImage::set_flipped(bool flipped) {
    if (flipped) {
        auto local_bounds = sprite_.getLocalBounds();
        sprite_.setOrigin({local_bounds.size.x, local_bounds.position.y});
        sprite_.setScale({-1.f, 1.f});
    } else {
        sprite_.setOrigin({0.f, 0.f});
        sprite_.setScale({1.f, 1.f});
    }
}

} // namespace engine::ui