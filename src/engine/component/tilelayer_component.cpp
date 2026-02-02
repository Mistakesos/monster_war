#include "engine/component/tilelayer_component.hpp"
#include "engine/core/context.hpp"
#include "engine/render/render.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {
TileLayerComponent::TileLayerComponent(engine::object::GameObject* owner
                                     , sf::Vector2i tile_size
                                     , sf::Vector2i map_size
                                     , std::vector<TileInfo>&& tiles)
    : Component{owner}
    , tile_size_{std::move(tile_size)}
    , map_size_{std::move(map_size)}
    , tiles_{std::move(tiles)} {
    if (tiles_.size() != static_cast<size_t>(map_size_.x * map_size_.y)) {
        spdlog::error("TileLayerComponent: 地图尺寸与提供的瓦片向量大小不匹配。瓦片数据将被清除。");
        tiles_.clear();
        map_size_ = {0, 0};
    }
    
    spdlog::trace("TileLayerComponent 构造完成");
}

TileLayerComponent::~TileLayerComponent() {

}

const TileInfo* TileLayerComponent::get_tile_info_at(sf::Vector2i pos) const {
    if (pos.x < 0 || pos.x >= map_size_.x || pos.y < 0 || pos.y >= map_size_.y) {
        spdlog::warn("TileLayerComponent: 瓦片坐标越界: ({}, {})", pos.x, pos.y);
        return nullptr;
    }
    size_t index = static_cast<size_t>(pos.y * map_size_.x + pos.x);
    if (index < tiles_.size()) {
        return &tiles_[index];
    }
    spdlog::warn("TileLayerComponent: 瓦片索引越界: {}", index);
    return nullptr;
}

TileType TileLayerComponent::get_tile_type_at(sf::Vector2i pos) const {
    const TileInfo* info = get_tile_info_at(pos);
    return info ? info->type : TileType::Empty;
}

TileType TileLayerComponent::get_tile_type_at_world_pos(const sf::Vector2f& world_pos) const {
    sf::Vector2f relative_pos = world_pos - offset_;

    int tile_x = static_cast<int>(std::floor(relative_pos.x / tile_size_.x));
    int tile_y = static_cast<int>(std::floor(relative_pos.y / tile_size_.y));

    return get_tile_type_at(sf::Vector2i{tile_x, tile_y});
}

