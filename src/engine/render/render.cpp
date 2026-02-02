#include "engine/render/render.hpp"
#include "engine/resource/resource_manager.hpp"
#include "engine/render/camera.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <iostream>

namespace engine::render {
Renderer::Renderer(sf::RenderWindow* window, engine::resource::ResourceManager* resource_manager)
    : window_obs_{window}
    , resourec_manager_obs_{resource_manager} {
    spdlog::trace("构造 Renderer...");
    if (!window_obs_) {
        throw std::runtime_error("Renderer 构造失败：提供的 window 指针为空");
    }
    if (!resourec_manager_obs_) {
        throw std::runtime_error("ResourceManager 构造失败：提供的 ResourceManager 指针为空");
    }
    spdlog::trace("Renderer 构造成功");
}

void Renderer::clear_frame() {
    window_obs_->clear(sf::Color::Black);
}

void Renderer::display_frame() {
    window_obs_->display();
}

void Renderer::draw_sprite(const Camera& camera, sf::Sprite& sprite) {
    window_obs_->setView(camera.get_world_view());
    window_obs_->draw(sprite);
}

void Renderer::draw_parallax(
    const Camera& camera,
    sf::Sprite& sprite,
    const sf::Vector2f& scroll_factor,
    sf::Vector2<bool> repeat,
    const sf::Vector2f& scale
) {
    sf::IntRect src = sprite.getTextureRect();
    if (src.size.x <= 0 || src.size.y <= 0) return;

    const sf::View& view = camera.get_world_view();
    sf::Vector2f view_center = camera.get_world_view_center();
    sf::Vector2f view_size = camera.get_world_view_size();

    sf::Vector2f layer_origin = sprite.getPosition();

    sf::Vector2f tile_size = {
        static_cast<float>(src.size.x) * scale.x,
        static_cast<float>(src.size.y) * scale.y
    };

    sprite.setScale(scale);

    // 视差偏移
    sf::Vector2f parallax_offset = {
        view_center.x * (1.f - scroll_factor.x),
        view_center.y * (1.f - scroll_factor.y)
    };

    sf::Vector2f layer_world_pos = layer_origin + parallax_offset;

    // 视口边界
    sf::Vector2f view_min = view_center - view_size / 2.f;
    sf::Vector2f view_max = view_center + view_size / 2.f;

    window_obs_->setView(view);

    if (!repeat.x && !repeat.y) {
        sprite.setPosition(layer_world_pos);
        window_obs_->draw(sprite);
        return;
    }

    // 计算起始位置
    auto calc_start_pos = [](float view_min, float layer_pos, float tile_size, bool repeat) -> float {
        if (!repeat) return layer_pos;
        float offset = view_min - layer_pos;
        int tile_idx = static_cast<int>(std::floor(offset / tile_size));
        return layer_pos + (tile_idx - 1) * tile_size;
    };

    float start_x = calc_start_pos(view_min.x, layer_world_pos.x, tile_size.x, repeat.x);
    float start_y = calc_start_pos(view_min.y, layer_world_pos.y, tile_size.y, repeat.y);

    float end_x = repeat.x ? view_max.x + tile_size.x : start_x + tile_size.x;
    float end_y = repeat.y ? view_max.y + tile_size.y : start_y + tile_size.y;

    // 绘制
    for (float y = start_y; y < end_y; y += tile_size.y) {
        for (float x = start_x; x < end_x; x += tile_size.x) {
            sprite.setPosition({x, y});
            window_obs_->draw(sprite);
        }
    }
}

void Renderer::draw_ui_sprite(const Camera& camera, sf::Sprite& sprite) {
    window_obs_->setView(camera.get_ui_view());
    window_obs_->draw(sprite);
}

void Renderer::draw_text(const Camera& camera
                       , std::string_view str
                       , std::string_view font_id
                       , unsigned int font_size
                       , sf::Vector2f position
                       , sf::Color font_color) {
    window_obs_->setView(camera.get_world_view());

    auto font = resourec_manager_obs_->get_font(font_id);
    if (!font) {
        spdlog::warn("drawUIText 获取字体失败: {} 大小 {}", std::string(font_id), font_size);
        return;
    }

    sf::String string(sf::String::fromUtf8(str.begin(), str.end()));
    sf::Text text(*font, string, font_size);
    text.setPosition(position);
    text.setFillColor(font_color);

    auto shadow = text;
    shadow.setPosition(text.getPosition() + sf::Vector2f{2.f, 2.f});
    shadow.setFillColor(sf::Color::Black);

    window_obs_->draw(shadow);
    window_obs_->draw(text);
}

void Renderer::draw_ui_text(const Camera& camera
                          , std::string_view str
                          , std::string_view font_id
                          , unsigned int font_size
                          , sf::Vector2f position
                          , sf::Color font_color) {
    window_obs_->setView(camera.get_ui_view());

    auto font = resourec_manager_obs_->get_font(font_id);
    if (!font) {
        spdlog::warn("drawUIText 获取字体失败: {} 大小 {}", std::string(font_id), font_size);
        return;
    }

    sf::String string(sf::String::fromUtf8(str.begin(), str.end()));
    sf::Text text(*font, string, font_size);
    text.setPosition(position);
    text.setFillColor(font_color);

    auto shadow = text;
    shadow.setPosition(text.getPosition() + sf::Vector2f{2.f, 2.f});
    shadow.setFillColor(sf::Color::Black);

    window_obs_->draw(shadow);
    window_obs_->draw(text);
}

void Renderer::draw_ui_filled_rect(const Camera& camera, const sf::FloatRect& rect, sf::Color color) {
    window_obs_->setView(camera.get_ui_view());
    sf::RectangleShape shape;
    shape.setPosition({rect.position.x, rect.position.y});
    shape.setSize({rect.size.x, rect.size.y});
    shape.setFillColor(color);
    window_obs_->draw(shape);
}
} // namespace engine::render