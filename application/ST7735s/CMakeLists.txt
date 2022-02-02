set(SOURCES ST7735s.cpp)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS . REQUIRES EspException)