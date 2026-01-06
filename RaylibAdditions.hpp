#pragma once

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ALIGN_UP 2
#define ALIGN_DOWN 3

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

#include "DrawClasses.hpp"

namespace RaylibAdditions { // Define classes here
	// Window class, construct using title, width, heigth. All args can be changed and then run updateWindow to resize and rename window
	class WindowClass {
	public:
		std::string title;
		int width;
		int height;

		WindowClass(std::string windowTitle, int windowWidth, int windowHeight, int monitor = 0) 
		: title(windowTitle), width(windowWidth), height(windowHeight) {
			InitWindow(width, height, title.c_str());
			SetWindowMonitor(monitor);
		};

		WindowClass(std::string windowTitle, Vector2 windowSize, int monitor = 0) 
		: title(windowTitle), width(windowSize.x), height(windowSize.y) {
			InitWindow(width, height, title.c_str());
			SetWindowMonitor(monitor);
		};

		~WindowClass() {
			CloseWindow();
		}

		void updateWindow() {
			SetWindowTitle(title.c_str());
			SetWindowSize(width, height);
		}
	};
	
	// Button class, consturct using all args (some are optional)
	class ButtonClass {
	public:
		Rectangle rect;
		std::string text;
		int textSize;
		Color color;
		Color outlineColor;
		Color textColor;
		int outlineThickness;
		float scale;
		// state, 0 = off, 1 = hover, 2 = pressed, 3 = released (only one cycle)
		int state;
		//optional sound
		Sound pressedSound;
		Sound releasedSound;

		void updateState();

		ButtonClass(Rectangle buttonRect, std::string buttonText, int buttonTextSize, Color buttonColor, Color buttonOutlineColor, Color buttonTextColor, int buttonOutlineThickness, float buttonScale, Sound buttonPressedSound = Sound(), Sound buttonReleasedSound = Sound(), int buttonState = 0)
		: rect(buttonRect), text(buttonText), textSize(buttonTextSize), color(buttonColor), outlineColor(buttonOutlineColor), textColor(buttonTextColor), outlineThickness(buttonOutlineThickness), scale(buttonScale), state(buttonState), pressedSound(buttonPressedSound), releasedSound(buttonReleasedSound) {}
	};

	class LoadedButtonClass { // rename to like textured button or something
	public:
		Texture2D texture;
		Vector2 pos;
		float scale;
		// state, 0 = off, 1 = hover, 2 = pressed, 3 = released (only one cycle)
		int state;
		//optional sound
		Sound pressedSound;
		Sound releasedSound;

		void drawAndUpdate();

		LoadedButtonClass(Texture2D buttonTexture, Vector2 buttonPos, float buttonScale, Sound buttonPressedSound = Sound(), Sound buttonReleasedSound = Sound(), int buttonState = 0)
		: texture(buttonTexture), pos(buttonPos), scale(buttonScale), state(buttonState), pressedSound(buttonPressedSound), releasedSound(buttonReleasedSound) {}
	};

	class DrawClass { // Experimental use only for testing, more a preformance lost than boost
		std::vector<std::variant<
		Rectangle,
		DrawStructs::DrawTextCenterRectStruct,
		DrawStructs::DrawRectRectStruct,
		DrawStructs::DrawTextureStruct
		>> list;

		public:
		
		void pushList(std::variant<Rectangle, DrawStructs::DrawTextCenterRectStruct, DrawStructs::DrawRectRectStruct, DrawStructs::DrawTextureStruct> item);
		std::variant<Rectangle, DrawStructs::DrawTextCenterRectStruct, DrawStructs::DrawRectRectStruct, DrawStructs::DrawTextureStruct> popList();
		void clearList();
		void drawList();
	};

	class SpeedometerClass {
		private:
			const float lowestValue = 0.0;
			const float heighestValue = 10.0;
			const int startAngel = 0; // from right
			const int stopAngel = 180;
			const int indents = 5;
			const int segments = 20;
			const Rectangle place;
			const std::string text;
			const Color color;
			const Color needleColor;
		public:
			float value = 0.0;

