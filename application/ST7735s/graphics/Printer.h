#pragma once

#include "stdint.h"

#include "Color.h"

class ST7735s;

enum Mode { Horizontal, Portrait };
enum FontSize { Small, Big };

class Printer {
private:
    static const uint8_t FONT_CHARS[96][5];
    static const uint8_t BIG_FONT[256][20];
    uint8_t cursorX, cursorY;

    ST7735s* display;
    Mode mode;
    FontSize fontSize;

    void renderBigChar(char ch, Color color);
    void renderSmallChar(char ch, Color color);

public:
    Printer(ST7735s* display) :cursorX(0), cursorY(0), display(display), fontSize(Small) {}

    void writeInt(int i, Color color);
    void writeHex(int i, Color color);
    void writeString(const char* text, Color color);
    void writeChar(char ch, Color color);

    void setFontSize(FontSize fontSize);
    void setCursor(uint8_t x, uint8_t y);
    void setLine(uint8_t y);
    void advanceCursor();
};