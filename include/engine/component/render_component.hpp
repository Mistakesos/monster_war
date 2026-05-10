#pragma once
#include <SFML/Graphics/Color.hpp>

namespace engine::component {

/**
 * @brief 渲染组件, 包含图层ID和深度，
 * 颜色调整参数（调整后 = 原始颜色 * 调整颜色）
 */
struct RenderComponent {
    /**
     * @brief 构造函数
     * @param layer 图层ID，数字越小越先绘制(默认MAIN_LAYER)
     * @param depth 在同一图层内的深度，数字越小越先绘制 (默认0.0f)
     * @param color 颜色调整参数 (调整后 = 原始颜色 * 调整颜色) (默认白色)
     */
    explicit RenderComponent(int layer = MAIN_LAYER, float depth = 0.f, sf::Color color = sf::Color::White)
        : layer_{layer}
        , depth_{depth}
        , color_{color} {
    }

    // 重载比较运算符，用于排序
    bool operator<(const RenderComponent& other) const {
        if (layer_ == other.layer_) {     // 如果图层相同，则比较深度
            return depth_ < other.depth_;
        }
        return layer_ < other.layer_;    // 如果图层不同，则比较图层ID
    }

    static constexpr int MAIN_LAYER = 10;    ///< @brief 主图层ID，默认为10
    int layer_ = 0;        ///< @brief 图层ID，数字越小越先绘制 
    float depth_ = 0.f;    ///< @brief 在同一图层内的深度，数字越小越先绘制
                          /*  (可用于实现y-sort排序，也可设定其它渲染顺序逻辑) */
    sf::Color color_{sf::Color::White};      ///< @brief 颜色调整参数
};

}   // namespace engine::component