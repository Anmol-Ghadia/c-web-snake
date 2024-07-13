// Standard Libs
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
#include "main.h"

// Function defs
void update_draw_frame(void);
void initialize_global_variables();
void draw_snake();
void paint_snake_body_helper(SnakeBody*,SnakeBody*,SnakeBody*);
void handle_key_press();
void change_snake_direction(enum SnakeDirection);
void move_snake();
void update_snake();
void get_updated_vector(enum SnakeDirection, Position, Position*);
void destroy_global_variables();
int round_world_coordinates(int, int);
void place_new_food();
void draw_food();
enum SnakeDirection get_snake_relative_direction(SnakeBody*, SnakeBody*);
void print_error_body(SnakeBody*);
void draw_score();
void draw_playing();
void drawgrid();
void toggle_dark_theme();

// old helper function defs
void drawWindowBoxMarker();
void drawWindowBorder();


// sets the touch input
// used by external api
void give_touch_input(int x, int y) {
    g_extern_touch_x = x;
    g_extern_touch_y = y;
}
// sets snake direction
// used by external
// 0,1,2,3
// represents directions
void give_key_input(unsigned int dir) {
    if (SNAKE == NULL) return;
    switch (dir)
    {
    case 0:
        change_snake_direction(UP);
        break;
    case 1:
        change_snake_direction(RIGHT);
        break;
    case 2:
        change_snake_direction(DOWN);
        break;
    case 3:
        change_snake_direction(LEFT);
        break;
    default:
        break;
    }
}

// ==================================================
// ====== HELPERS ========

// Toggles the dark theme 
void toggle_dark_theme() {
    if (g_is_dark_theme) {
        // set to light
        g_background_color = WHITE;
        g_score_color = BLACK;
        g_pause_color = BLACK;
        g_border_color = BLACK;

    } else {
        // set to dark
        g_background_color = BLACK;
        g_score_color = WHITE;
        g_pause_color = WHITE;
        g_border_color = WHITE;
        
    }
    g_is_dark_theme = !g_is_dark_theme;
}

// Helper function to convert hue to RGB
float hueToRgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0 / 6) return p + (q - p) * 6 * t;
    if (t < 1.0 / 2) return q;
    if (t < 2.0 / 3) return p + (q - p) * (2.0 / 3 - t) * 6;
    return p;
}

// Function to convert HSL to RGB8
Color hslToRgb8(float h, float s, float l) {
    float r, g, b;

    if (s == 0) {
        r = g = b = l; // Achromatic
    } else {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = hueToRgb(p, q, h / 360 + 1.0 / 3);
        g = hueToRgb(p, q, h / 360);
        b = hueToRgb(p, q, h / 360 - 1.0 / 3);
    }

    Color rgb8;
    rgb8.r = (unsigned char)(r * 255);
    rgb8.g = (unsigned char)(g * 255);
    rgb8.b = (unsigned char)(b * 255);
    rgb8.a = 255;

    return rgb8;
}

// Returns a color corresponding to a snake index
Color get_color_for_index(SnakeBody* bodyToPaint) {
    return hslToRgb8(((double)bodyToPaint->index/30)*360,0.75,0.5);
}

