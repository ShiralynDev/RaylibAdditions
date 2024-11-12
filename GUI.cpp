#include "GUI.hpp"

#include "Functions.hpp"

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>

RaylibAdditions::LoadedRoomClass RaylibAdditions::RoomClass::loadRoom(std::string path, std::string fileName) {
    std::ifstream file;
	file.open(path + fileName);
	if (!file.is_open()) {
		std::cerr << "Failed to load: " << path << std::endl;
		LoadedRoomClass failedClass;
		failedClass.isValid = false;
	}
	std::string line;
	int currentFrame = 0, lastFrame = 0;
	bool buttons = false, music = false, tag = false;
	LoadedRoomClass LoadedRoom{};
	FrameClass CurrentLoadedFrame;
	while (std::getline(file, line)) {
		std::cout << line << std::endl;
		
        if (line.find('[') == 0 && line.find(']') == line.size() - 1) {
            tag = true;
        }

		if (tag == false && line.find("-----") == std::string::npos) {
			continue; // if it is first line aka defining size of room for editor
		}

        std::string tagType;
        std::string tagData;
        if (tag) {
            tagType = line.substr(1, line.size() - 2);
            if (tagType.find("frame") == 0) {
                tagType = "frame";
                tagData = line.substr(line.find(':') + 1);
                tagData = tagData.substr(0, tagData.length() - 1);
            }
        }

        if (tagType == "frame") {
            lastFrame = currentFrame;
            currentFrame = std::stoi(tagData);

            if (currentFrame != lastFrame) {
				LoadedRoom.frames.push_back(CurrentLoadedFrame);
				std::cout << "Log: frame placed in LoadedRoom" << std::endl;
				CurrentLoadedFrame = FrameClass();
			}

            continue;
        }

        if (tagType == "buttons") {
            buttons = true;
            continue;
        }

        if (tagType == "music") {
            buttons = false;
			music = true;
			continue;
        }

		if(buttons == true) {
            std::vector<std::string> splitString = functions::splitString(line, "-----");

			std::string texture = splitString[1];
			float textureX = stof(splitString[2]);
			float textureY = stof(splitString[3]);
			float scale = stof(splitString[4].substr(1));
			if(3 == splitString.size()) {
				LoadedRoom.Buttons.push_back(RaylibAdditions::LoadedButtonClass(LoadTexture((path + "Textures/" + texture + ".png").c_str()), {textureX, textureY}, scale));
				continue;
			}
			Sound soundPressed = LoadSound((path + "Sounds/" + splitString[5] + ".wav").c_str());
			Sound soundReleased = LoadSound((path + "Sounds/" + splitString[6] + ".wav").c_str());
			LoadedRoom.Buttons.push_back(RaylibAdditions::LoadedButtonClass(LoadTexture((path + "Textures/" + texture + ".png").c_str()), {textureX, textureY}, scale, soundPressed, soundReleased));
			continue;
		}
		if (music == true) {
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
			LoadedRoom.music = LoadSound((path + "Music/" + line + ".wav").c_str());
			break;
		}
        std::vector<std::string> splitString = functions::splitString(line, "-----");

		std::string texture = splitString[1];
		float textureX = stof(splitString[2]);
		float textureY = stof(splitString[3]);
		float scale = stof(splitString[4].substr(1));

		if (LoadedRoom.textures.find(texture) == LoadedRoom.textures.end())
			LoadedRoom.textures.emplace(texture, LoadTexture((path + "Textures/" + texture + ".png").c_str()));

		CurrentLoadedFrame.textures.push_back(&LoadedRoom.textures.at(texture));
		std::cout << "Log: texture placed in frame: " << path + "Textures/" + texture + ".png" << std::endl;
		CurrentLoadedFrame.texturePos.push_back(Vector2{textureX, textureY});
		std::cout << "Log: texturePos placed in frame: " << textureX << " " << textureY << std::endl;
		CurrentLoadedFrame.textureScales.push_back(scale);
		std::cout << "Log: textureScale placed in frame: " << scale << std::endl;
	}

	LoadedRoom.frames.push_back(CurrentLoadedFrame);
	std::cout << "Log: frame placed in LoadedRoom" << std::endl;

	file.close();
	return LoadedRoom;
}

