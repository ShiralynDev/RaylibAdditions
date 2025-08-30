#include "RaylibAdditions.hpp"
#include "Functions.hpp"

#include <raylib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <typeinfo>

void RaylibAdditions::ButtonClass::updateState() {
	if (CheckCollisionPointRec(GetMousePosition(), rect)) {
		state = 1;
		if (IsMouseButtonPressed(0)) {
			state = 2;
			if(IsSoundValid(pressedSound))
				PlaySound(pressedSound);
		}
	}
	else
		state = 0;
}

void RaylibAdditions::drawTextLeftCenterRect(Rectangle &rect, std::string &text, int fontSize, Color color) {
	DrawText(text.c_str(),
		rect.x,
		(rect.y + rect.height / 2) - fontSize / 2, 
		fontSize,
		color
		);
}

void RaylibAdditions::drawTextLeftCenterRect(Rectangle &rect, std::string &text, int fontSize, Color color, float xOffset) {
	DrawText(text.c_str(),
		rect.x + xOffset,
		(rect.y + rect.height / 2) - fontSize / 2, 
		fontSize,
		color
		);
}

void RaylibAdditions::drawTextCenterTopRect(Rectangle &rect, std::string &text, int fontSize, Color color) {
	DrawText(text.c_str(),
		(rect.x + rect.width / 2) - MeasureText(text.c_str(), fontSize) / 2,
		rect.y, 
		fontSize,
		color
		);
}

void RaylibAdditions::drawTextCenterTopRect(Rectangle &rect, std::string &text, int fontSize, Color color, int topOffset) {
	DrawText(text.c_str(),
		(rect.x + rect.width / 2) - MeasureText(text.c_str(), fontSize) / 2,
		rect.y + topOffset, 
		fontSize,
		color
		);
}


void RaylibAdditions::drawTextCenterRect(Rectangle &rect, std::string &text, int fontSize, Color color) {
	DrawText(text.c_str(),
		(rect.x + rect.width / 2) - MeasureText(text.c_str(), fontSize) / 2,
		(rect.y + rect.height / 2) - fontSize / 2, 
		fontSize,
		color
		);
}

void RaylibAdditions::drawTextCenterRect(Rectangle& rect, std::string& text, int fontsize, float spacing, Color tint, Font font) {
	DrawTextEx(font,
		text.c_str(),
		{ (rect.x + rect.width / 2) - MeasureTextEx(font, text.c_str(), fontsize, spacing).x / 2, 
		(rect.y + rect.height / 2) + MeasureTextEx(font, text.c_str(), fontsize, spacing).y / 2},
		fontsize,
		spacing,
		tint
		);
}

void RaylibAdditions::drawRectWOutline(Rectangle& rect, float lineThick, Color color, Color outlineColor) {
	DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
	DrawRectangleLinesEx(rect, lineThick, outlineColor);
}

void RaylibAdditions::drawRectWOutlineWText(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, std::string& text, int fontSize, Color textColor) {
	drawRectWOutline(rect, lineThick, rectColor, outlineColor);
	drawTextCenterRect(rect, text, fontSize, textColor);
}

void RaylibAdditions::drawRectWOutlineWTextCenterTop(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, std::string& text, int fontSize, Color textColor) {
	drawRectWOutline(rect, lineThick, rectColor, outlineColor);
	drawTextCenterTopRect(rect, text, fontSize, textColor, lineThick);
}

void RaylibAdditions::drawRectWOutlineWText(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, std::string& text, int fontSize, Color textColor, float textSpacing, Font font) {
	drawRectWOutline(rect, lineThick, rectColor, outlineColor);
	drawTextCenterRect(rect, text, fontSize, textSpacing, textColor, font);
}

void RaylibAdditions::drawRectRoundedWOutline(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, float roundness, int segments) {
	DrawRectangleRounded(rect, roundness, segments, rectColor);
	DrawRectangleRoundedLinesEx(rect, roundness, segments, lineThick, outlineColor);
}

void RaylibAdditions::drawButton(RaylibAdditions::ButtonClass* button) {
	drawRectWOutlineWText(button->rect, button->outlineThickness, button->color, button->outlineColor, button->text, button->textSize, button->textColor);
}

void RaylibAdditions::drawButtons(std::unordered_map<std::string, ButtonClass>* buttons) {
	for (int i = 0; i < buttons->size(); i++) {
		auto it = std::next(buttons->begin(), i);
		drawButton(&it->second);
	}
}

void RaylibAdditions::updateButtonstates(std::unordered_map<std::string, ButtonClass>* buttons) {
	for (int i = 0; i < buttons->size(); i++) {
		auto it = std::next(buttons->begin(), i);
		if (CheckCollisionPointRec(GetMousePosition(), it->second.rect)) {
			it->second.state = 1;
			if (IsMouseButtonPressed(0)) {
				it->second.state = 2;
				if(IsSoundValid(it->second.pressedSound))
					PlaySound(it->second.pressedSound);
			}
		}
		else
			it->second.state = 0;
	}
}