// checks if snake as eaten itself
bool is_snake_dead() {
    if (SNAKE->size < 2) return false;

    SnakeBody* head = SNAKE->head;
    SnakeBody* current = head->next;
    for (size_t i = 1; i < SNAKE->size; i++)
    {
        if (head->posX == current->posX && head->posY == current->posY) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Returns true if food is colliding with snake
bool is_food_colliding_snake() {
    SnakeBody* current = SNAKE->head;
    for (size_t i = 0; i < SNAKE->size; i++)
    {
        if (current->posX == g_food.x && current->posY == g_food.y) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Places food at a random location on the map that
//   does not collide the snake
void place_new_food() {
    g_food.x = GetRandomValue(0,g_horizontal_grid_count-1);
    g_food.y = GetRandomValue(0,g_vertical_grid_count-1);
    if (is_food_colliding_snake()) {
        place_new_food();
    }
}

// TODO !!!
// computes the score and updates
void computeScore(int size) {
    int increase = 0;
    if (size < 5) {
        increase = 8;
    } else if (size < 15) {
        increase = 15;
    } else {
        increase = 25;
    }
    g_score += increase;
}

// Checks if the snake has eaten food
bool check_food_eaten() {
    return (SNAKE->head->posX == g_food.x && SNAKE->head->posY == g_food.y);
}


// Draws the title of a window
// ex. Menu screen, pause screen
void draw_window_title(char *ptr_text) {
    
    int titleY = GetScreenHeight()/6;
    int titleFontSize = GetScreenHeight()/6;
    int titleX = (GetScreenWidth()-MeasureText(ptr_text,titleFontSize))/2;

    DrawText(ptr_text,titleX,titleY,titleFontSize,g_background_color);
}

// Draws the button with given text and offset
// Returns the bounding box of text button
Rectangle draw_button(char *ptr_text,unsigned int index) {
    
    int buttonSidePadding = 40;
    int buttonUpDownPadding = 10;
    
    int offsetY = index*(GetScreenHeight()/12 + 2*buttonUpDownPadding+ 10);
    
    int fontY = GetScreenHeight()/2 + offsetY;
    int fontHeight = GetScreenHeight()/12;
    int fontWidth = MeasureText(ptr_text,fontHeight);
    int fontX = (GetScreenWidth()-fontWidth)/2;

    Rectangle buttonBox = {
        fontX - buttonSidePadding,
        fontY-buttonUpDownPadding,
        fontWidth+2*buttonSidePadding,
        fontHeight+2*buttonUpDownPadding
    };

    DrawRectangle(buttonBox.x,buttonBox.y,buttonBox.width,buttonBox.height,g_background_color);
    DrawText(ptr_text,fontX,fontY,fontHeight,RED);
    return buttonBox;
}

// Returns true if the button is clicked
// works for both web and desktop 
bool is_button_clicked(Rectangle button) {
#if defined(PLATFORM_WEB)
    if (g_extern_touch_x != 0 && g_extern_touch_y != 0) {
        Vector2 point = {g_extern_touch_x,g_extern_touch_y};
        if (CheckCollisionPointRec(point,button)) {
            g_extern_touch_x = 0;
            g_extern_touch_y = 0;
            return true;
        }
    }
#else
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(GetMousePosition(),button)) {
            return true;
        }
    }
#endif
    return false;
}

// draws the pause button when game is being played
void draw_pause_button() {

    char titleText[6] = "Pause";

    Rectangle pauseText;
    pauseText.y = 0 + g_margin_size;
    pauseText.height = g_screen_height/16;
    pauseText.width = MeasureText((char *)&titleText,pauseText.height);
    int screen_with_padding = g_grid_size*g_horizontal_grid_count-g_margin_size;
    pauseText.x = (screen_with_padding-MeasureText((char *)&titleText,pauseText.height));

    DrawText((char *)&titleText,pauseText.x,pauseText.y,pauseText.height,g_pause_color);
    if (is_button_clicked(pauseText)) {
        g_game_state = PAUSE;
    }
}

// Handles web input
void handle_input() {
    if (g_extern_touch_x == 0 && g_extern_touch_y == 0) return;

    Vector2 point = {g_extern_touch_x,g_extern_touch_y};
    Vector2 mid = {GetScreenWidth()/2,GetScreenHeight()/2};
    Vector2 topLeft = {0,0};
    Vector2 topRight = {GetScreenWidth(),0};
    Vector2 bottomRight = {GetScreenWidth(),GetScreenHeight()};
    Vector2 bottomLeft = {0,GetScreenHeight()};

    if (CheckCollisionPointTriangle(point,topLeft,mid,topRight)) {
        change_snake_direction(UP);
    } else if (CheckCollisionPointTriangle(point,topRight,mid,bottomRight)) {
        change_snake_direction(RIGHT);
    } else if (CheckCollisionPointTriangle(point,bottomRight,mid,bottomLeft)) {
        change_snake_direction(DOWN);
    } else if (CheckCollisionPointTriangle(point,bottomLeft,mid,topLeft)) {
        change_snake_direction(LEFT);
    }
    g_extern_touch_x = 0;
    g_extern_touch_y = 0;
}

// Draws the score when game is being played
void draw_score() {
    char buffer[10];
    char* ptr_score = &buffer[0];

    sprintf(buffer, "%d", g_score);
    DrawText(ptr_score,g_margin_size,g_margin_size,g_grid_size-g_margin_size,g_score_color);
}

// Free all malloced memory of snake
void destroy_global_variables() {
    if (SNAKE == NULL) return;

    SnakeBody* prev = SNAKE->head;
    SnakeBody* current = prev->next;
    for (size_t i = 1; i < SNAKE->size; i++)
    {
        free(prev);
        prev = current;
        current = current->next;
    }
    free(current);
}

// ===== Main functions =========

// Draws the menu on screen and checks if button is pressed
void draw_menu() {
    ClearBackground(RED);

    // Menu title
    draw_window_title("CLASSIC SNAKE");

    // Start Game
    Rectangle startButton = draw_button("START",0);
    if (is_button_clicked(startButton)) {
        g_game_state = PLAYING;
    }

    // Theme Toggle
    Rectangle themeButton;
    if (g_is_dark_theme) {
        themeButton = draw_button("LIGHT MODE",1);
    } else {
        themeButton = draw_button("DARK MODE",1);
    }
    if (is_button_clicked(themeButton)) {
        toggle_dark_theme();
    }
}

// draws the pause menu when game is paused
void draw_pause() {
    ClearBackground(RED);

    // window title
    draw_window_title("GAME PAUSED");

    // Continue button
    Rectangle continueButton = draw_button("PLAY",0);
    if (is_button_clicked(continueButton)) {
        g_game_state = PLAYING;
    }

    // Theme Toggle
    Rectangle themeButton;
    if (g_is_dark_theme) {
        themeButton = draw_button("LIGHT MODE",1);
    } else {
        themeButton = draw_button("DARK MODE",1);
    }
    if (is_button_clicked(themeButton)) {
        toggle_dark_theme();
    }
}

// Draws the death screen when snake is dead
void draw_death() {

    ClearBackground(g_background_color);
    
    drawgrid();
    draw_snake();
    draw_score();
    drawWindowBorder();

    // End game banner
    int bannerPadding = GetScreenHeight()/7;

    DrawRectangle(bannerPadding,bannerPadding,g_screen_width-2*bannerPadding,g_screen_height-2*bannerPadding,Fade(RED,0.85));

    // SCORE display
    char buffer[10];
    char* score = &buffer[0];
    sprintf(buffer, "SCORE: %d", g_score);
    
    draw_window_title(score);

    // back button
    Rectangle backButton = draw_button("BACK TO MENU",0);
    if (is_button_clicked(backButton)) {
        g_game_state = MENU;
    }
}

// ==================================================

int main(void)
{
    SetGesturesEnabled(GESTURE_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(g_screen_width, g_screen_height, "Classic Snake Game");
    initialize_global_variables();

#if defined(PLATFORM_WEB)
    TraceLog(LOG_INFO,"WEB MODE");
    emscripten_set_main_loop(update_draw_frame, 0, 1);
#else
    SetTargetFPS(240);
    TraceLog(LOG_INFO,"DESKTOP MODE");

    while (!WindowShouldClose())
    {
        update_draw_frame();
    }
#endif

    CloseWindow();
    destroy_global_variables();

    return 0;
}


// Game Loop function
void update_draw_frame(void)
{
    if (IsWindowResized()) {
        // New dimensions
        g_screen_height = GetScreenHeight();
        g_screen_width = GetScreenWidth();

        // recompute global vargs
        g_grid_size = MIN(g_screen_width/g_horizontal_grid_count,g_screen_height/g_vertical_grid_count);
        g_margin_size = ((double)g_grid_size)*g_margin_ratio;
        
        int leftPadding = GetScreenWidth()-(g_grid_size*g_horizontal_grid_count);
        int topPadding = GetScreenHeight()-(g_grid_size*g_vertical_grid_count);

        g_screen_padding.x = leftPadding/2;
        g_screen_padding.y = topPadding/2;
        g_screen_padding.width = GetScreenWidth()-leftPadding;
        g_screen_padding.height = GetScreenHeight()-topPadding;
    }
    BeginDrawing();
    
    switch (g_game_state)
    {
    case MENU:
        draw_menu();
        break;
    case PLAYING:
        draw_playing();
        break;
    case DEATH:
        draw_death();
        break;
    default: // PAUSE by default
        draw_pause();
        break;
    }

    EndDrawing();
    g_extern_touch_x = 0;
    g_extern_touch_y = 0;
}

// Does the painting for GameState = PLAYING
void draw_playing() {
    if (g_game_over) {
        g_game_over = false;
        initialize_global_variables();
    }

    ClearBackground(g_background_color);

    drawgrid();
    drawWindowBorder();

    if (g_game_over) {

        drawWindowBoxMarker();
    
    } else {
        
        move_snake();
        if (check_food_eaten()) {
            // Food eaten
            place_new_food();
            SNAKE->increase_length = true;
            computeScore(SNAKE->size);
        }
        if (is_snake_dead()) {
            g_game_over = true;
            g_game_state = DEATH;
        }
    }

    draw_snake();
    draw_food();
    draw_score();
    draw_pause_button();
#if defined(PLATFORM_WEB)
        handle_input();
#else
        handle_key_press();
#endif
}

// ====== HELPER FUNCTIONS ======

// paints a standing body
void print_body_standing(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX)+g_margin_size, g_grid_size * (bodyToPaint->posY)};
    Vector2 size =  {g_grid_size-2*g_margin_size, g_grid_size};
    
    Color color = get_color_for_index(bodyToPaint);
    DrawRectangleV(topLeft,size,color);
}

void print_body_sleeping(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX), g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 size =  {g_grid_size, g_grid_size-2*g_margin_size};
    
    Color color = get_color_for_index(bodyToPaint);
    DrawRectangleV(topLeft,size,color);
}