RaylibAdditions::LoadedRoomClass RaylibAdditions::RoomClass::unloadRoom(RaylibAdditions::LoadedRoomClass room) {
	for (auto& frame : room.frames) {
		for (auto& texture : room.textures) {
			UnloadTexture(texture.second);
		}
	}

	for (auto& button : room.Buttons) {
		UnloadTexture(button.texture);
		UnloadSound(button.pressedSound);
		UnloadSound(button.releasedSound);
	}

	UnloadSound(room.music);

	return RaylibAdditions::LoadedRoomClass();
}

void RaylibAdditions::RoomClass::drawFrameClass(FrameClass* frame) {
	for(int i = 0; i < frame->texturePos.size(); i++) {
		if (i > frame->textures.size() || i > frame->textureScales.size()) {
			std::cout << "drawFrameClass error dumping frame info: " << frame->textures.data() << std::endl;
			return;
		}
		DrawTexture(*(frame->textures[i]), frame->texturePos[i].x, frame->texturePos[i].y, WHITE);
	}
}

// This code looks more trash than my room
void RaylibAdditions::Menu::Menu::DrawAndUpdate(Vector2 mousePos) {
	Rectangle MenuBox = {};
	if (centered) {
		MenuBox.x = (GetScreenWidth() / 2) - (menuSize.x / 2);
		MenuBox.y = (GetScreenHeight() / 2) - (menuSize.y / 2);
	} else {
		MenuBox.x = xPos;
		MenuBox.y = yPos;
	}
	MenuBox.width = menuSize.x;
	MenuBox.height = menuSize.y;

	std::vector<Rectangle> MenuTabs = {};
	float adjustedWidth = MenuBox.width + outlineThickness * (pageTitles.size() - 1);
	float tabWidth = adjustedWidth / float(pageTitles.size());

	for (int i = 0; i < pageTitles.size(); i++) {
		MenuTabs.push_back({
			MenuBox.x + i * (tabWidth - outlineThickness),
			MenuBox.y,
			tabWidth,
			titleBoxHeight
		});
	}


	int i = 0;
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		for (Rectangle& MenuTab : MenuTabs) {
			if (CheckCollisionPointRec(mousePos, MenuTab))
				selectedPage = i;
			i++;
		}
	}

	RaylibAdditions::drawRectWOutline(MenuBox, outlineThickness, background, outline);
	for (int i = 0; i < MenuTabs.size(); i++) {
		if (i == selectedPage)
			drawRectWOutlineWText(MenuTabs.at(i), outlineThickness, LIGHTGRAY, outline, pageTitles.at(i), titleFontSize, textColor);
		else
			drawRectWOutlineWText(MenuTabs.at(i), outlineThickness, background, outline, pageTitles.at(i), titleFontSize, textColor);
	}

	i = 0;
	std::vector<Rectangle> settingsEntry = {};
	std::vector<std::string> settingsEntryText;
	
	for (auto& setting : settings.at(selectedPage)) {  
		std::string entryName = std::visit([](const auto& obj) -> std::string {
        	return obj.name;
    	}, setting);

		settingsEntry.push_back({MenuBox.x + outlineThickness + 10, 
		MenuBox.y + titleBoxHeight + float(outlineThickness) + i * (entryFontSize + 5.0f), 
		MenuBox.width - (float(outlineThickness) * 2.0f), 
		float(entryFontSize) + 5.0f});
		settingsEntryText.push_back(entryName);
		i++;
	}

	for (int i = 0; i < settingsEntry.size(); i++) {
		RaylibAdditions::drawTextLeftCenterRect(settingsEntry.at(i), settingsEntryText.at(i), entryFontSize, textColor);

		if (auto value = std::get_if<toggleBox>(&settings.at(selectedPage).at(i))) {
			Rectangle box {settingsEntry.at(i).x + MeasureText(settingsEntryText.at(i).c_str(), entryFontSize) + 10,
			settingsEntry.at(i).y,
			float(entryFontSize), float(entryFontSize)};
			DrawRectangleLinesEx(box, 1, BLACK);

			if (value->state == true) {
				DrawLineEx({box.x, box.y}, {box.x + box.width, box.y + box.height}, entryFontSize/10, BLACK);
				DrawLineEx({box.x, box.y + box.height}, {box.x + box.width, box.y}, entryFontSize/10, BLACK);
			}
		}

		if (auto value = std::get_if<slider>(&settings.at(selectedPage).at(i))) {
			value->box = {
			settingsEntry.at(i).x + MeasureText(settingsEntryText.at(i).c_str(), entryFontSize) + 10,
			settingsEntry.at(i).y,
			float(entryFontSize) * 5.0f,
			float(entryFontSize)
			};

			value->procentageRect = {
			value->box.x + float(entryFontSize) / 10.0f,
			value->box.y + float(entryFontSize) / 10.0f, 
			(value->box.width - ((float(entryFontSize) / 10.0f) * 2.0f)) * (value->procentage / 100.0f), 
			value->box.height - ((float(entryFontSize) / 10.0f) * 2.0f)
			};

			DrawRectangleLinesEx(value->box, float(entryFontSize) / 10.0f, BLACK);
			DrawRectangleRec(value->procentageRect, GREEN);
		}
	}

	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		i = 0;
		for (Rectangle& entry : settingsEntry) {
			if (CheckCollisionPointRec(mousePos, entry)) {
				auto& settingList = settings.at(selectedPage);
				auto& it = settingList.at(i);

				if (it.index() != settingList.size()) {
					if (auto value = std::get_if<toggleBox>(&it)) {
						if (IsMouseButtonPressed(0)) {
							value->state = !value->state;
							std::cerr << value->state;
						}
					}

					if (auto value = std::get_if<slider>(&it)) {
						Rectangle collisionRect = value->box;
						collisionRect.x += (float(entryFontSize) / 10.0f);
						collisionRect.width -= ((float(entryFontSize) / 10.0f) * 2.0f);
						if (CheckCollisionPointRec(mousePos, collisionRect)) {
							value->procentage = 100 / ((value->box.width - ((float(entryFontSize) / 10.0f) * 2)) / ((mousePos.x + 1) - (value->box.x+float(entryFontSize) / 10.0f)) );
						}
					}

				}

			}
			i++;
    	}
	}
}

