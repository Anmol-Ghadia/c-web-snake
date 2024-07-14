#ifndef MAIN_H
#define MAIN_H

// Libs
#include "raylib.h"

// Helper
#include "snake.h"

// CONFIG GLOBALS
const double g_snake_movement_time = 0.4;
const double g_margin_ratio = 0.35;
const double g_head_margin_ratio = 0.30;

// GLOBALS
int g_extern_touch_x = 0;
int g_extern_touch_y = 0;
int g_screen_width = 900;
int g_screen_height = 600;
int g_margin_size;
int g_vertical_grid_count;
int g_horizontal_grid_count;
double g_last_snake_movement_time = 0;
int g_grid_size = 80;
unsigned int g_score;
enum GameState g_game_state = MENU;
Position g_food = {0,0};
bool g_is_dark_theme = false;
Rectangle g_screen_padding = {0,0,0,0};
Color g_background_color = WHITE;
Color g_grid_color = GRAY;
Color g_snake_head_color = RED;
Color g_score_color = BLACK;
Color g_pause_color = BLACK;
Color g_border_color = BLACK;
Color g_background_outisde_color = GRAY;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#endif
