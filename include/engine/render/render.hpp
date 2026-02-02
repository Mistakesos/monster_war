#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <string>
#include <optional>

namespace sf {
    class RenderWindow;
    class Sprite;
    class Text;
} // namespace sf

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::render {
class Camera;

/**
 * @brief 封装 sfml 渲染操作
 *
 * 包装 sf::RenderWindow 并提供清除屏幕、绘制精灵和呈现最终图像的方法。
 * 在构造时初始化。依赖于一个有效的 sf::RenderWindow 和 ResourceManager。
 * 构造失败会抛出异常。
 */
class Renderer final {
public:
    /**
     * @brief 构造函数
     *
     * @param window 指向有效的 SDL_Renderer 的指针。不能为空。
     * @param resource_manager 指向有效的 ResourceManager 的指针。不能为空。
     * @throws std::runtime_error 如果任一指针为 nullptr。
     */
    Renderer(sf::RenderWindow* window, engine::resource::ResourceManager* resource_manager);

    ~Renderer() = default;

    /**
     * @brief 清空当前帧
     */
    void clear_frame();

    /**
     * @brief 显示当前绘制内容
     */
    void display_frame();

    /**
     * @brief 绘制一个精灵
     * @param sprite 包含纹理ID、源矩形和翻转状态的 Sprite 对象。
     */
    void draw_sprite(const Camera& camera, sf::Sprite& sprite);

    /**
     * @brief 绘制精灵考虑视差背景
     */
    void draw_parallax(const Camera& camera, sf::Sprite& sprite
                     , const sf::Vector2f& scroll_factor
                     , sf::Vector2<bool> repeat = {true, true}
                     , const sf::Vector2f& scale = {1.f, 1.f}
    );
    
    /**
     * @brief 绘制ui精灵
     * @param sprite 要绘制的ui精灵
     */
    void draw_ui_sprite(const Camera& camera, sf::Sprite& sprite);

    /**
     * @brief 绘制文字
     * @param text 要绘制的文字
     */
    void draw_text(const Camera& camera
                 , std::string_view str
                 , std::string_view font_id
                 , unsigned int font_size
                 , sf::Vector2f position
                 , sf::Color font_color = sf::Color::White
    );

    /**
     * @brief 绘制 ui 文字
     * @param text 要绘制的 ui 文字
     */
    void draw_ui_text(const Camera& camera
                    , std::string_view str
                    , std::string_view font_id
                    , unsigned int font_size
                    , sf::Vector2f position
                    , sf::Color font_color = sf::Color::White
    );

    /**
     * @brief 绘制填充矩形
     * @param color 填充颜色
     */
    void draw_ui_filled_rect(const Camera& camera, const sf::FloatRect& rect, sf::Color color);

private:
    sf::RenderWindow* window_obs_ = nullptr;                                    ///< @brief 窗口的观察者指针，不负责管理生命周期，不要在该类里手动释放他
    engine::resource::ResourceManager* resourec_manager_obs_ = nullptr;         ///< @brief 资源管理器的观察者指针，不负责管理生命周期，不要在该类里手动释放他
};
} // namespace engine::render
