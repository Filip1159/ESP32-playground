#include "Canvas.h"

#pragma region
void Canvas::drawPixel(uint8_t x, uint8_t y, Color color) {
	if (isBounded && (x > XSIZE || y > YSIZE))
		throw new invalid_argument("Invalid point coordinates: (" + $(x) + ", " + $(y) + ")")
	setAddrWindow(x, y, x, y);
	fillColor(color, 1);
}
#pragma endregion

void Canvas::horizontalLine(uint8_t xLeft, uint8_t xRight, uint8_t y, Color color) {
	if (xLeft > xRight || xLeft > XSIZE || xRight > XSIZE) throw invalid_argument(
		"Invalid horizontal line coordinates: (" + $(xLeft) + ", " + $(xRight) + ")"
	)
	setAddrWindow(xLeft, y, xRight, y);
	fillColor(color, xRight - xLeft + 1);
}