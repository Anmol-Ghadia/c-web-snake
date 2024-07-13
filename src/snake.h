#ifndef SNAKE_H
#define SNAKE_H

// Type defs
enum GameState {
    MENU = 0,
    PLAYING = 1,
    PAUSE = 2,
    DEATH = 3
};
enum SnakeDirection {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

typedef struct Position {
    int x;
    int y;
} Position;

// Represents a single cell of snake body
typedef struct SnakeBody {
    unsigned int posX;
    unsigned int posY; 
    unsigned int index;
    void* next;
} SnakeBody;


// Represents a complete snake built from multiple Snake Bodies 
typedef struct Snake {
    unsigned int size;
    enum SnakeDirection direction;
    bool increase_length;
    bool direction_changed;
    SnakeBody* head;
    SnakeBody* tail;
} Snake;

// GLOBALS
Snake* SNAKE;

#endif // SNAKE_H
