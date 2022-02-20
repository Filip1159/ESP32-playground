#include "Canvas.h"
#include "ST7735s.h"
#include "math.h"

void Canvas::drawBitmap(const uint8_t bitmap[], uint8_t width, uint8_t height) {
	display->setAddrWindow(0, 0, width-1, height-1);
	display->sendCommand(RamWrite);
	for (uint8_t row = 0; row < height; row++) {
		for (uint16_t col = 0; col < width*2; col+=2) {
			uint32_t i = row * width * 2 + col;
			display->sendData16Bit((uint16_t)bitmap[i] * 256 + bitmap[i+1]);
		}
	}
}

void Canvas::fillEllipse(int xCenter,int yCenter, int width, int height, Color color) {
	int xRadius = width/2, yRadius = height/2;
	int xRight, xLeft = xRadius, y = 1, dx = 0;
	uint32_t xRadius2 = xRadius * xRadius, yRadius2 = yRadius * yRadius;
	uint32_t radiuses22 = xRadius2 * yRadius2;
	drawHorizontalLine(xCenter - xRadius, xCenter + xRadius, yCenter, color);
	while (y <= yRadius) {
		for (xRight = xLeft - dx + 1; xRight > 0; xRight--)
			if (yRadius2 * xRight * xRight + xRadius2 * y * y <= radiuses22)
				break;
		dx = xLeft - xRight;
		xLeft = xRight;
		drawHorizontalLine(xCenter - xLeft, xCenter + xLeft, yCenter + y, color);
		drawHorizontalLine(xCenter - xLeft, xCenter + xLeft, yCenter - y, color);
		y++;
	}
}

void Canvas::drawEllipse(int xCenter, int yCenter, int width, int height, Color color) {
// two-part Bresenham method
	int xRadius = width/2, yRadius = height/2;
	int x = 0, y = yRadius;
	long xRadius2 = (uint32_t) xRadius * xRadius;
	long yRadius2 = (uint32_t) yRadius * yRadius;
	long error = (uint32_t) xRadius2 * yRadius;
	long stopY = 0, stopX = 2 * xRadius2 * yRadius;
	while (stopY <= stopX) {
		drawPixel(xCenter + x, yCenter + y, color);
		drawPixel(xCenter + x, yCenter - y, color);
		drawPixel(xCenter - x, yCenter + y, color);
		drawPixel(xCenter - x, yCenter - y, color);
		x++;
		error -= 2 * yRadius2 * (x - 1);
		stopY += 2 * yRadius2;
		if (error < 0) {
			error += 2 * xRadius2 * (y - 1);
			y--;
			stopX -= 2 * xRadius2;
		}
	}
	x = xRadius; y = 0; error = yRadius2 * xRadius;
	stopY = 2 * yRadius2 * xRadius; stopX = 0;
	while (stopY >= stopX) {
		drawPixel(xCenter + x, yCenter + y, color);
		drawPixel(xCenter + x, yCenter - y, color);
		drawPixel(xCenter - x, yCenter + y, color);
		drawPixel(xCenter - x, yCenter - y, color);
		y++;
		error -= 2 * xRadius2 * (y - 1);
		stopX += 2 * xRadius2;
		if (error < 0) {
			error += 2 * yRadius2 * (x - 1);
			x--;
			stopY -= 2 * yRadius2;
		}
	}
}

void Canvas::fillCircle(uint8_t xCenter, uint8_t yCenter, uint8_t r, Color color) {
	uint16_t r2 = (uint16_t)r * r;
	for (int x = 0; x <= r; x++) {
		uint8_t y = intsqrt(r2 - (uint16_t)x * x);
		uint8_t yUpper = yCenter - y;
		uint8_t yLower = yCenter + y;
		drawVerticalLine(xCenter + x, yUpper, yLower, color);
		drawVerticalLine(xCenter - x, yUpper, yLower, color);
	}
}

