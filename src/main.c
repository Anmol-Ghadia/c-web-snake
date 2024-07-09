// Standard Libs
#include <stddef.h>
#include <stdlib.h>

// Libs
#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Helper Files
#include "snake.h"

// Function defs
void update_draw_frame(void);
void initialize_global_variables();
void draw_snake();
void paint_snake_body_helper(SnakeBody*);
void handle_key_press();
void change_snake_direction(enum SnakeDirection);
void move_snake();
void update_snake();
void get_updated_vector(enum SnakeDirection, Position, Position*);
void destroy_global_variables();
int round_world_coordinates(int, int);
void move_food_randomly();
void draw_food();

// old helper function defs
void drawgrid(Color);
void drawWindowBoxMarker();
void drawWindowBorder(Color);

// GLOBALS
Position g_food = {0,0};
int g_screen_width = 1280;
int g_screen_height = 720;
int g_grid_size = 40;
int g_vertical_grid_count;
int g_horizontal_grid_count;
double g_last_snake_movement_time = 0;
double g_snake_movement_time = 0.5;

int main(void)
{
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
    handle_key_press();
    move_snake();

    BeginDrawing();
        
        ClearBackground(WHITE);
        
        drawgrid(LIME);
        // drawWindowBoxMarker();
        drawWindowBorder(ORANGE);

        draw_snake();
        draw_food();
    
    EndDrawing();
}

// ====== HELPER FUNCTIONS ======

// Free all malloced memory of snake
void destroy_global_variables() {

}

// Paints a single body piece on grid
void paint_snake_body_helper(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {40 * (bodyToPaint->posX), 40 * (bodyToPaint->posY)};
    Vector2 size =  {40, 40};
    DrawRectangleV(topLeft,size,GRAY);
}

// Draws the entire snake on screen
void draw_snake() {
    struct SnakeBody* currentBody = SNAKE->head;
    for (size_t i = 0; i < SNAKE->size; i++)
    {
        paint_snake_body_helper(currentBody);
        currentBody = currentBody->next;
    }
}

// Initializes global vars
void initialize_global_variables() {
    // SNAKE
    SNAKE = malloc(sizeof(struct Snake));
    SNAKE->size = 1;
    SNAKE->direction = DOWN;
    SNAKE->increase_length = FALSE;

    SnakeBody* head = malloc(sizeof(struct SnakeBody));
    head->posX = 0;
    head->posY = 0;
    SNAKE->head = head;
    SNAKE->tail = head;

    // Vars
    g_vertical_grid_count = g_screen_height/g_grid_size;
    g_horizontal_grid_count = g_screen_width/g_grid_size;

    // Food
    srand(GetTime());
    move_food_randomly();
}

// Checks if user input is received
void handle_key_press() {
    if (IsKeyPressed(KEY_W)) {
        change_snake_direction(UP);
    }
    if (IsKeyPressed(KEY_A)) {
        change_snake_direction(LEFT);
    }
    if (IsKeyPressed(KEY_S)) {
        change_snake_direction(DOWN);
    }
    if (IsKeyPressed(KEY_D)) {
        change_snake_direction(RIGHT);
    }
    // TEMPORARY !!!
    if (IsKeyPressed(KEY_E)) {
        SNAKE->increase_length = TRUE;
    }
    if (IsKeyPressed(KEY_R)) {
        move_food_randomly();
    }
}

// Draws the food on scren
void draw_food() {
    Vector2 topLeft =  {40 * (g_food.x), 40 * (g_food.y)};
    Vector2 size =  {40, 40};
    DrawRectangleV(topLeft,size,RED);
}

// Places food at a random location on the map
void move_food_randomly() {
    g_food.x = rand() % g_horizontal_grid_count;
    g_food.y = rand() % g_vertical_grid_count;
}

// changes  the direction of snake as given by enum 
void change_snake_direction(enum SnakeDirection direction) {
    SNAKE->direction = direction;
}

// Periodically checks if the snake should be moved
void move_snake() {
    if (GetTime() - g_last_snake_movement_time > g_snake_movement_time) {
        update_snake();
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

    if (SNAKE->increase_length == TRUE) {
        // Make new tail
        SnakeBody* new_tail = malloc(sizeof(struct SnakeBody));
        new_tail->next = NULL;
        new_tail->posX = new_position->x;
        new_tail->posY = new_position->y;

        // update old tail
        SnakeBody* old_tail = SNAKE->tail;
        old_tail->next = new_tail;
        
        // Update snake
        SNAKE->tail = new_tail;
        SNAKE->size++;
        SNAKE->increase_length = FALSE;

    }

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

    int columns = g_screen_width/g_grid_size;
    int rows = g_screen_height/g_grid_size;

    for (size_t i = 0; i < columns; i++)
    {
        Vector2 columnStart = {(g_screen_width/columns) * i, 0};
        Vector2 columnEnd = {(g_screen_width/columns) * i, g_screen_height};
        DrawLineV(columnStart,columnEnd,gridColor);
    }

    for (size_t i = 0; i < rows; i++)
    {
        Vector2 rowStart = {0, (g_screen_height/rows) * i};
        Vector2 rowEnd = {g_screen_width, (g_screen_height/rows) * i};
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
    DrawLine(0,0,g_screen_width,0,color); // TOP
    DrawLine(0,g_screen_height-1,g_screen_width,g_screen_height-1,color); // BOTTOM
    DrawLine(1,0,1,g_screen_height,color); // LEFT
    DrawLine(g_screen_width,0,g_screen_width,g_screen_height,color); // RIGHT
}
