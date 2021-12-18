#pragma once

typedef enum {
  Black = 0x000000,
  Blue = 0x0000FF,
  BlueViolet = 0x8A2BE2,
  Cyan = 0x00FFFF,
  DarkBlue = 0x00008B,
  DarkCyan = 0x008B8B,
  Green = 0x008000,
  GreenYellow = 0xADFF2F,
  HotPink = 0xFF69B4,
  LightBlue = 0xADD8E6,
  LightCoral = 0xF08080,
  LightCyan = 0xE0FFFF,
  LightGoldenrodYellow = 0xFAFAD2,
  LightGreen = 0x90EE90,
  LightGrey = 0xD3D3D3,
  LightPink = 0xFFB6C1,
  LightSalmon = 0xFFA07A,
  LightSeaGreen = 0x20B2AA,
  LightSkyBlue = 0x87CEFA,
  LightSlateGray = 0x778899,
  LightSlateGrey = 0x778899,
  LightSteelBlue = 0xB0C4DE,
  LightYellow = 0xFFFFE0,
  Lime = 0x00FF00,
  LimeGreen = 0x32CD32,
  Magenta = 0xFF00FF,
  Orange = 0xFFA500,
  Pink = 0xFFC0CB,
  Purple = 0x800080,
  Red = 0xFF0000,
  Teal = 0x008080,
  Wheat = 0xF5DEB3,
  White = 0xFFFFFF,
  WhiteSmoke = 0xF5F5F5,
  Yellow = 0xFFFF00,
  YellowGreen = 0x9ACD32,
} Color;

void led_manager_setup();

void show_led_color(const Color& color);

void led_manager_loop();
