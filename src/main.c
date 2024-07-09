#include <stddef.h>
#include <stdlib.h>

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

typedef struct SnakeBody
{
    int posX;
    int posY;
    void* next;
} SnakeBody;

typedef struct Snake
{
    int size;
    SnakeBody* head;
    SnakeBody* tail;
} Snake;


void UpdateDrawFrame(void);
void InitVariables();

const int screenWidth = 1280;
const int screenHeight = 720;
Snake* SNAKE;


int main(void)
{
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    InitVariables();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(240);

    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();

    return 0;
}


// Draws the grid for snake game
void drawgrid(int gridSize,Color gridColor) {
    int height = GetScreenHeight();
    int width = GetScreenWidth();

    int columns = width/gridSize;
    int rows = height/gridSize;

    for (size_t i = 0; i < columns; i++)
    {
        Vector2 columnStart = {(width/columns) * i, 0};
        Vector2 columnEnd = {(width/columns) * i, height};
        DrawLineV(columnStart,columnEnd,gridColor);
    }

    for (size_t i = 0; i < rows; i++)
    {
        Vector2 rowStart = {0, (height/rows) * i};
        Vector2 rowEnd = {width, (height/rows) * i};
        DrawLineV(rowStart,rowEnd,gridColor);
    }
    
}

// makes a bounding box lines on viewport
void drawWindowBoxMarker() {
    int height = GetScreenHeight();
    int width = GetScreenWidth();
    DrawLine(0,0,width,height,RED);
    DrawLine(width,0,0,height,RED);
}

// Makes Lines on viewport
void drawWindowBorder(Color color) {
    int height = GetScreenHeight();
    int width = GetScreenWidth();
    DrawLine(0,0,width,0,color); // TOP
    DrawLine(0,height-1,width,height-1,color); // BOTTOM
    DrawLine(1,0,1,height,color); // LEFT
    DrawLine(width,0,width,height,color); // RIGHT
}

// Initializes global vars
void InitVariables() {
    SNAKE = malloc(sizeof(struct Snake));
    SNAKE->size = 1;

    SnakeBody* head = malloc(sizeof(struct SnakeBody));
    head->posX = 5;
    head->posY = 5;
    SNAKE->head = head;
}

void paintBody(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {40 * (bodyToPaint->posX), 40 * (bodyToPaint->posY)};
    Vector2 size =  {40, 40};
    DrawRectangleV(topLeft,size,GRAY);
}

void drawSnake() {
    struct SnakeBody* currentBody = SNAKE->head;
    for (size_t i = 0; i < SNAKE->size; i++)
    {
        paintBody(currentBody);
        currentBody = currentBody->next;
    }
}

// Game Loop function
void UpdateDrawFrame(void)
{
    BeginDrawing();
    ClearBackground(WHITE);

    drawgrid(40,LIME);
    // drawWindowBoxMarker();
    drawWindowBorder(ORANGE);

    drawSnake();
    
    EndDrawing();
}