void RaylibAdditions::updateButtonstates(std::unordered_map<std::string, ButtonClass>* buttons, Camera2D* camera) {
	for (int i = 0; i < buttons->size(); i++) {
		auto it = std::next(buttons->begin(), i);
		if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), *(camera)), it->second.rect)) {
			it->second.state = 1;
			if (IsMouseButtonPressed(0)) {
				it->second.state = 2;
				if(IsSoundValid(it->second.pressedSound))
					PlaySound(it->second.pressedSound);
			}
		}
		else
			it->second.state = 0;
	}
}

Camera2D RaylibAdditions::createCamera() {
	Camera2D camera{};
	camera.target = Vector2{ 0, 0 };
	camera.offset = Vector2{ 0, 0 };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;
	return camera;
}

Camera2D RaylibAdditions::createCamera(int gameHeight) {
	Camera2D camera{};
	camera.target = Vector2{ 0, 0 };
	camera.offset = Vector2{ 0, 0 };
	camera.rotation = 0.0f;
	camera.zoom = float(GetScreenHeight()) / float(gameHeight);
	return camera;
}

void RaylibAdditions::drawFPSCounter(int position, int fontSize, Color color) {
	int positionX = 0, positionY = 0;
	std::string FPSString = std::to_string(GetFPS());
	if (position == 2 || position == 5 || position == 8)
		positionX = float(GetScreenWidth()) / 2.0 - float(MeasureText(FPSString.c_str(), fontSize)) / 2.0;
	if (position == 3 || position == 6 || position == 9)
		positionX = float(GetScreenWidth()) - float(MeasureText(FPSString.c_str(), fontSize));
	if (position == 4 || position == 5 || position == 6)
		positionY = float(GetScreenHeight()) / 2.0 - fontSize / 2.0;
	if (position == 7 || position == 8 || position == 9)
		positionY = float(GetScreenHeight()) - fontSize;
	DrawText(FPSString.c_str(), positionX, positionY, fontSize, color);
	return;
}

void RaylibAdditions::DrawClass::pushList(std::variant<Rectangle, DrawStructs::DrawTextCenterRectStruct, DrawStructs::DrawRectRectStruct, DrawStructs::DrawTextureStruct> item) {
	list.push_back(item);
}

std::variant<Rectangle, DrawStructs::DrawTextCenterRectStruct, DrawStructs::DrawRectRectStruct, DrawStructs::DrawTextureStruct> RaylibAdditions::DrawClass::popList() {
	if (list.empty())
		return Rectangle(); // this is dumb, should be null

	std::variant<Rectangle, DrawStructs::DrawTextCenterRectStruct, DrawStructs::DrawRectRectStruct, DrawStructs::DrawTextureStruct> lastItem = list.back();
	list.pop_back();
	return lastItem;
}

void RaylibAdditions::DrawClass::clearList() {
	list.clear();
}

// Dont know how much better/worse this is but might be worth it for textures
void RaylibAdditions::DrawClass::drawList() {
	std::vector<Rectangle*> rectsThatAreDrawn;

	for (const auto& element : list) {
        if (std::holds_alternative<DrawStructs::DrawRectRectStruct>(element)) {
			DrawStructs::DrawRectRectStruct DrawRectRectStruct = std::get<DrawStructs::DrawRectRectStruct>(element);
			rectsThatAreDrawn.push_back(DrawRectRectStruct.rect);
			
			int drawX = DrawRectRectStruct.rect->x;
			int drawY = DrawRectRectStruct.rect->y;
			for (const auto& element : rectsThatAreDrawn) {
				// jump to next rect as this is not behind another one due to it drawing more to the left or the top compared to the current one
				if (drawX < element->x || drawY < element->y) 
					continue;

				// Can't be behind another one if its bigger
				if (DrawRectRectStruct.rect->width > element->width || DrawRectRectStruct.rect->height > element->height)
					continue;

				if (drawX + DrawRectRectStruct.rect->width > element->x + element->width || drawY + DrawRectRectStruct.rect->height > element->y + element->height)
					continue;

				DrawRectangleRec(*(DrawRectRectStruct.rect), DrawRectRectStruct.color);
				break;
			}


        } else if (std::holds_alternative<DrawStructs::DrawTextureStruct>(element)) {
			DrawStructs::DrawTextureStruct DrawTextureStruct = std::get<DrawStructs::DrawTextureStruct>(element);
			Rectangle RectOfTexture = {DrawTextureStruct.pos->x, DrawTextureStruct.pos->y, float(DrawTextureStruct.texture->height), float(DrawTextureStruct.texture->width)};
			rectsThatAreDrawn.push_back(&RectOfTexture); // This is prob stupid and list might work better as rect list instead of rect* list
			
			int drawX = RectOfTexture.x;
			int drawY = RectOfTexture.y;
			for (const auto& element : rectsThatAreDrawn) {
				// jump to next rect as this is not behind another one due to it drawing more to the left or the top compared to the current one
				if (drawX < element->x || drawY < element->y) 
					continue;

				// Can't be behind another one if its bigger
				if (RectOfTexture.width > element->width || RectOfTexture.height > element->height)
					continue;

				if (drawX + RectOfTexture.width > element->x + element->width || drawY + RectOfTexture.height > element->y + element->height)
					continue;

				DrawTextureEx(*(DrawTextureStruct.texture), *(DrawTextureStruct.pos), 0, 1, WHITE);
				break;
			}

		}
	}

}