			SpeedometerClass(Rectangle place, float lowestValue, float heighestValue, int startAngel, int stopAngel, int indents, std::string text, Color color, Color needleColor) 
			: place(place), lowestValue(lowestValue), heighestValue(heighestValue), startAngel(startAngel), stopAngel(stopAngel), indents(indents), text(text), color(color), needleColor(needleColor) {}
	
			void drawSpeedometer();
			void drawNeedle(Color needle, float needleValue, float lenghtProcentage); // draws an extra needle
	};
}

namespace RaylibAdditions { // Define functions here
	// Draws text in the Y center of a Rectangle and to the X left in the Rectangle
	void drawTextLeftCenterRect(Rectangle& rect, std::string& text, int fontSize, Color color);
	void drawTextLeftCenterRect(Rectangle &rect, std::string &text, int fontSize, Color color, float xOffset);
	// Draws text in the X center of a Rectangle and to the Y top in the Rectangle
	void drawTextCenterTopRect(Rectangle& rect, std::string& text, int fontSize, Color color);
	void drawTextCenterTopRect(Rectangle& rect, std::string& text, int fontSize, Color color, int topOffset);
	// Draws text in the center of a Rectangle 
	void drawTextCenterRect(Rectangle& rect, std::string& text, int fontSize, Color color);
	// Draws text in the center of Rectangle custom text args
	void drawTextCenterRect(Rectangle& rect, std::string& text, int fontsize, float spacing, Color tint, Font font = GetFontDefault());
	// Draws a Rectangle with an outline
	void drawRectWOutline(Rectangle& rect, float lineThick, Color color, Color outlineColor);
	// Draws a Rectangle with an outline and text
	void drawRectWOutlineWText(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, std::string& text, int fontSize, Color textColor);
	// Draws a Rectangle with an outline and text but top center
	void drawRectWOutlineWTextCenterTop(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, std::string& text, int fontSize, Color textColor);
	// Draws a Rectangle with an outline and text using custom text args
	void drawRectWOutlineWText(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, std::string& text, int fontSize, Color textColor, float textSpacing, Font font = GetFontDefault());
	// Draws a rounded Rectangle with an outline and text using custom text args
	void drawRectRoundedWOutlineWText(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, std::string& text, int fontSize, Color textColor, float roundness, int segments);
	// Draws a Rounded Rectangle with an outline
	void drawRectRoundedWOutline(Rectangle& rect, float lineThick, Color rectColor, Color outlineColor, float roundness, int segments);
	// Draws a line aligned to a direction instead of using the cords as the middle (made for flat lines)
	void drawLineAligned(Vector2 startPos, Vector2 endPos, float lineThick, Color color, int alignment);
	// Inserts \n where it is needed
    std::string formatString(std::string string, int length, int fontSize);
	// Draws a button using the ButtonClass
	void drawButton(RaylibAdditions::ButtonClass* button);
	// Draws all the buttons in a std::unordererd_map
	void drawButtons(std::unordered_map<std::string, ButtonClass>* buttons);
	// Updates the state of all buttons in a std::unordererd_map
	void updateButtonstates(std::unordered_map<std::string, ButtonClass>* buttons);
	// updates the state of all buttons in a std::unordered_map but using a camera and does getScreenToWorld
	void updateButtonstates(std::unordered_map<std::string, ButtonClass>* buttons, Camera2D* camera);
	// updates the state of a single button
	void updateButtonstate(ButtonClass* button, bool useIsMouseDown = false);
	// Creates a normal Camera2D
	Camera2D createCamera();
	// Creates camera zooming into game area using getScreenHeight()
	Camera2D createCamera(Vector2 targetArea);
	// Updates the zoom of a camera using height and width trying to keep a 16:9 aspect ratio
	void updateCamera(Camera2D* camera, Vector2 targetArea);
	// Draws an FPS counter at the position supplied 1 = top left, 2 = top middle, 3 = top right, 4 = middle left and so on
	void drawFPSCounter(int position, int fontSize, Color color);
}