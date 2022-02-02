#pragma once

class Pixel {
private:
    int x, y;
    bool isBounded;
    static const xMax, yMax;

public:
    Pixel(int x, int y) :x(x), y(x), isBounded(false) {}
    Pixel(int x, int y, bool isBounded): x(x), y(y), isBounded(isBounded) {}
    
}