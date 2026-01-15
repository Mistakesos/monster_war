// src/main.cpp
#include <iostream>
#include <optional>

// 测试 SFML
#include <SFML/Graphics.hpp>

// 测试 ImGui
#include "imgui/imgui.h"
#include "imgui_sfml/imgui-SFML.h"
#include "entt/entt.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

int main(int, char*[]) {
    std::cout << "Monster War starting..." << std::endl;
    
    // --- entt 测试 ---
    entt::registry registry;

    // 创建一个实体
    entt::entity player = registry.create();
    entt::entity enemy = registry.create();

    // 打印出实体ID，但它就是一个数字，本身没有意义
    // entt会保证这个数字是独一无二的
    std::cout << "Player entity ID: " << static_cast<uint32_t>(player) << std::endl;
    std::cout << "Enemy entity ID: " << static_cast<uint32_t>(enemy) << std::endl;

    // --- spdlog json 测试 ---
    spdlog::info("你好，世界!");
    nlohmann::json json_data = {{"a",10}};
    auto num = json_data["a"].get<int>();
    spdlog::warn("json: {}", num);

    
    // 创建窗口
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Monster War");
    window.setFramerateLimit(60);
    
    // 初始化 ImGui-SFML
    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Failed to initialize ImGui-SFML" << std::endl;
        return -1;
    }
    
    // UI 缩放
    ImGui::GetIO().FontGlobalScale = 2.f;
    ImGui::GetIO().IniFilename = "bin/imgui.ini";  // 相对路径
    ImGui::GetStyle().ScaleAllSizes(2.f);
    
    sf::Clock delta_clock;
    
    // 简单的 SFML 形状
    float radius = 50.f;
    sf::Color color = sf::Color::Red;
    sf::CircleShape circle(radius);
    circle.setFillColor(color);
    circle.setPosition({100.f, 100.f});
    
    // 为 ImGui 颜色编辑器准备的浮点数组 (RGB，范围 0.-1.)
    float circle_color[3] = {1.f, 0.f, 0.f}; // 初始红色
    
    bool show_demo_window = true;
    
    while (window.isOpen()) {
        // 处理事件
        while (std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, event.value());
            
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (auto key_pressed = event->getIf<sf::Event::KeyPressed>()) {
                if (key_pressed->scancode == sf::Keyboard::Scan::Escape) {
                    window.close();
                }
            }
        }
        
        // 更新 ImGui
        ImGui::SFML::Update(window, delta_clock.restart());
        
        // 简单的 ImGui 界面
        ImGui::Begin("Monster War Control Panel");
        ImGui::Text("Welcome to Monster War!");
        ImGui::Checkbox("Show Demo Window", &show_demo_window);
        
        // 颜色编辑器 - 使用 float[3] 数组
        if (ImGui::ColorEdit3("Circle Color", circle_color)) {
            // 当颜色改变时，更新 SFML 颜色
            color.r = static_cast<uint8_t>(circle_color[0] * 255);
            color.g = static_cast<uint8_t>(circle_color[1] * 255);
            color.b = static_cast<uint8_t>(circle_color[2] * 255);
            circle.setFillColor(color);
        }
        
        // 半径滑块
        if (ImGui::SliderFloat("Circle Radius", &radius, 10.f, 100.f)) {
            circle.setRadius(radius);
        }
        
        // 显示当前颜色值
        ImGui::Text("Color RGB: %d, %d, %d", color.r, color.g, color.b);
        ImGui::Text("Position: %.1f, %.1f", circle.getPosition().x, circle.getPosition().y);
        
        // 添加一些额外控制
        static float pos_x = 100.f, pos_y = 100.f;
        ImGui::SliderFloat("Position X", &pos_x, 0.f, 700.f);
        ImGui::SliderFloat("Position Y", &pos_y, 0.f, 500.f);
        circle.setPosition({pos_x, pos_y});
        
        ImGui::End();
        
        // ImGui 演示窗口（可选）
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        
        // 渲染
        window.clear(sf::Color(20, 20, 20));  // 深灰色背景
        
        // 绘制 SFML 内容
        window.draw(circle);
        
        // 绘制 ImGui
        ImGui::SFML::Render(window);
        
        window.display();
    }
    
    // 清理
    ImGui::SFML::Shutdown();
    
    std::cout << "Monster War exiting..." << std::endl;
    return 0;
}