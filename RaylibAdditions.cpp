#include "RaylibAdditions.hpp"
#include "Functions.hpp"

#include <raylib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <typeinfo>
#include <math.h>

void RaylibAdditions::ButtonClass::draw() {
	drawRectWOutlineWText(this->rect, this->outlineThickness, this->color, this->outlineColor, this->text, this->textSize, this->textColor);
}

bool RaylibAdditions::ButtonClass::updateState(bool useIsMouseDown) {
	if (CheckCollisionPointRec(GetMousePosition(), rect)) {
		state = 1;
		bool pressed = false;
		if (useIsMouseDown && IsMouseButtonDown(0)) pressed = true;
		if (!useIsMouseDown && IsMouseButtonPressed(0)) pressed = true; 
		if (pressed) {
			state = 2;
			if(IsSoundValid(pressedSound))
				PlaySound(pressedSound);
			return true;
		}
	}
	else
		state = 0;
	return false;
}

bool RaylibAdditions::ButtonClass::drawAndUpdate(bool useIsMouseDown) {
	this->draw();
	return this->updateState(useIsMouseDown);
}


void RaylibAdditions::LoadedButtonClass::drawAndUpdate() {
	DrawTexture(texture, pos.x, pos.y, WHITE);
	if (CheckCollisionPointRec(GetMousePosition(), {pos.x, pos.y, float(texture.width), float(texture.height)})) {
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

void RaylibAdditions::drawLineAligned(Vector2 startPos, Vector2 endPos, float lineThick, Color color, int alignment) {
	switch (alignment) {
	case 0:
		startPos.x = startPos.x - lineThick / 2;
		endPos.x = endPos.x - lineThick / 2;
		break;

	case 1:
		startPos.x = startPos.x + lineThick / 2;
		endPos.x = endPos.x + lineThick / 2;
		break;

	case 2:
		startPos.y = startPos.y - lineThick / 2;
		endPos.y = endPos.y - lineThick / 2;
		break;

	case 3:
		startPos.y = startPos.y + lineThick / 2;
		endPos.y = endPos.y + lineThick / 2;
		break;
	
	default:
		break;
	}

	DrawLineEx(startPos, endPos, lineThick, color);
}

std::string RaylibAdditions::formatString(std::string string, int length, int fontSize) { // this code is so dumb
    std::string modString, modString2;
    std::string returnString;
    int processedChars = 0;
	float currentMultiply = 0.5;

    while (processedChars < string.size()) {
        modString = string.substr(processedChars);
		if (MeasureText(modString.c_str(), fontSize) <= length) {
			returnString.append(modString);
			processedChars += modString.size();
		} else {
			currentMultiply = 0.5;
			while (MeasureText(modString.c_str(), fontSize) > length) {
				modString2 = modString.substr(0, modString.size()*currentMultiply);
				if (MeasureText(modString2.c_str(), fontSize) > length) {
					modString = modString2;
				} else { 
					currentMultiply = currentMultiply + (1 - currentMultiply)/2; // this is prob so dumb but im tired rn
					if (currentMultiply >= 0.999) // good enough ig
						break;
				}
			}
			size_t lastSpace = modString.find_last_of(' ');
			returnString.append(modString.substr(0, lastSpace));
			returnString.append("\n");
			processedChars += lastSpace + 1;
		}
    }
    
    return returnString;
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
			if (IsMouseButtonDown(0)) {
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
			if (IsMouseButtonDown(0)) {
				it->second.state = 2;
				if(IsSoundValid(it->second.pressedSound))
					PlaySound(it->second.pressedSound);
			}
		}
		else
			it->second.state = 0;
	}
}

void RaylibAdditions::updateButtonstate(ButtonClass* button, bool useIsMouseDown) {
	if (CheckCollisionPointRec(GetMousePosition(), button->rect)) {
		button->state = 1;
		if (!useIsMouseDown && IsMouseButtonPressed(0)) {
			button->state = 2;
			if(IsSoundValid(button->pressedSound))
				PlaySound(button->pressedSound);
		} else if (useIsMouseDown && IsMouseButtonDown(0)) {
			button->state = 2;
			if(IsSoundValid(button->pressedSound))
				PlaySound(button->pressedSound);
		}
	}
	else
		button->state = 0;
}


Camera2D RaylibAdditions::createCamera() {
	Camera2D camera{};
	camera.target = Vector2{ 0, 0 };
	camera.offset = Vector2{ 0, 0 };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;
	return camera;
}

Camera2D RaylibAdditions::createCamera(Vector2 targetArea) {
	Camera2D camera{};
	camera.target = Vector2{ 0, 0 };
	camera.offset = Vector2{ 0, 0 };
	camera.rotation = 0.0f;

	float screenWidth = float(GetScreenWidth());
	float screenHeight = float(GetScreenHeight());

	float screenAspect = screenWidth / screenHeight;
	float targetAspect = targetArea.x / targetArea.y;
	if (screenAspect < targetAspect)
		camera.zoom = screenWidth / targetArea.x; 
	else
		camera.zoom = screenHeight / targetArea.y;

	return camera;
}

void RaylibAdditions::updateCamera(Camera2D* camera, Vector2 targetArea) {
	float screenWidth = float(GetScreenWidth());
	float screenHeight = float(GetScreenHeight());

	float screenAspect = screenWidth / screenHeight;
	float targetAspect = targetArea.x / targetArea.y;
	if (screenAspect < targetAspect)
		camera->zoom = screenWidth / targetArea.x; 
	else
		camera->zoom = screenHeight / targetArea.y;
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

void RaylibAdditions::SpeedometerClass::drawSpeedometer() {
	int shortestDiameter = 0;
	if (place.height >= place.width) 
		shortestDiameter = place.width; 
	else 
		shortestDiameter = place.height;

	Vector2 midPoint = {place.width/2 + place.x, place.height/2 + place.y};
	// add z var; add segments var instead of indents; add color var;
	DrawTextEx(GetFontDefault(), text.c_str(), {midPoint.x - MeasureTextEx(GetFontDefault(), text.c_str(), 10, 2).x/2, midPoint.y - 20}, 10, 2, color);
	DrawRingLines(midPoint, shortestDiameter/2 - 5, shortestDiameter/2, startAngel, stopAngel, segments, color);

	for (int i = 0; i < indents; ++i) {
		float t = (float)i / (indents - 1);
		float angleDeg = startAngel + t * (stopAngel - startAngel);
		float angleRad = angleDeg * (M_PI / 180.0f);

		float indentWidth = 2;
		float indentHeight = 8;

		float indentRadius = ((shortestDiameter - 5) / 2);
        float indentX = midPoint.x + cos(angleRad) * indentRadius;
        float indentY = midPoint.y + sin(angleRad) * indentRadius;
		DrawRectanglePro({indentX, indentY, indentWidth, indentHeight}, {indentWidth / 2, indentHeight / 2}, angleDeg + 90.0, RED);
		
		int indentValue = ((heighestValue - lowestValue) / float(indents - 1.0)) * i + lowestValue;
		std::string indentValueString = std::to_string(indentValue);
		float textRadius = indentRadius - 10;
		Vector2 textPos = {midPoint.x + cos(angleRad) * textRadius, midPoint.y + sin(angleRad) * textRadius};
		Font font = GetFontDefault();
		float fontSize = 10;
		float spacing = 2;
		Vector2 textSize = MeasureTextEx(font, indentValueString.c_str(), fontSize, spacing);
		DrawTextPro(font, indentValueString.c_str(), {textPos.x, textPos.y}, {textSize.x / 2, textSize.y / 2}, angleDeg + 90.0f, fontSize, spacing, color);
	}

	float valuePos = (value - lowestValue) / (heighestValue - lowestValue);
	float angleDeg = startAngel + valuePos * (stopAngel - startAngel);
	float angleRad = (angleDeg + 90.0f) * (M_PI / 180.0f);
	float radius = (shortestDiameter / 2 - 5) + 10;
	float triangleSize = 5.0f;
	Vector2 p1 = {0, -radius};
	Vector2 p2 = {-triangleSize, triangleSize};
	Vector2 p3 = {triangleSize, triangleSize};

	auto rotatePoint = [](Vector2 pt, float angle) -> Vector2 {
		return {
			pt.x * cos(angle) - pt.y * sin(angle),
			pt.x * sin(angle) + pt.y * cos(angle)
		};
	};

	Vector2 r1 = {
		midPoint.x + rotatePoint(p1, angleRad).x,
		midPoint.y + rotatePoint(p1, angleRad).y
	};
	Vector2 r2 = {
		midPoint.x + rotatePoint(p2, angleRad).x,
		midPoint.y + rotatePoint(p2, angleRad).y
	};
	Vector2 r3 = {
		midPoint.x + rotatePoint(p3, angleRad).x,
		midPoint.y + rotatePoint(p3, angleRad).y
	};
	
	DrawTriangle(r1, r2, r3, needleColor);
}

void RaylibAdditions::SpeedometerClass::drawNeedle(Color needle, float needleValue, float lenghtProcentage) {
	int shortestDiameter = 0;
	if (place.height >= place.width) 
		shortestDiameter = place.width; 
	else 
		shortestDiameter = place.height;
	
	Vector2 midPoint = {place.width/2 + place.x, place.height/2 + place.y};
	float valuePos = (needleValue - lowestValue) / (heighestValue - lowestValue);
	float angleDeg = startAngel + valuePos * (stopAngel - startAngel);
	float angleRad = (angleDeg + 90.0f) * (M_PI / 180.0f);
	float radius = (shortestDiameter / 2 - 5) + 10;
	float triangleSize = 5.0f;
	Vector2 p1 = {0, -radius * lenghtProcentage};
	Vector2 p2 = {-triangleSize, triangleSize};
	Vector2 p3 = {triangleSize, triangleSize};

	auto rotatePoint = [](Vector2 pt, float angle) -> Vector2 {
		return {
			pt.x * cos(angle) - pt.y * sin(angle),
			pt.x * sin(angle) + pt.y * cos(angle)
		};
	};

	Vector2 r1 = {
		midPoint.x + rotatePoint(p1, angleRad).x,
		midPoint.y + rotatePoint(p1, angleRad).y
	};
	Vector2 r2 = {
		midPoint.x + rotatePoint(p2, angleRad).x,
		midPoint.y + rotatePoint(p2, angleRad).y
	};
	Vector2 r3 = {
		midPoint.x + rotatePoint(p3, angleRad).x,
		midPoint.y + rotatePoint(p3, angleRad).y
	};
	
	DrawTriangle(r1, r2, r3, needle);
}