void Canvas::drawRoundRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, uint8_t r, Color color) {
	drawHorizontalLine(xLeft + r, xRight - r, yUp, color);
	drawHorizontalLine(xLeft + r, xRight - r, yDown, color);
	drawVerticalLine(xLeft + r, yUp + r, yDown - r, color);
	drawVerticalLine(xRight, yUp + r, yDown - r, color);
	circleQuadrant(xLeft + r, yUp + r, r, UpperLeft, color);
	circleQuadrant(xRight - r, yUp + r, r, UpperRight, color);
	circleQuadrant(xLeft + r, yDown - r, r, LowerLeft, color);
	circleQuadrant(xRight - r, yDown - r, r, LowerRight, color);
}

void Canvas::drawCircle(int x, int y, int r, Color color) {
	circleQuadrant(x, y, r, LowerRight, color);
	circleQuadrant(x, y, r, UpperRight, color);
	circleQuadrant(x, y, r, LowerLeft, color);
	circleQuadrant(x, y, r, UpperLeft, color);
}

void Canvas::circleQuadrant(int16_t xCenter, int16_t yCenter, uint8_t r, Quad quad, Color color) {
    uint16_t r2 = (uint16_t)r * r;
	for (uint8_t x = 0; x <= r; x++) {
		uint8_t y = intsqrt(r2 - x*x);
        uint8_t yUpper = yCenter - y;
		uint8_t yLower = yCenter + y;
        switch (quad) {
            case LowerRight:
                drawPixel(xCenter + x, yLower, color);
                break;
            case UpperRight:
                drawPixel(xCenter + x, yUpper, color);
                break;
            case LowerLeft:
                drawPixel(xCenter - x, yLower, color);
                break;
            case UpperLeft:
                drawPixel(xCenter - x, yUpper, color);
        }
	}
}

void Canvas::fillRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, Color color) {
	uint8_t width = xRight - xLeft + 1;
	uint8_t height = yDown - yUp + 1;
	display->setAddrWindow(xLeft, yUp, xRight, yDown);
	display->fillColor(color, width * height);
}

void Canvas::drawRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, Color color) {
	drawHorizontalLine(xLeft, xRight, yUp, color);
	drawHorizontalLine(xLeft, xRight, yDown, color);
	drawVerticalLine(xLeft, yUp, yDown, color);
	drawVerticalLine(xRight, yUp, yDown, color);
}

void Canvas::drawLine(int xLeft, int yUp, int xRight, int yDown, Color color) { // Bresenham algorithm
	int dx = fabs(xRight - xLeft), sx = xLeft < xRight ? 1 : -1;
	int dy = fabs(yDown - yUp), sy = yUp < yDown ? 1 : -1;
	int err = (dx > dy ? dx : -dy)/2, e2;
	while (true) {
		drawPixel(xLeft, yUp, color);
		if (xLeft == xRight && yUp == yDown) break;
		e2 = err;
		if (e2 >- dx) { err -= dy; xLeft += sx; }
		if (e2 < dy) { err += dx; yUp += sy; }
	}
}

void Canvas::drawVerticalLine(uint8_t x, uint8_t yUp, uint8_t yDown, Color color) {
	display->setAddrWindow(x, yUp, x, yDown);
	display->fillColor(color, yDown - yUp + 1);
}

void Canvas::drawHorizontalLine(uint8_t xLeft, uint8_t xRight, uint8_t y, Color color) {
	display->setAddrWindow(xLeft, y, xRight, y);
	display->fillColor(color, xRight - xLeft + 1);
}

void Canvas::drawPixel(uint8_t x, uint8_t y, Color color) {
    if (x > XSIZE || y > YSIZE) return;
	display->setAddrWindow(x, y, x, y);
	display->fillColor(color, 1);
}

uint32_t Canvas::intsqrt(uint32_t val) {
	unsigned long mulMask = 0x0008000;
	unsigned long retVal = 0;
	if (val > 0) {
		while (mulMask != 0) {
			retVal |= mulMask;
			if (retVal*retVal > val)
				retVal &= ~ mulMask;
			mulMask >>= 1;
		}
	}
	return retVal;
}