void print_body_up_right(SnakeBody* bodyToPaint) {
    
    Color color = get_color_for_index(bodyToPaint);
    
    // Main body
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX)+g_margin_size, g_grid_size * (bodyToPaint->posY)};
    Vector2 size =  {g_grid_size-2*g_margin_size, g_grid_size-g_margin_size};
    
    DrawRectangleV(topLeft,size,color);

    // FILL
    Vector2 topLeftFill =  {topLeft.x + size.x, topLeft.y + g_margin_size};
    Vector2 sizeFill =  {g_margin_size,g_grid_size-2*g_margin_size};
    DrawRectangleV(topLeftFill,sizeFill,color);
}

void print_body_up_left(SnakeBody* bodyToPaint) {
    Color color = get_color_for_index(bodyToPaint);
    
    // Main body
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX)+g_margin_size, g_grid_size * (bodyToPaint->posY)};
    Vector2 size =  {g_grid_size-2*g_margin_size, g_grid_size-g_margin_size};
    
    DrawRectangleV(topLeft,size,color);

    // FILL
    Vector2 topLeftFill =  {g_grid_size * (bodyToPaint->posX), g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 sizeFill =  {g_margin_size,g_grid_size-2*g_margin_size};
    DrawRectangleV(topLeftFill,sizeFill,color);
}

