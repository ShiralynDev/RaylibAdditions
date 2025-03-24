#include "RaylibAdditions.hpp"

#include <vector>
#include <raylib.h>
#include <variant>
#include <string>

namespace RaylibAdditions {

    class FrameClass {
        public:
        std::vector<Texture2D*> textures;
        std::vector<Vector2>   texturePos;
        std::vector<float>     textureScales;
    };

    class LoadedRoomClass {
        public:
        std::unordered_map<std::string, Texture2D>      textures;
        std::vector<FrameClass>                         frames;
        std::vector<RaylibAdditions::LoadedButtonClass> Buttons;
        Sound                                           music;
        bool                                            isValid = true;
    }; 

    class RoomClass {
        public:
        int ID = 0;
        Camera2D camera;

        // Loads a .gui room made with RaylibGuiCreator https://github.com/SparklesReal/RaylibGuiCreator also this needs to be rewritten
        LoadedRoomClass loadRoom(std::string path, std::string fileName);
        // Unloads a LoadedRoomClass
        RaylibAdditions::LoadedRoomClass unloadRoom(LoadedRoomClass room);
        // Draws a single frameclass
        void drawFrameClass(FrameClass* frame);
    };

    namespace Menu {

        class slider {
            public:
            slider(std::string entryName, int entryProcentage) : name(entryName), procentage(entryProcentage) {};

            std::string name;
            int procentage = 100;
            Rectangle box = {};
            Rectangle procentageRect = {};
        };
        
        class toggleBox {
            public:
            toggleBox(std::string entryName, bool entryState) : name(entryName), state(entryState) {};

            std::string name;
            bool state = false;
        };

        class stringList {
            public:
            stringList(std::string entryName, std::vector<std::string> stringList) : name(entryName), items(stringList) {};

            std::string name;
            std::vector<std::string> items;
            bool extended = false;
        };

        class Menu {
            bool centered = false;
            float xPos, yPos = 0;
            std::vector<std::string> pageTitles;
            int selectedPage = 0;
            Vector2 menuSize {500, 500};
            std::vector<std::vector<std::variant<toggleBox, slider, stringList>>> settings; // Name, type

            public:
                Menu(bool center, std::vector<std::string> pageNames = {}, Vector2 size = {800, 800}, float x = 0, float y = 0) : centered(center), pageTitles(pageNames), menuSize(size), xPos(x), yPos(y) {
                    for (int i = 0; i < pageNames.size(); i++) {
                        this->settings.push_back({});
                    }
                };


                void loadSettingsFromFile(std::string path);
                void saveSettingsToFile(std::string path);
                void addSettingToPage(std::string page, std::variant<toggleBox, slider, stringList> setting);
                void removeSettingFromPage(std::string page, std::string name);
                void setSettingAtPage(std::string page, std::string name, std::variant<toggleBox, slider, stringList> setting);
                std::variant<toggleBox, slider, stringList>* getVariant(std::string page, std::string name);

                int titleFontSize = 20;
                int entryFontSize = 40;
                int outlineThickness = 10;
                float titleBoxHeight = 50.0;
                Color background = DARKGRAY;
                Color outline = BLACK;
                Color textColor = BLACK;

                void DrawAndUpdate(Vector2 mousePos = GetMousePosition());
        };

        std::unordered_map<std::string, std::variant<bool, int, std::string>> loadSettingsFromFileToMap(std::string path); // For loading to game not as a menu but applied setting
    }

    class ScrollingMenu {
        private:
        float lastGamepadAxis = 0;
        int selectedText = 0;
        std::vector<std::string> options;
        const Color textColor = BLACK;
        const Color selectedColor = BLACK;
        const float textSize = 10;
        const float selectedTextSize = 15;

        public:
        ScrollingMenu(std::vector<std::string> texts, Color textColor, Color selectedColor, float textSize, float selectedTextSize) : options(texts), textColor(textColor), selectedColor(selectedColor), textSize(textSize), selectedTextSize(selectedTextSize) {};

        int drawAndUpdate(Sound* optionChangeSound = nullptr, int gamepad = -1, bool drawWithoutUpdate = false);
        void updateOptions(std::vector<std::string> newOptions) { options = newOptions; };
    };
}