void TileLayerComponent::rebuild_cache() const {
    if (tile_size_.x <= 0 || tile_size_.y <= 0 || map_size_.x <= 0 || map_size_.y <= 0) {
        spdlog::warn("TileLayerComponent: 无效的瓦片尺寸或地图尺寸");
        return;
    }

    // 计算整个层的像素尺寸
    unsigned int total_width = static_cast<unsigned int>(map_size_.x * tile_size_.x);
    unsigned int total_height = static_cast<unsigned int>(map_size_.y * tile_size_.y);
    
    // 计算最大精灵高度以确定是否需要额外空间
    unsigned int max_sprite_height = static_cast<unsigned int>(tile_size_.y);
    for (const auto& tile_info : tiles_) {
        if (tile_info.type != TileType::Empty) {
            unsigned int sprite_height = static_cast<unsigned int>(tile_info.sprite.getGlobalBounds().size.y);
            if (sprite_height > max_sprite_height) {
                max_sprite_height = sprite_height;
            }
        }
    }
    
    // 如果有精灵高度超过瓦片高度，调整总高度
    if (max_sprite_height > static_cast<unsigned int>(tile_size_.y)) {
        total_height = static_cast<unsigned int>(map_size_.y * tile_size_.y) + 
                      (max_sprite_height - static_cast<unsigned int>(tile_size_.y));
        spdlog::debug("TileLayerComponent: 检测到高精灵，调整纹理高度为 {}", total_height);
    }

    // 创建或重新创建渲染纹理
    if (!render_texture_.resize({total_width, total_height})) {
        spdlog::error("TileLayerComponent: 无法创建 RenderTexture，大小 {}x{}", total_width, total_height);
        return;
    }

    // 清除为透明
    render_texture_.clear(sf::Color::Transparent);

    // 设置视图，确保坐标从(0,0)开始
    sf::View view = render_texture_.getDefaultView();
    // 使用 SFML 3 正确的 API - 通过 setCenter 和 setSize 设置视图
    view.setCenter({total_width / 2.f, total_height / 2.f});
    view.setSize({static_cast<float>(total_width), static_cast<float>(total_height)});
    render_texture_.setView(view);

    // 绘制所有瓦片
    for (int y = 0; y < map_size_.y; ++y) {
        for (int x = 0; x < map_size_.x; ++x) {
            size_t index = static_cast<size_t>(y) * map_size_.x + x;
            if (index >= tiles_.size() || tiles_[index].type == TileType::Empty) {
                continue;
            }

            auto& tile_info = tiles_[index];
            
            // 创建精灵的副本，避免修改原始精灵
            sf::Sprite sprite = tile_info.sprite;
            
            // 计算位置（与原始render函数相同的逻辑）
            sf::Vector2f tile_left_top_pos = {
                static_cast<float>(x * tile_size_.x),
                static_cast<float>(y * tile_size_.y)
            };
            
            // 处理高度调整 - 使用底部对齐
            float sprite_height = sprite.getGlobalBounds().size.y;
            if (static_cast<int>(sprite_height) != tile_size_.y) {
                tile_left_top_pos.y -= (sprite_height - static_cast<float>(tile_size_.y));
            }
            
            sprite.setPosition(tile_left_top_pos);
            render_texture_.draw(sprite);
        }
    }

    // 完成绘制并更新纹理
    render_texture_.display();
    
    // 创建或更新缓存的精灵
    if (!cached_sprite_) {
        cached_sprite_ = std::make_unique<sf::Sprite>(render_texture_.getTexture());
    } else {
        cached_sprite_->setTexture(render_texture_.getTexture());
    }
    
    cached_sprite_->setPosition(offset_);
    
    // 标记缓存为最新
    cache_dirty_ = false;
    
    spdlog::debug("TileLayerComponent: 缓存重建完成，纹理大小: {}x{}", total_width, total_height);
}

void TileLayerComponent::render(engine::core::Context& context) {
    if (is_hidden_) return;
    if (tile_size_.x <= 0 || tile_size_.y <= 0) {
        spdlog::warn("TileLayerComponent: 无效瓦片尺寸！");
        return;
    }

    // 检查是否需要重建缓存
    if (cache_dirty_ || !cached_sprite_) {
        rebuild_cache();
    }

    // 如果缓存精灵有效，则绘制
    if (cached_sprite_) {
        context.get_renderer().draw_sprite(context.get_camera(), *cached_sprite_);
    } else {
        spdlog::warn("TileLayerComponent: 缓存精灵无效，回退到原始渲染");
        // 回退到原始渲染方法
        for (int y = 0; y < map_size_.y; y++) {
            for (int x = 0; x < map_size_.x; x++) {
                size_t index = static_cast<size_t>(y) * map_size_.x + x;
                if (index < tiles_.size() && tiles_[index].type != TileType::Empty) {
                    auto& tile_info = tiles_[index];
                    sf::Vector2f tile_left_top_pos = {
                        offset_.x + static_cast<float>(x) * tile_size_.x,
                        offset_.y + static_cast<float>(y) * tile_size_.y
                    };
                    if (static_cast<int>(tile_info.sprite.getGlobalBounds().size.y) != tile_size_.y) {
                        tile_left_top_pos.y -= (tile_info.sprite.getGlobalBounds().size.y - static_cast<float>(tile_size_.y));
                    }
                    tile_info.sprite.setPosition(tile_left_top_pos);
                    context.get_renderer().draw_sprite(context.get_camera(), tile_info.sprite);
                }
            }
        }
    }
}
} // namespace engine::component