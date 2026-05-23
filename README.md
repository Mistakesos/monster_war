# MonsterWar
**MonsterWar** is a cross-platform tower defense game developed in C++ with Entt, SFML3, ImGui, nlohmann-json and Tiled.

>This is a learning project based on the 4th episode of the tutorial series "[C++ 游戏开发之旅](https://cppgamedev.top/)".

## Control
```
Mouse left - select a unit from UI portrait / place a unit on map;
S - Skill active shortcut;
R - Retreat shortcut;
U - Upgrade shortcut;
P - pause or resume;
A,D / left,right - to move UI portrait panel;
```

- **Note**: The game uses ImGui for temporary UI, so you need to resize the webpage size to fit the screen. Otherwise some UI elements may not be visible.

## ScreenShot
<img src="https://theorhythm.top/gamedev/MW/screen_shot_mw-en1.webp" style='width: 800px;'/>
<img src="https://theorhythm.top/gamedev/MW/screen_shot_mw-en2.webp" style='width: 800px;'/>
<img src="https://theorhythm.top/gamedev/MW/screen_shot_mw-en3.webp" style='width: 800px;'/>

## Third-party libraries
* [SFML3](https://github.com/SFML/SFML)
* [ImGui](https://github.com/ocornut/imgui)
* [EnTT](https://github.com/skypjack/entt)
* [nlohmann-json](https://github.com/nlohmann/json)
* [spdlog](https://github.com/gabime/spdlog)

## How to build
Dependencies will be automatically downloaded by Git FetchContent to make building quite easy:
```bash
git clone https://github.com/Mistakesos/monster_war.git
cd monster_war
cmake -S . -B build
cmake --build build
```
# Credits
- sprite
    - https://pixelfrog-assets.itch.io/tiny-swords
    - https://pipoya.itch.io/pipoya-free-2d-game-character-sprites
    - https://htmljsgit.itch.io/magic-area
- portrait
    - https://blog.goo.ne.jp/akarise
    - https://roughsketch.en-grey.com/
- FX
    - https://bdragon1727.itch.io/750-effect-and-fx-pixel-all
    - https://sentient-dream-games.itch.io/pixel-vfx-level-up-effect
- font
    - https://timothyqiu.itch.io/vonwaon-bitmap
- UI
    - https://ludicarts.itch.io/free-rpg-icon-set-i
    - https://clockworkraven.itch.io/free-rpg-icon-pack-100-weapons-and-po-clockwork-raven-studios
    - https://kenney.nl/assets/emotes-pack
    - https://bdragon1727.itch.io/custom-border-and-panels-menu-all-part
- sound
    - https://ateliermagicae.itch.io/fantasy-ui-sound-effects
    - https://pixabay.com/sound-effects/violin-lose-4-185125/
    - https://pixabay.com/sound-effects/level-win-6416/
    - https://freesound.org/people/SilverIllusionist/sounds/664265/ (Healing (Balm).wav by Dylan Kelk)
    - https://freesound.org/people/DWOBoyle/sounds/136696/
- music
    - https://tommusic.itch.io/free-fantasy-sfx-and-music-bundle
    - https://www.chosic.com/download-audio/45301/