void RaylibAdditions::Menu::Menu::addSettingToPage(std::string page, std::variant<toggleBox, slider, stringList> setting) {
	for (int i = 0; i < pageTitles.size(); i++) {
		if (pageTitles.at(i) == page)
			settings.at(i).push_back(setting);
	}
}

void RaylibAdditions::Menu::Menu::loadSettingsFromFile(std::string path) {
	std::ifstream settingsFile(path);

	if (!settingsFile.is_open()) {
		std::cerr << "Could not find file: " << path << std::endl;
		return;
	}

	std::string line;
	std::string page;
	while ( getline(settingsFile, line) ) {
    	std::cout << line << '\n';

		if (line.find('[') == 0 && line.find(']') == line.size() - 1) {
            page = line.substr(1, line.size() - 2);
			pageTitles.push_back(page);
			settings.push_back({});
			continue;
        }

		std::vector<std::string> data = functions::splitString(line, "|"); // data[2] is the value
		data[0].erase(std::remove(data[0].begin(), data[0].end(), ' '), data[0].end());

		if(data[0] == "toggleBox") {
			if (data[2] == "true")
				addSettingToPage(page, RaylibAdditions::Menu::toggleBox(data[1], true));
			else
				addSettingToPage(page, RaylibAdditions::Menu::toggleBox(data[1], false));
		}
		else if (data[0] == "slider") 
			addSettingToPage(page, RaylibAdditions::Menu::slider(data[1], std::stoi(data[2])));

    }
    settingsFile.close();

}

void RaylibAdditions::Menu::Menu::saveSettingsToFile(std::string path) {
	std::ofstream settingsFile(path);

	if (!settingsFile.is_open()) {
		std::cerr << "Could not find file: " << path << std::endl;
		return;
	}

	for (int i = 0; i < pageTitles.size(); i++) {
		if (i != 0)
			settingsFile << "\n";
		settingsFile << "[" << pageTitles.at(i) << "]" << std::endl;
		for (int j = 0; j < settings.at(i).size(); j++) {

			std::string type = "";
			std::string name = "";
			std::string data = "";

			if (std::holds_alternative<RaylibAdditions::Menu::toggleBox>(settings.at(i).at(j))) {
				auto& box = std::get<RaylibAdditions::Menu::toggleBox>(settings.at(i).at(j));
				type = "toggleBox";
				name = box.name;
				if (box.state == true)
					data = "true";
				else
					data = "false";
			}
			if (std::holds_alternative<RaylibAdditions::Menu::slider>(settings.at(i).at(j))) {
				auto& slider = std::get<RaylibAdditions::Menu::slider>(settings.at(i).at(j));
				type = "slider";
				name = slider.name;
				data = std::to_string(slider.procentage);
			}

			settingsFile << "   " << type << " | " << name << " | " << data << std::endl;
		}
	}

    settingsFile.close();
}