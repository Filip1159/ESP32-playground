set(SOURCES ST7735s.cpp ./graphics/Printer.cpp ./graphics/Canvas.cpp)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS . ./graphics REQUIRES EspException)