void print_body_down_left(SnakeBody* bodyToPaint) {
    Color color = get_color_for_index(bodyToPaint);
    
    // Main body
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX)+g_margin_size, g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 size =  {g_grid_size-2*g_margin_size,g_grid_size-g_margin_size};
    DrawRectangleV(topLeft,size,color);

    Vector2 topLeftFill =  {g_grid_size * (bodyToPaint->posX), g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 sizeFill =  {g_margin_size, g_grid_size-2*g_margin_size};
    DrawRectangleV(topLeftFill,sizeFill,color);
}

void print_body_down_right(SnakeBody* bodyToPaint) {
    Color color = get_color_for_index(bodyToPaint);
    
    // Main body
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX)+g_margin_size, g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 size =  {g_grid_size-2*g_margin_size,g_grid_size-g_margin_size};
    DrawRectangleV(topLeft,size,color);

    Vector2 topLeftFill =  {topLeft.x+size.x, topLeft.y};
    Vector2 sizeFill =  {g_margin_size, g_grid_size-2*g_margin_size};
    DrawRectangleV(topLeftFill,sizeFill,color);
}

// Paints a single body piece on grid
void paint_snake_body_helper(SnakeBody* last, SnakeBody* bodyToPaint,SnakeBody* next) {
    enum SnakeDirection dir_1 = get_snake_relative_direction(bodyToPaint,last);
    enum SnakeDirection dir_2 = get_snake_relative_direction(bodyToPaint,next);

    // Error case
    if (dir_1 == dir_2) print_error_body(bodyToPaint);
    
    // Standing
    if (dir_1 == UP && dir_2 == DOWN) print_body_standing(bodyToPaint);
    if (dir_1 == DOWN && dir_2 == UP) print_body_standing(bodyToPaint);
    
    // Sleeping
    if (dir_1 == LEFT && dir_2 == RIGHT) print_body_sleeping(bodyToPaint);
    if (dir_1 == RIGHT && dir_2 == LEFT) print_body_sleeping(bodyToPaint);
    
    // top right
    if (dir_1 == DOWN && dir_2 == LEFT) print_body_up_right(bodyToPaint);
    if (dir_1 == LEFT && dir_2 == DOWN) print_body_up_right(bodyToPaint);
    
    // top left
    if (dir_1 == DOWN && dir_2 == RIGHT) print_body_up_left(bodyToPaint);
    if (dir_1 == RIGHT && dir_2 == DOWN) print_body_up_left(bodyToPaint);
    
    // bottom left
    if (dir_1 == UP && dir_2 == RIGHT) print_body_down_left(bodyToPaint);
    if (dir_1 == RIGHT && dir_2 == UP) print_body_down_left(bodyToPaint);
    
    // bottom right
    if (dir_1 == UP && dir_2 == LEFT) print_body_down_right(bodyToPaint);
    if (dir_1 == LEFT && dir_2 == UP) print_body_down_right(bodyToPaint);

}

