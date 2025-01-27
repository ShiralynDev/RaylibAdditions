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
            std::vector<std::string> splitString = functions::splitString(line, " ----- ");

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
        std::vector<std::string> splitString = functions::splitString(line, " ----- ");

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
		DrawTextureEx(*(frame->textures[i]), frame->texturePos[i], 0, frame->textureScales.at(i), WHITE);
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
			if (CheckCollisionPointRec(mousePos, MenuTab) && i != selectedPage) {

				for (auto& element : settings[selectedPage]) { // Close all extended stringlists
                		if (auto value = std::get_if<stringList>(&element)) {
                		value->extended = false;
					}
				}

				selectedPage = i;
				break;
			}
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

		if (auto value = std::get_if<stringList>(&settings.at(selectedPage).at(i))) {
			Rectangle box = {
				settingsEntry.at(i).x + MeasureText(settingsEntryText.at(i).c_str(), entryFontSize) + 10,
				settingsEntry.at(i).y,
				float(entryFontSize) * 5.0f,
				float(entryFontSize)
			};

			DrawRectangleLinesEx(box, float(entryFontSize) / 10.0f, BLACK);
			RaylibAdditions::drawTextLeftCenterRect(box, value->items.at(0), 20, BLACK, 10.0f);

			if (value->extended) { // All -/+ are to adjust for the selected value being at position 0
				std::vector<Rectangle> boxes;
				int amountOfBoxes = value->items.size() - 1; // Rewrite with scrolling later or offer it as diffrent style

				for (int j = 0; j < amountOfBoxes; j++) {
					Rectangle newBox = box;
					if (j != 0) 
						newBox = boxes.at(j-1);
					newBox.y += newBox.height - float(entryFontSize) / 10.0f; // float(entryFontSize) / 10.0f remove outline
					boxes.push_back(newBox);
				}

				for (int j = 0; j < boxes.size(); j++) {
					DrawRectangleLinesEx(boxes.at(j), float(entryFontSize) / 10.0f, BLACK);
					RaylibAdditions::drawTextLeftCenterRect(boxes.at(j), value->items.at(j + 1), 20, BLACK, 10.0f);
				}

				if (IsMouseButtonPressed(0)) {
					for (int j = 0; j < boxes.size(); j++) {
						if(CheckCollisionPointRec(GetMousePosition(), boxes.at(j))) {
							std::string target = value->items.at(j + 1);
							value->items.erase(value->items.begin() + j + 1);
							value->items.insert(value->items.begin(), target);
							value->extended = false;
						}
					}
				}

			}
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

					if (auto value = std::get_if<stringList>(&it)) {
						if (IsMouseButtonPressed(0)) {
							value->extended = !value->extended;
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

void RaylibAdditions::Menu::Menu::removeSettingFromPage(std::string page, std::string name) {
    for (size_t i = 0; i < pageTitles.size(); i++) {
        if (pageTitles.at(i) == page) {

            auto& pageSettings = settings.at(i);
            for (auto it = pageSettings.begin(); it != pageSettings.end(); ) {
                bool shouldErase = false;

                std::visit([&](auto&& contained) {
                    if (contained.name == name) {
                        shouldErase = true;
                    }
                }, *it);

                if (shouldErase) {
                    it = pageSettings.erase(it);
                } else {
                    ++it;
                }
            }

        }
    }
}

void RaylibAdditions::Menu::Menu::setSettingAtPage(std::string page, std::string name, std::variant<toggleBox, slider, stringList> setting) {
    for (size_t i = 0; i < pageTitles.size(); i++) {
        if (pageTitles.at(i) == page) {

            auto& pageSettings = settings.at(i);
            for (auto it = pageSettings.begin(); it != pageSettings.end(); ) {
                bool found = false;

                std::visit([&](auto&& contained) {
                    if (contained.name == name) {
                        found = true;
                    }
                }, *it);

                if (found) {
					*it = setting;
					return;
                } else {
                    ++it;
                }
            }

        }
    }
}

std::variant<RaylibAdditions::Menu::toggleBox, RaylibAdditions::Menu::slider, RaylibAdditions::Menu::stringList>* RaylibAdditions::Menu::Menu::getVariant(std::string page, std::string name) {
        
	auto pageIter = std::find(pageTitles.begin(), pageTitles.end(), page);
	if (pageIter == pageTitles.end()) {
		return nullptr;  // Page not found.
	}

	auto& pageSettings = settings.at(std::distance(pageTitles.begin(), pageIter));

	for (auto& setting : pageSettings) {
		bool found = false;
		std::visit([&](auto&& contained) {
			if (contained.name == name) {
				found = true;
			}
		}, setting);

		if (found) {
			return &setting;
		}
	}

	return nullptr;
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

		std::vector<std::string> data = functions::splitString(line, " | "); // data[2] is the value
		data[0].erase(std::remove(data[0].begin(), data[0].end(), ' '), data[0].end());

		if(data[0] == "toggleBox") {
			if (data[2] == "true")
				addSettingToPage(page, RaylibAdditions::Menu::toggleBox(data[1], true));
			else
				addSettingToPage(page, RaylibAdditions::Menu::toggleBox(data[1], false));
		}

		else if (data[0] == "slider") 
			addSettingToPage(page, RaylibAdditions::Menu::slider(data[1], std::stoi(data[2])));

		else if (data[0] == "stringList") {
			std::vector<std::string> stringList = functions::splitString(data[2], ", ");
			addSettingToPage(page, RaylibAdditions::Menu::stringList(data[1], stringList) );
		}

    }
    settingsFile.close();
}

std::unordered_map<std::string, std::variant<bool, int, std::string>> RaylibAdditions::Menu::loadSettingsFromFileToMap(std::string path) {
	std::ifstream settingsFile(path);

	std::unordered_map<std::string, std::variant<bool, int, std::string>> returnMap;

	if (!settingsFile.is_open()) {
		std::cerr << "Could not find file: " << path << std::endl;
		return returnMap; // Exit like nothing happend and the rest of the program will just have to enjoy and empty map :)
	}

	std::string line;
	while ( getline(settingsFile, line) ) {
    	std::cout << line << '\n';

		if (line.find('[') == 0 && line.find(']') == line.size() - 1) { // Page name
			continue;
        }

		std::vector<std::string> data = functions::splitString(line, " | "); // data[2] is the value
		data[0].erase(std::remove(data[0].begin(), data[0].end(), ' '), data[0].end());

		if(data[0] == "toggleBox") {
			if (data[2] == "true")
				returnMap.emplace(data[1], true);
			else
				returnMap.emplace(data[1], false);
		}

		else if (data[0] == "slider") 
			returnMap.emplace(data[1], std::stoi(data[2]));

		else if (data[0] == "stringList") {
			std::vector<std::string> stringList = functions::splitString(data[2], ", ");
			returnMap.emplace(data[1], stringList.at(0));
		}

    }

    settingsFile.close();
	return returnMap;
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

			if (std::holds_alternative<RaylibAdditions::Menu::stringList>(settings.at(i).at(j))) {
				auto& stringList = std::get<RaylibAdditions::Menu::stringList>(settings.at(i).at(j));
				type = "stringList";
				name = stringList.name;

				for (int k = 0; k < stringList.items.size(); k++) {
					data += stringList.items.at(k);
					if (k != stringList.items.size() - 1)
						data += ", ";
				}
			}

			settingsFile << "   " << type << " | " << name << " | " << data << std::endl;
		}
	}

    settingsFile.close();
}

