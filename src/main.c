// Standard Libs
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// Libs
#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Helper Files
#include "snake.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

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
void move_food_randomly();
void draw_food();
void check_food_eaten();
int food_is_colliding_snake(); 
void check_body_eaten();
enum SnakeDirection get_snake_relative_direction(SnakeBody*, SnakeBody*);
void print_error_body(SnakeBody*);
void draw_score();
void draw_playing();
void draw_menu(int width,int height);

// old helper function defs
void drawgrid(Color);
void drawWindowBoxMarker();
void drawWindowBorder(Color);

// GLOBALS
double g_snake_movement_time = 0.4;

int g_grid_size = 80;

// computed globals
enum GameState g_game_state = MENU;
Position g_food = {0,0};
int g_screen_width = 1280;
int g_screen_height = 720;
int g_margin_size = 30;
int g_vertical_grid_count;
int g_horizontal_grid_count;
double g_last_snake_movement_time = 0;

unsigned int g_score = 0;
bool g_game_over = false;
Color g_background_color = WHITE;

int main(void)
{
    SetGesturesEnabled(GESTURE_SWIPE_UP | GESTURE_SWIPE_DOWN | GESTURE_SWIPE_RIGHT | GESTURE_SWIPE_LEFT | GESTURE_TAP);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(g_screen_width, g_screen_height, "Simple Snake Game");
    initialize_global_variables();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(update_draw_frame, 0, 1);
#else
    SetTargetFPS(240);

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
    }
    BeginDrawing();
    
    switch (g_game_state)
    {
    case MENU:
        draw_menu(g_screen_width,g_screen_height);
        break;
    case PLAYING:
        draw_playing();
        break;
    
    default:
        break;
    }

    EndDrawing();
}
// Does the painting for GameState = MENU
void draw_menu(int width,int height) {
    ClearBackground(RED);

    // GAME TITLE
        char titleText[11] = "Snake Game";
        
        int titleY = height/6;
        int titleFontSize = height/6;
        int titleX = (width-MeasureText((char *)&titleText,titleFontSize))/2;

        DrawText((char *)&titleText,titleX,titleY,titleFontSize,BLACK);

    // START BUTTON
        char startText[6] = "START";
        
        int startY = height/2;
        int startFontHeight = height/12;
        int startFontWidth = MeasureText((char *)&startText,startFontHeight);
        int startX = (width-startFontWidth)/2;

        int startButtonSidePadding = 40;
        int startButtonUpDownPadding = 10;

        int startButtonX = startX - startButtonSidePadding;
        int startButtonY = startY-startButtonUpDownPadding;
        int startButtonWidth = startFontWidth+2*startButtonSidePadding;
        int startButtonHeight = startFontHeight+2*startButtonUpDownPadding;

        DrawRectangle(startButtonX,startButtonY,startButtonWidth,startButtonHeight,BLACK);
        DrawText((char *)&startText,startX,startY,startFontHeight,RED);
    
    // CHECK START CLICKED
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            bool cond_1 = startButtonX <= GetMouseX();
            bool cond_2 = GetMouseX() <= startButtonX+startButtonWidth;
            bool cond_3 = startButtonY <= GetMouseY();
            bool cond_4 = GetMouseY() <= startButtonY+startButtonHeight;
            if (cond_1 && cond_2 && cond_3 && cond_4) {
                g_game_state = PLAYING;
            }
        }

        if (IsGestureDetected(GESTURE_TAP)) {
            bool cond_1 = startButtonX <= GetTouchX();
            bool cond_2 = GetTouchX() <= startButtonX+startButtonWidth;
            bool cond_3 = startButtonY <= GetTouchY();
            bool cond_4 = GetTouchY() <= startButtonY+startButtonHeight;
            if (cond_1 && cond_2 && cond_3 && cond_4) {
                g_game_state = PLAYING;
            }
        }
}

// Does the painting for GameState = PLAYING
void draw_playing() {

    ClearBackground(g_background_color);

    drawgrid(LIME);
    drawWindowBorder(ORANGE);

    if (g_game_over) {

        drawWindowBoxMarker();
    
    } else {
        
        handle_key_press();
        move_snake();
        check_food_eaten();
        check_body_eaten();
    }

    draw_snake();
    draw_food();
    draw_score();
    
}

// ====== HELPER FUNCTIONS ======

