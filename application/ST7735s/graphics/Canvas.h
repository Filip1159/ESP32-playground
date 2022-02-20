#pragma once

#include "stdint.h"
#include "Color.h"

class ST7735s;

enum Quad {
	LowerRight, UpperRight, LowerLeft, UpperLeft
};

class Canvas {
private:
    ST7735s* display;

    uint32_t intsqrt(uint32_t val);
    void circleQuadrant(int16_t xCenter, int16_t yCenter, uint8_t r, Quad quad, Color color);

public:
    Canvas(ST7735s* display) : display(display) {}

    void drawBitmap(const uint8_t bitmap[], uint8_t width, uint8_t height);
    void fillEllipse(int xCenter,int yCenter, int width, int height, Color color);
    void drawEllipse(int xCenter, int yCenter, int width, int height, Color color);
    void fillCircle(uint8_t xCenter, uint8_t yCenter, uint8_t r, Color color);
    void drawRoundRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, uint8_t r, Color color);
    void drawCircle(int xCenter, int yCenter, int r, Color color);
    void fillRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, Color color);
    void drawRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, Color color);
    void drawLine(int xLeft, int yUp, int xRight, int yDown, Color color);
    void drawVerticalLine(uint8_t x, uint8_t yUp, uint8_t yDown, Color color);
    void drawHorizontalLine(uint8_t xLeft, uint8_t xRight, uint8_t y, Color color);
    void drawPixel(uint8_t x, uint8_t y, Color color);
   
};