int RaylibAdditions::ScrollingMenu::drawAndUpdate(Sound* optionChangeSound, int gamepad, bool drawWithoutUpdate) {
	 for (int i = 0; i < options.size(); i++) {
		if (i == selectedText) {
			DrawText(options.at(i).c_str(), 10, ((1080 / options.size()) * i) + 37.5, selectedTextSize, selectedColor);
			continue;
		}
		DrawText(options.at(i).c_str(), 10, ((1080/options.size()) * i) + 25, textSize, textColor);
    }

	float gamepadJoystick = GetGamepadAxisMovement(gamepad, 1);

	if (IsKeyPressed(KEY_W) && !drawWithoutUpdate || IsKeyPressed(KEY_UP) && !drawWithoutUpdate || gamepadJoystick < -0.5 && lastGamepadAxis > -0.5 && !drawWithoutUpdate) {
		
		if (optionChangeSound != nullptr)
			PlaySound(*(optionChangeSound));

		if (selectedText == 0) {
			selectedText = options.size() - 1;
		} else {
			selectedText--;
		}
	}

	if (IsKeyPressed(KEY_S) && !drawWithoutUpdate || IsKeyPressed(KEY_DOWN) && !drawWithoutUpdate || gamepadJoystick > 0.5 && lastGamepadAxis < 0.5 && !drawWithoutUpdate) {
		
		if (optionChangeSound != nullptr)
			PlaySound(*(optionChangeSound));

		if (selectedText == options.size() - 1) {
			selectedText = 0;
		} else {
			selectedText++;
		}
	}

	lastGamepadAxis = gamepadJoystick;

	return selectedText;
}