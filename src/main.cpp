#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <spdlog/spdlog.h>
#include "imgui_sfml/imgui-SFML.h"
#include "imgui/imgui.h"

void imgui_optional_settings() {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // 设置 ImGui 主题
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // 设置缩放
    float main_scale = 1.f;     // 或者直接设置更加稳定
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // 固定样式缩放比例。
    io.FontGlobalScale = main_scale;        // 设置初始字体缩放比例。

    // 设置透明度
    float window_alpha = 0.5f;

    // 修改各个UI元素的透明度
    style.Colors[ImGuiCol_WindowBg].w = window_alpha;
    style.Colors[ImGuiCol_PopupBg].w = window_alpha;

    // 为了正确显示中文，我们需要加载支持中文的字体。
    ImFont* font = io.Fonts->AddFontFromFileTTF(
        "assets/fonts/NotoSansSC-Medium.ttf",   // 字体文件路径
        // "assets/fonts/VonwaonBitmap-16px.ttf",
        36.f,                                   // 字体大小
        nullptr,                                // 字体配置参数
        io.Fonts->GetGlyphRangesChineseFull() // 字符范围
    );
    if (!font) {
        // 如果字体加载失败，回退到默认字体，但中文将无法显示。
        io.Fonts->AddFontDefault();
        spdlog::warn("警告：无法加载中文字体，中文字符将无法正确显示。");
    }

    io.FontDefault = font;  // 设置为全局默认字体

    if (!ImGui::SFML::UpdateFontTexture()) {
        spdlog::error("错误：字体更新失败！");
    }
}

void imgui_window_1() {
    static float volume_value = 0.5f;
    
    ImGui::Begin("窗口1");
    ImGui::Text("这是第一个窗口");
    ImGui::SetWindowFontScale(1.5f);
    if (ImGui::Button("按钮1", sf::Vector2i(200, 60))) {
        spdlog::info("按钮1被点击");
    }
    ImGui::SetWindowFontScale(1.f);
    if (ImGui::SliderFloat("音量", &volume_value, 0.f, 1.f)) {
        spdlog::info("音量被调整：{}", volume_value);
    }
    ImGui::End();
}

auto texture = sf::Texture("assets/textures/Buildings/Castle.png");
void imgui_window_2() {
    ImGui::Begin("窗口2");
    
    ImGui::Image(texture);
    
    ImGui::End();
}

int main(int, char**) {
    // sfml
    sf::RenderWindow window(sf::VideoMode({1280u, 720u}), "Testing Window");

    // imgui init
    if (!ImGui::SFML::Init(window)) {
        spdlog::critical("ImGui 未正确初始化！");
    }
    imgui_optional_settings();

    sf::Clock delta_clock;
    sf::CircleShape circle(30.f);
    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, event.value());

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, delta_clock.restart());

        // ImGui::ShowDemoWindow();
        imgui_window_1();
        imgui_window_2();

        window.clear();
        window.draw(circle);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    
    return 0;
}