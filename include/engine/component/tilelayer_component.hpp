#pragma once
#include "component.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <vector>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::component {
/**
 * @brief 定义瓦片的类型，用于游戏逻辑（例如碰撞）。
 */
enum class TileType {
    Empty,      ///< @brief 空白瓦片
    Normal,     ///< @brief 普通瓦片
    Solid,      ///< @brief 静止可碰撞瓦片
    Unisolid,   ///< @brief 单向静止可碰撞瓦片
    Slope_0_1,  ///< @brief 斜坡瓦片，高度:左0  右1
    Slope_1_0,  ///< @brief 斜坡瓦片，高度:左1  右0
    Slope_0_2,  ///< @brief 斜坡瓦片，高度:左0  右1/2
    Slope_2_1,  ///< @brief 斜坡瓦片，高度:左1/2右1
    Slope_1_2,  ///< @brief 斜坡瓦片，高度:左1  右1/2
    Slope_2_0,  ///< @brief 斜坡瓦片，高度:左1/2右0
    Hazard,     ///< @brief 危险瓦片（例如火焰、尖刺等）
    Ladder      ///< @brief 梯子瓦片
    // 未来补充其它类型
};

/**
 * @brief 包含单个瓦片的渲染和逻辑信息。
 */
struct TileInfo {
    TileInfo(sf::Sprite& s, TileType t = TileType::Empty)
        : sprite(std::move(s)), type(t) {
    }
    sf::Sprite sprite;          ///< @brief 瓦片的视觉表示
    TileType type;              ///< @brief 瓦片的逻辑类型
};

/**
 * @brief 管理和渲染瓦片地图层。
 *
 * 存储瓦片地图的布局、每个瓦片的精灵信息和类型。
 * 负责在渲染阶段绘制可见的瓦片。
 */
class TileLayerComponent final : public Component {
    friend class engine::object::GameObject;
public:
    // TileLayerComponent() = default;
    /**
     * @brief 构造函数
     * @param tile_size 单个瓦片尺寸（像素）
     * @param map_size 地图尺寸（瓦片数）
     * @param tiles 初始化瓦片数据的容器 (会被移动)
     */
    TileLayerComponent(engine::object::GameObject* owner
                     , sf::Vector2i tile_size
                     , sf::Vector2i map_size
                     , std::vector<TileInfo>&& tiles
    );
    ~TileLayerComponent();

    /**
     * @brief 根据瓦片坐标获取瓦片信息
     * @param pos 瓦片坐标 (0 <= x < map_size_.x, 0 <= y < map_size_.y)
     * @return const TileInfo* 指向瓦片信息的指针，如果坐标无效则返回 nullptr
     */
    const TileInfo* get_tile_info_at(sf::Vector2i pos) const;

    /**
     * @brief 根据瓦片坐标获取瓦片类型
     * @param pos 瓦片坐标 (0 <= x < map_size_.x, 0 <= y < map_size_.y)
     * @return TileType 瓦片类型，如果坐标无效则返回 TileType::EMPTY
     */
    TileType get_tile_type_at(sf::Vector2i pos) const;

    /**
     * @brief 根据世界坐标获取瓦片类型
     * @param world_pos 世界坐标
     * @return TileType 瓦片类型，如果坐标无效或对应空瓦片则返回 TileType::EMPTY
     */
    TileType get_tile_type_at_world_pos(const sf::Vector2f& world_pos) const;

    sf::Vector2i get_tile_size() const { return tile_size_; }                                                              ///< @brief 获取单个瓦片尺寸
    sf::Vector2i get_map_size() const { return map_size_; }                                                                ///< @brief 获取地图尺寸
    sf::Vector2f get_world_size() const { return sf::Vector2f(map_size_.x * tile_size_.x, map_size_.y * tile_size_.y); }   ///< @brief 获取地图世界尺寸
    const std::vector<TileInfo>& get_tiles() const { return tiles_; }                                                      ///< @brief 获取瓦片容器
    const sf::Vector2f& get_offset() const { return offset_; }                                                             ///< @brief 获取瓦片层的偏移量
    bool is_hidden() const { return is_hidden_; }                                                                          ///< @brief 获取是否隐藏（不渲染）

    void set_offset(sf::Vector2f offset) { offset_ = std::move(offset); }                                                  ///< @brief 设置瓦片层的偏移量
    void set_hidden(bool hidden) { is_hidden_ = hidden; }                                                                  ///< @brief 设置是否隐藏（不渲染）

protected:
    // 核心循环方法
    void update(sf::Time, engine::core::Context&) override {}
    void render(engine::core::Context& context) override;

private:
    void rebuild_cache() const;

    sf::Vector2i tile_size_;            ///< @brief 单个瓦片尺寸（像素）
    sf::Vector2i map_size_;             ///< @brief 地图尺寸（瓦片数）
    std::vector<TileInfo> tiles_;       ///< @brief 存储所有瓦片信息 (按"行主序"存储, index = y * map_width_ + x)
    sf::Vector2f offset_ = {0.f, 0.f};  ///< @brief 瓦片层在世界中的偏移量 (瓦片层通常不需要缩放及旋转，因此不引入Transform组件)
                                        ///< offset_ 最好也保持默认的0，以免增加不必要的复杂性
    bool is_hidden_ = false;            ///< @brief 是否隐藏（不渲染）

    mutable sf::RenderTexture render_texture_;                              // mutable 因为 render() 是 const 上下文也能重建
    mutable bool cache_dirty_ = true;                                       // 是否需要重新绘制到 render_texture_
    mutable std::unique_ptr<sf::Sprite> cached_sprite_ = nullptr;           // 从 render_texture_ 生成的 sprite，每帧只 draw 这一个
};
} // namespace engine::component