// Free all malloced memory of snake
void destroy_global_variables() {
    // TODO !!!
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

void draw_score() {
    char buffer[10];
    const char* str_pointer = &buffer[0];

    sprintf(buffer, "%d", g_score);
    DrawText(str_pointer,g_margin_size,g_margin_size,g_grid_size-g_margin_size,BLACK);
}

// sets g_game_over if snake has eaten itself 
void check_body_eaten() {
    if (SNAKE->size < 2) return;

    SnakeBody* head = SNAKE->head;
    SnakeBody* current = head->next;
    for (size_t i = 1; i < SNAKE->size; i++)
    {
        if (head->posX == current->posX && head->posY == current->posY) {
            g_game_over = true;
            return;
        }
        current = current->next;
    }
}

// Returns 1 if food is colliding
int food_is_colliding_snake() {
    SnakeBody* current = SNAKE->head;
    for (size_t i = 0; i < SNAKE->size; i++)
    {
        if (current->posX == g_food.x && current->posY == g_food.y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Checks if the snake has eaten food
void check_food_eaten() {
    if (SNAKE->head->posX == g_food.x && SNAKE->head->posY == g_food.y) {
        // Food eaten
        move_food_randomly();
        SNAKE->increase_length = true;
        g_score += MIN(15,3*SNAKE->size);
    }
}

Color getColor(SnakeBody* bodyToPaint) {
    return hslToRgb8(((double)bodyToPaint->index/15)*360,0.75,0.5);
}

// paints a standing body
void print_body_standing(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX)+g_margin_size, g_grid_size * (bodyToPaint->posY)};
    Vector2 size =  {g_grid_size-2*g_margin_size, g_grid_size};
    
    Color color = getColor(bodyToPaint);
    DrawRectangleV(topLeft,size,color);
}

void print_body_sleeping(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX), g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 size =  {g_grid_size, g_grid_size-2*g_margin_size};
    
    Color color = getColor(bodyToPaint);
    DrawRectangleV(topLeft,size,color);
}

void print_body_up_right(SnakeBody* bodyToPaint) {
    
    Color color = getColor(bodyToPaint);
    
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
    Color color = getColor(bodyToPaint);
    
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
    Color color = getColor(bodyToPaint);
    
    // Main body
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX)+g_margin_size, g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 size =  {g_grid_size-2*g_margin_size,g_grid_size-g_margin_size};
    DrawRectangleV(topLeft,size,color);

    Vector2 topLeftFill =  {g_grid_size * (bodyToPaint->posX), g_grid_size * (bodyToPaint->posY)+g_margin_size};
    Vector2 sizeFill =  {g_margin_size, g_grid_size-2*g_margin_size};
    DrawRectangleV(topLeftFill,sizeFill,color);
}

void print_body_down_right(SnakeBody* bodyToPaint) {
    Color color = getColor(bodyToPaint);
    
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

    print_error_body(bodyToPaint);
}

// Whenever a direction error is encountered, the body is painted with this
void print_error_body(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {g_grid_size * (bodyToPaint->posX), g_grid_size * (bodyToPaint->posY)};
    Vector2 size =  {g_grid_size, g_grid_size};
    DrawRectangleV(topLeft,size,GRAY);
}

// Paints the snake head
void paint_snake_head_helper(SnakeBody* head, SnakeBody* next) {
    Vector2 topLeft =  {g_grid_size * (head->posX)+g_margin_size, g_grid_size * (head->posY)+g_margin_size};
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

    DrawRectangleV(topLeft,size,BLACK);
}

void paint_snake_tail_helper(SnakeBody* tail, SnakeBody* secondLast) {
    Vector2 topLeft =  {g_grid_size * (tail->posX)+g_margin_size, g_grid_size * (tail->posY)+g_margin_size};
    Vector2 size =  {g_grid_size-2*g_margin_size, g_grid_size-2*g_margin_size};
    switch (get_snake_relative_direction(tail,secondLast))
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
    Color color = getColor(tail);
    DrawRectangleV(topLeft,size,color);
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
    srand(time(NULL));
    move_food_randomly();
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

    // TEMPORARY !!!
    if (IsKeyPressed(KEY_E)) {
        SNAKE->increase_length = true;
    }
    if (IsKeyPressed(KEY_R)) {
        move_food_randomly();
    }
}

// Draws the food on scren
void draw_food() {
    Vector2 topLeft =  {g_grid_size * (g_food.x), g_grid_size * (g_food.y)};
    Vector2 size =  {g_grid_size, g_grid_size};
    DrawRectangleV(topLeft,size,RED);
}

// Places food at a random location on the map
void move_food_randomly() {
    g_food.x = rand() % g_horizontal_grid_count;
    g_food.y = rand() % g_vertical_grid_count;
    if (food_is_colliding_snake() == 1) {
        move_food_randomly();
    }
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
void drawgrid(Color gridColor) {

    // int columns = g_screen_width/g_grid_size;
    // int rows = g_screen_height/g_grid_size;

    for (size_t i = 0; i < g_horizontal_grid_count; i++)
    {
        Vector2 columnStart = {g_grid_size * i, 0};
        Vector2 columnEnd = {g_grid_size * i, g_grid_size*g_vertical_grid_count};
        DrawLineV(columnStart,columnEnd,gridColor);
    }

    for (size_t i = 0; i < g_vertical_grid_count; i++)
    {
        Vector2 rowStart = {0, g_grid_size * i};
        Vector2 rowEnd = {g_horizontal_grid_count*g_grid_size, g_grid_size * i};
        DrawLineV(rowStart,rowEnd,gridColor);
    }
    
}

// makes a bounding box lines on viewport
void drawWindowBoxMarker() {
    DrawLine(0,0,g_screen_width,g_screen_height,RED);
    DrawLine(g_screen_width,0,0,g_screen_height,RED);
}

// Makes Lines on viewport
void drawWindowBorder(Color color) {
    int right_padding = g_screen_width - (g_horizontal_grid_count*g_grid_size);
    int bottom_padding = g_screen_height - (g_vertical_grid_count*g_grid_size);

    DrawLine(0,0,g_screen_width-right_padding,0,color); // TOP
    DrawLine(0,g_screen_height-bottom_padding-1,g_screen_width-right_padding,g_screen_height-bottom_padding-1,color); // BOTTOM
    DrawLine(1,0,1,g_screen_height-bottom_padding,color); // LEFT
    DrawLine(g_screen_width-right_padding,0,g_screen_width-right_padding,g_screen_height-bottom_padding,color); // RIGHT
}
