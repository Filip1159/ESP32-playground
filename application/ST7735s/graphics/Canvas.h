#pragma once

class Canvas {
private:
    bool isBounded;
    int maxX, maxY;

public:
    Canvas(): isBounded(false), maxX(0), maxY(0) {}
    Canvas(bool isBounded): isBounded(isBounded), maxX(127), maxY(159) {}
    void setBounds(int maxX, maxY);

    void drawPixel(uint8_t x, uint8_t y, Color color);
}