// Whenever a direction error is encountered, the body is painted with this
void print_error_body(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX), g_grid_size * (bodyToPaint->posY)};
    Vector2 size =  {g_grid_size, g_grid_size};
    DrawRectangleV(topLeft,size,GRAY);
}

// Paints the snake head
void paint_snake_head_helper(SnakeBody* head, SnakeBody* next) {
    g_margin_size -= 5;
    Vector2 topLeft =  {g_grid_size * (head->posX)+g_margin_size,g_grid_size * (head->posY)+g_margin_size};
    Vector2 size =  {g_grid_size-2*g_margin_size, g_grid_size-2*g_margin_size};
    switch (get_snake_relative_direction(head,next))
    {
    case UP:
        size.y += g_margin_size;
        break;
    case RIGHT:
        topLeft.x -= g_margin_size;
        size.x += g_margin_size;
        break;
    case DOWN:
        topLeft.y -= g_margin_size;
        size.y += g_margin_size;
        break;
    default: // LEFT by default
        size.x += g_margin_size;
        break;
    }

    DrawRectangleV(topLeft,size,g_snake_head_color);
    g_margin_size += 5;
}

void paint_snake_tail_helper(SnakeBody* tail, SnakeBody* secondLast) {
    int centerSize = g_grid_size-2*g_margin_size;
    Vector2 topLeft =  {g_grid_size * (tail->posX)+g_margin_size, g_grid_size * (tail->posY)+g_margin_size};
    Vector2 size =  {g_grid_size-2*g_margin_size, g_grid_size-2*g_margin_size};
    Vector2 circleTopLeft = {g_grid_size * (tail->posX)+(g_grid_size/2),g_grid_size * (tail->posY)+(g_grid_size/2)};
    switch (get_snake_relative_direction(tail,secondLast))
    {
    case UP:
        topLeft.y += centerSize;
        size.y = g_margin_size;
        circleTopLeft.y += (centerSize/2);
        break;
    case RIGHT:
        topLeft.x -= g_margin_size;
        size.x = g_margin_size;
        circleTopLeft.x -= (centerSize/2);
        break;
    case DOWN:
        topLeft.y -= g_margin_size;
        size.y = g_margin_size;
        circleTopLeft.y -= (centerSize/2);
        break;
    default: // LEFT by default
        topLeft.x += centerSize;
        size.x = g_margin_size;
        circleTopLeft.x += (centerSize/2);
        break;
    }
    Color color = get_color_for_index(tail);
    DrawRectangleV(topLeft,size,color);
    DrawCircle(circleTopLeft.x,circleTopLeft.y,(g_grid_size-2*g_margin_size)/2,color);
}

// REQUIRES, first and second are at right angles
// gives the direction of first relative to second
enum SnakeDirection get_snake_relative_direction(SnakeBody* first, SnakeBody* second) {

    if (first->posX < second->posX) {
        if ((second->posX - first->posX) > 1) {
            return RIGHT; // due to world wrap
        }
        return LEFT;
    }
    if (second->posX < first->posX) {
        if ((first->posX - second->posX) > 1) {
            return LEFT; // due to world wrap
        }
        return RIGHT;
    }
    if (second->posY < first->posY) {
        if ((first->posY - second->posY) > 1) {
            return UP; // due to world wrap
        }
        return DOWN;
    }

