#ifndef SNAKE_H
#define SNAKE_H

// Type defs

enum SnakeDirection {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

enum Boolean {
    FALSE = 0,
    TRUE = 1
};

// Represents a single cell of snake body
typedef struct SnakeBody
{
    unsigned int posX;
    unsigned int posY; 
    void* next;
} SnakeBody;


// Represents a complete snake built from multiple Snake Bodies 
typedef struct Snake
{
    unsigned int size;
    enum SnakeDirection direction;
    enum Boolean increase_length;
    SnakeBody* head;
    SnakeBody* tail;
} Snake;

// GLOBALS
Snake* SNAKE;

// Interface
// TODO !!!

#endif // SNAKE_H