    if ((second->posY-first->posY)>1) {
        return DOWN;  // due to world wrap
    }
    return UP;
}

// Draws the entire snake on screen
void draw_snake() {
    struct SnakeBody* currentBody = SNAKE->head;
    struct SnakeBody* lastBody = NULL;
    for (size_t i = 0; i < SNAKE->size; i++)
    {
        if (i==0) {
            paint_snake_head_helper(currentBody,currentBody->next);
        } else if (i==SNAKE->size-1) {
            paint_snake_tail_helper(currentBody,lastBody);
        } else {
            paint_snake_body_helper(lastBody,currentBody,currentBody->next);
        }
        lastBody = currentBody;
        currentBody = currentBody->next;
    }
}

// Initializes global vars
void initialize_global_variables() {
    
    // Vars
    g_vertical_grid_count = g_screen_height/g_grid_size;
    g_horizontal_grid_count = g_screen_width/g_grid_size;
    g_margin_size = ((double)g_grid_size)*g_margin_ratio;
    int leftPadding = GetScreenWidth()-(g_grid_size*g_horizontal_grid_count);
    int topPadding = GetScreenHeight()-(g_grid_size*g_vertical_grid_count);

    g_screen_padding.x = leftPadding/2;
    g_screen_padding.y = topPadding/2;
    g_screen_padding.width = GetScreenWidth()-leftPadding;
    g_screen_padding.height = GetScreenHeight()-topPadding;
    g_score = 0;
    // SNAKE

    SnakeBody* head = malloc(sizeof(struct SnakeBody));
    head->posX = g_horizontal_grid_count/2;
    head->posY = g_vertical_grid_count/2;
    head->index = 0;
    
    SnakeBody* tail = malloc(sizeof(struct SnakeBody));
    tail->posX = head->posX+1;
    tail->posY = head->posY+1;
    tail->index = 1;
    tail->next = NULL;

    head->next = tail;
    
    SNAKE = malloc(sizeof(struct Snake));
    SNAKE->size = 2;
    SNAKE->direction = DOWN;
    SNAKE->increase_length = false;
    SNAKE->direction_changed = false;
    SNAKE->head = head;
    SNAKE->tail = tail;

    // Food
    SetRandomSeed(time(NULL));
    place_new_food();
}

// Checks if user input is received
void handle_key_press() {
    if (IsKeyPressed(KEY_W) ||
        IsKeyPressed(KEY_UP) ||
        IsGestureDetected(GESTURE_SWIPE_UP)) {
        change_snake_direction(UP);
    }
    if (IsKeyPressed(KEY_A) ||
        IsKeyPressed(KEY_LEFT) ||
        IsGestureDetected(GESTURE_SWIPE_LEFT)) {
        change_snake_direction(LEFT);
    }
    if (IsKeyPressed(KEY_S) ||
        IsKeyPressed(KEY_DOWN) ||
        IsGestureDetected(GESTURE_SWIPE_DOWN)) {
        change_snake_direction(DOWN);
    }
    if (IsKeyPressed(KEY_D) ||
        IsKeyPressed(KEY_RIGHT) ||
        IsGestureDetected(GESTURE_SWIPE_RIGHT)) {
        change_snake_direction(RIGHT);
    }
    if (IsKeyPressed(KEY_SPACE)) {
        g_game_state = PAUSE;
    }

    // TEMPORARY !!!
    if (IsKeyPressed(KEY_E)) {
        SNAKE->increase_length = true;
    }
    if (IsKeyPressed(KEY_R)) {
        place_new_food();
    }
}

// Draws the food on scren
void draw_food() {
    Vector2 topLeft =  {g_grid_size * (g_food.x)+g_grid_size/2, g_grid_size * (g_food.y)+g_grid_size/2};
    DrawCircleV(topLeft,(g_grid_size/2)*0.7,RED);
}

// changes  the direction of snake as given by enum 
void change_snake_direction(enum SnakeDirection direction) {
    if (!SNAKE->direction_changed) {
        // Avoids setting reverse direction
        switch (SNAKE->direction)
        {
        case UP:
            if (direction == DOWN) return;
            break;
        case RIGHT:
            if (direction == LEFT) return;
            break;
        case DOWN:
            if (direction == UP) return;
            break;
        default:
            if (direction == RIGHT) return;
            break;
        }
        SNAKE->direction = direction;
        SNAKE->direction_changed = true;
    }
}

// Periodically checks if the snake should be moved
void move_snake() {
    if (GetTime() - g_last_snake_movement_time > g_snake_movement_time) {
        update_snake();
        if (g_score > 0) {
            g_score--;
        }
        g_last_snake_movement_time = GetTime();
    }
}

// moves the position of each snake body by 1 in the snake direction 
void update_snake() {
    
    SnakeBody* current = SNAKE->head;
    Position head_positon = {current->posX,current->posY};
    Position* new_position = malloc(sizeof(struct Position));
    get_updated_vector(SNAKE->direction,head_positon,new_position);
    for (size_t i = 0; i < SNAKE->size; i++)
    {   
        Position temporary = {current->posX,current->posY};
        
        current->posX = round_world_coordinates(new_position->x,g_horizontal_grid_count);
        current->posY = round_world_coordinates(new_position->y,g_vertical_grid_count);
        
        new_position->x = temporary.x;
        new_position->y = temporary.y;

        current = current->next;
    }

    if (SNAKE->increase_length) {
        // Make new tail
        SnakeBody* new_tail = malloc(sizeof(struct SnakeBody));
        new_tail->next = NULL;
        new_tail->posX = new_position->x;
        new_tail->posY = new_position->y;
        new_tail->index = SNAKE->tail->index + 1;

        // update old tail
        SnakeBody* old_tail = SNAKE->tail;
        old_tail->next = new_tail;
        
        // Update snake
        SNAKE->tail = new_tail;
        SNAKE->size++;
        SNAKE->increase_length = false;

    }

    SNAKE->direction_changed = false;
    free(new_position);
}

// Returns the coordinate as if the world is a sphere and start,end loop
int round_world_coordinates(int coord, int word_size) {
    if (coord < 0) {
        return word_size -1;
    }
    if (coord > word_size-1) {
        return 0;
    }
    return coord;
}

// returns a Vector2 pointer with updated 
//    coordinates in the given direction 
void get_updated_vector(enum SnakeDirection direction, Position oldPosition,Position* dest) {
    dest->x = oldPosition.x;
    dest->y = oldPosition.y;
    switch (direction)
    {
    case UP:
        dest->y -= 1;
        break;
    case RIGHT:
        dest->x += 1;
        break;
    case DOWN:
        dest->y += 1;
        break;
    default: // Implicitly right
        dest->x -= 1;
        break;
    }
}

// ====== OLD HELPERS ======
// Draws the grid for snake game
void drawgrid() {

    // int columns = g_screen_width/g_grid_size;
    // int rows = g_screen_height/g_grid_size;

    for (size_t i = 0; i < g_horizontal_grid_count; i++)
    {
        Vector2 columnStart = {g_grid_size * i, 0};
        Vector2 columnEnd = {g_grid_size * i, g_grid_size*g_vertical_grid_count};
        DrawLineV(columnStart,columnEnd,g_grid_color);
    }

    for (size_t i = 0; i < g_vertical_grid_count; i++)
    {
        Vector2 rowStart = {0, g_grid_size * i};
        Vector2 rowEnd = {g_horizontal_grid_count*g_grid_size, g_grid_size * i};
        DrawLineV(rowStart,rowEnd,g_grid_color);
    }
}

// makes a bounding box lines on viewport
void drawWindowBoxMarker() {
    DrawLine(0,0,g_screen_width,g_screen_height,RED);
    DrawLine(g_screen_width,0,0,g_screen_height,RED);
}

// Makes Lines on viewport
void drawWindowBorder() {
    // int right_padding = g_screen_width - (g_horizontal_grid_count*g_grid_size);
    // int bottom_padding = g_screen_height - (g_vertical_grid_count*g_grid_size);

    DrawRectangleLines(g_screen_padding.x,g_screen_padding.y,g_screen_padding.width,g_screen_padding.height,g_border_color);

    // DrawLine(0,0,g_screen_width-right_padding,0,g_border_color); // TOP
    // DrawLine(0,g_screen_height-bottom_padding-1,g_screen_width-right_padding,g_screen_height-bottom_padding-1,g_border_color); // BOTTOM
    // DrawLine(1,0,1,g_screen_height-bottom_padding,g_border_color); // LEFT
    // DrawLine(g_screen_width-right_padding,0,g_screen_width-right_padding,g_screen_height-bottom_padding,g_border_color); // RIGHT
}
