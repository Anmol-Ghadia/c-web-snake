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

void change_snake_direction(enum SnakeDirection);


#if defined(PLATFORM_WEB)
// sets the touch input
// used by external api
void give_touch_input(int x, int y) {
    g_extern_touch.x = x;
    g_extern_touch.y = y;
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

// Pauses the game if playing,
// unpauses if already paused.
void pause_playing_game() {
    if (g_game_state == PLAYING) {
        g_game_state = PAUSE;
    } else if (g_game_state == PAUSE) {
        g_game_state = PLAYING;
    }
}
#endif
// ====== HELPERS ========

#if defined(PLATFORM_WEB)
// Resets the touch input by PLATFORM_WEB
void resetInput() {
    g_extern_touch.x = EXTERN_TOUCH_NONE;
    g_extern_touch.y = EXTERN_TOUCH_NONE;
}
#endif

// Draws the grid for snake game
void drawgrid() {

    for (size_t i = 1; i < g_horizontal_grid_count; i++)
    {
        Vector2 columnStart = {
            g_grid_size * i + g_screen_padding.x,
            g_screen_padding.y
        };
        Vector2 columnEnd = {
            g_grid_size * i + g_screen_padding.x,
            g_grid_size*g_vertical_grid_count + g_screen_padding.y
        };
        DrawLineV(columnStart,columnEnd,g_grid_color);
    }

    for (size_t i = 1; i < g_vertical_grid_count; i++)
    {
        Vector2 rowStart = {
            g_screen_padding.x,
            g_grid_size * i + g_screen_padding.y
        };
        Vector2 rowEnd = {
            g_horizontal_grid_count*g_grid_size + g_screen_padding.x,
            g_grid_size * i + g_screen_padding.y
        };
        DrawLineV(rowStart,rowEnd,g_grid_color);
    }
}

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

// Draws the game border and paints outside region
void drawWindowBorder() {
    ClearBackground(g_background_outisde_color);
    
    DrawRectangle(g_screen_padding.x,g_screen_padding.y,g_screen_padding.width,g_screen_padding.height,g_background_color);
    
    DrawRectangleLines(g_screen_padding.x,g_screen_padding.y,g_screen_padding.width,g_screen_padding.height,g_border_color);
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
    if (g_extern_touch.x != EXTERN_TOUCH_NONE && g_extern_touch.y != EXTERN_TOUCH_NONE) {
        if (CheckCollisionPointRec(g_extern_touch,button)) {
            resetInput();
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
    pauseText.y = g_margin_size + g_screen_padding.y;
    pauseText.height = GetScreenHeight()/16;
    pauseText.width = MeasureText((char *)&titleText,pauseText.height);
    pauseText.x = (g_screen_padding.x+g_screen_padding.width-g_margin_size-pauseText.width);

    DrawText((char *)&titleText,pauseText.x,pauseText.y,pauseText.height,g_pause_color);
    if (is_button_clicked(pauseText)) {
        g_game_state = PAUSE;
    }
}

#if defined(PLATFORM_WEB)
// Handles web input
void handle_web_input() {
    if (g_extern_touch.x == EXTERN_TOUCH_NONE && g_extern_touch.y == EXTERN_TOUCH_NONE) return;

    Vector2 mid = {GetScreenWidth()/2,GetScreenHeight()/2};
    Vector2 topLeft = {0,0};
    Vector2 topRight = {GetScreenWidth(),0};
    Vector2 bottomRight = {GetScreenWidth(),GetScreenHeight()};
    Vector2 bottomLeft = {0,GetScreenHeight()};

    if (CheckCollisionPointTriangle(g_extern_touch,topLeft,mid,topRight)) {
        change_snake_direction(UP);
    } else if (CheckCollisionPointTriangle(g_extern_touch,topRight,mid,bottomRight)) {
        change_snake_direction(RIGHT);
    } else if (CheckCollisionPointTriangle(g_extern_touch,bottomRight,mid,bottomLeft)) {
        change_snake_direction(DOWN);
    } else if (CheckCollisionPointTriangle(g_extern_touch,bottomLeft,mid,topLeft)) {
        change_snake_direction(LEFT);
    }
    resetInput();
}
#endif

// Draws the score when game is being played
void draw_score() {
    char buffer[10];
    char* ptr_score = &buffer[0];

    sprintf(buffer, "%d", g_score);
    int textX = g_margin_size+g_screen_padding.x;
    int textY = g_margin_size+g_screen_padding.y;
    int textSize = g_grid_size-g_margin_size;

    DrawText(ptr_score,textX,textY,textSize,g_score_color);
}

// Draws snake body when it is in NORTH-SOUTH direction
void draw_body_standing(SnakeBody* bodyToPaint) {
    Vector2 size =  {
        g_grid_size-2*g_margin_size,
        g_grid_size
    };
    Vector2 topLeft =  {
        g_screen_padding.x + g_grid_size * (bodyToPaint->posX)+g_margin_size,
        g_screen_padding.y + g_grid_size * (bodyToPaint->posY)
    };
    
    Color color = get_color_for_index(bodyToPaint);
    DrawRectangleV(topLeft,size,color);
}

// Draws snake body when it is in EAST-WEST direction
void draw_body_sleeping(SnakeBody* bodyToPaint) {
    Vector2 size =  {
        g_grid_size,
        g_grid_size-2*g_margin_size
    };
    Vector2 topLeft =  {
        g_screen_padding.x + g_grid_size * (bodyToPaint->posX),
        g_screen_padding.y + g_grid_size * (bodyToPaint->posY)+g_margin_size
    };
    
    Color color = get_color_for_index(bodyToPaint);
    DrawRectangleV(topLeft,size,color);
}

// Draws a right angle piece of body with given rotation
//  rotation = 0 is NORTH-EAST piece
void draw_snake_right_angle(SnakeBody *bodyToPaint,float rotation) {
    
    Color color = get_color_for_index(bodyToPaint);
    
    int centerHalf = (g_grid_size-2*g_margin_size)/2;

    Vector2 topOrigin = {
        (g_grid_size-2*g_margin_size)/2,
        g_grid_size-g_margin_size-centerHalf
    };

    // Top body
    Rectangle top =  {
        g_screen_padding.x + g_grid_size * (bodyToPaint->posX)+g_margin_size + centerHalf,
        g_screen_padding.y + g_grid_size * (bodyToPaint->posY)+g_margin_size + centerHalf,
        g_grid_size-2*g_margin_size,
        g_grid_size-g_margin_size
    };
    DrawRectanglePro(top,topOrigin,rotation,color);

    // Right body
    Vector2 rightOrigin = {
        centerHalf,
        centerHalf
    };
    Rectangle right =  {
        g_screen_padding.x + g_grid_size * (bodyToPaint->posX)+g_margin_size+centerHalf,
        g_screen_padding.y + g_grid_size * (bodyToPaint->posY)+g_margin_size+centerHalf,
        g_grid_size-g_margin_size,
        g_grid_size-2*g_margin_size
    };
    DrawRectanglePro(right,rightOrigin,rotation,color);
}

// Draws a faulty body Piece
void draw_body_error(SnakeBody* bodyToPaint) {
    Vector2 topLeft =  {
        g_grid_size * (bodyToPaint->posX),
        g_grid_size * (bodyToPaint->posY)
    };
    Vector2 size =  {g_grid_size, g_grid_size};
    DrawRectangleV(topLeft,size,GRAY);
}


// REQUIRES, first and second are at right angles
// gives the direction of first relative to second
enum SnakeDirection get_relative_snake_direction(SnakeBody* first, SnakeBody* second) {

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

// decides and draws the snake body in appropriate direction 
void paint_snake_body_helper(SnakeBody* last, SnakeBody* bodyToPaint,SnakeBody* next) {
    enum SnakeDirection dir_1 = get_relative_snake_direction(bodyToPaint,last);
    enum SnakeDirection dir_2 = get_relative_snake_direction(bodyToPaint,next);

    // Error case
    if (dir_1 == dir_2) draw_body_error(bodyToPaint);
    
    // Standing
    if (dir_1 == UP && dir_2 == DOWN) draw_body_standing(bodyToPaint);
    if (dir_1 == DOWN && dir_2 == UP) draw_body_standing(bodyToPaint);
    
    // Sleeping
    if (dir_1 == LEFT && dir_2 == RIGHT) draw_body_sleeping(bodyToPaint);
    if (dir_1 == RIGHT && dir_2 == LEFT) draw_body_sleeping(bodyToPaint);
    
    // top right
    if (dir_1 == DOWN && dir_2 == LEFT) draw_snake_right_angle(bodyToPaint,0);
    if (dir_1 == LEFT && dir_2 == DOWN) draw_snake_right_angle(bodyToPaint,0);
    
    // top left
    if (dir_1 == DOWN && dir_2 == RIGHT) draw_snake_right_angle(bodyToPaint,270);
    if (dir_1 == RIGHT && dir_2 == DOWN) draw_snake_right_angle(bodyToPaint,270);
    
    // bottom left
    if (dir_1 == UP && dir_2 == RIGHT) draw_snake_right_angle(bodyToPaint,180);
    if (dir_1 == RIGHT && dir_2 == UP) draw_snake_right_angle(bodyToPaint,180);
    
    // bottom right
    if (dir_1 == UP && dir_2 == LEFT) draw_snake_right_angle(bodyToPaint,90);
    if (dir_1 == LEFT && dir_2 == UP) draw_snake_right_angle(bodyToPaint,90);
}

// returns a Vector2 pointer with updated 
//    coordinates in the given direction 
void move_snake_in_direction(enum SnakeDirection direction, Position oldPosition,Position* dest) {
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

// Returns the coordinate as if the world is a sphere
//  EAST-WEST loop and NORTH-SOUTH loop 
int get_round_world_coordinate(int coord, int worldSize) {
    if (coord < 0) {
        return worldSize -1;
    }
    if (coord > worldSize-1) {
        return 0;
    }
    return coord;
}

// Moves the snake by 1 cell in snake's direction
void update_snake_position() {
    
    SnakeBody* current = SNAKE->head;
    Position head_positon = {current->posX,current->posY};
    Position* new_position = malloc(sizeof(struct Position));
    move_snake_in_direction(SNAKE->direction,head_positon,new_position);
    for (size_t i = 0; i < SNAKE->size; i++)
    {   
        Position temporary = {current->posX,current->posY};
        
        current->posX = get_round_world_coordinate(new_position->x,g_horizontal_grid_count);
        current->posY = get_round_world_coordinate(new_position->y,g_vertical_grid_count);
        
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

// Periodically checks if the snake should be moved
void move_snake_if_possible() {
    if (GetTime() - g_last_snake_movement_time > g_snake_movement_time) {
        update_snake_position();
        if (g_score > 0) {
            g_score--;
        }
        g_last_snake_movement_time = GetTime();
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

// Draws the food on scren
void draw_food() {
    Vector2 topLeft =  {
        g_screen_padding.x + g_grid_size * (g_food.x)+g_grid_size/2,
        g_screen_padding.y + g_grid_size * (g_food.y)+g_grid_size/2
    };
    DrawCircleV(topLeft,(g_grid_size/2)*0.7,RED);
}

// Checks if user input is received
void handle_desktop_input() {
    if (IsKeyPressed(KEY_W) ||
        IsKeyPressed(KEY_UP)) {
        change_snake_direction(UP);
    }
    if (IsKeyPressed(KEY_A) ||
        IsKeyPressed(KEY_LEFT)) {
        change_snake_direction(LEFT);
    }
    if (IsKeyPressed(KEY_S) ||
        IsKeyPressed(KEY_DOWN)) {
        change_snake_direction(DOWN);
    }
    if (IsKeyPressed(KEY_D) ||
        IsKeyPressed(KEY_RIGHT)) {
        change_snake_direction(RIGHT);
    }
    if (IsKeyPressed(KEY_SPACE)) {
        g_game_state = PAUSE;
    }
}

// Paints the snake head in correct direction based on
//   next body piece
void paint_snake_head(SnakeBody* head, SnakeBody* next) {
    int head_margin_size = g_grid_size*g_head_margin_ratio;

    Vector2 gridCentre =  {
        g_screen_padding.x + g_grid_size * (head->posX) + g_grid_size/2,
        g_screen_padding.y + g_grid_size * (head->posY) + g_grid_size/2
    };

    DrawCircle(
        gridCentre.x,
        gridCentre.y,
        (g_grid_size-2*head_margin_size)/2,
        g_snake_head_color
    );
    
    Rectangle rect = {
        gridCentre.x,
        gridCentre.y,
        g_grid_size-2*head_margin_size,
        g_grid_size-head_margin_size-(g_grid_size-2*head_margin_size)/2,
    };
    Vector2 rectOrigin = {
        (g_grid_size-2*head_margin_size)/2,
        0
    };
    float rotation;
    switch (get_relative_snake_direction(head,next))
    {
    case UP:
        rotation = 0;
        break;
    case RIGHT:
        rotation = 90;
        break;
    case DOWN:
        rotation = 180;
        break;
    default: // LEFT assumed
        rotation = 270;
        break;
    }
    DrawRectanglePro(rect,rectOrigin,rotation,g_snake_head_color);
}

// Paints the tail of snake in correct direction based on
//    second last body piece 
void paint_snake_tail(SnakeBody* tail, SnakeBody* before) {

    Color color = get_color_for_index(tail);

    Vector2 gridCentre =  {
        g_screen_padding.x + g_grid_size * (tail->posX) + g_grid_size/2,
        g_screen_padding.y + g_grid_size * (tail->posY) + g_grid_size/2
    };

    DrawCircle(
        gridCentre.x,
        gridCentre.y,
        (g_grid_size-2*g_margin_size)/2,
        color
    );
    
    Rectangle rect = {
        gridCentre.x,
        gridCentre.y,
        g_grid_size-2*g_margin_size,
        g_grid_size-g_margin_size-(g_grid_size-2*g_margin_size)/2,
    };
    Vector2 rectOrigin = {
        (g_grid_size-2*g_margin_size)/2,
        0
    };
    float rotation;
    switch (get_relative_snake_direction(tail,before))
    {
    case UP:
        rotation = 0;
        break;
    case RIGHT:
        rotation = 90;
        break;
    case DOWN:
        rotation = 180;
        break;
    default: // LEFT assumed
        rotation = 270;
        break;
    }
    DrawRectanglePro(rect,rectOrigin,rotation,color);
}

// Draws the entire snake on screen
void draw_snake() {
    struct SnakeBody* currentBody = SNAKE->head;
    struct SnakeBody* lastBody = NULL;
    for (size_t i = 0; i < SNAKE->size; i++)
    {
        if (i==0) {
            paint_snake_head(currentBody,currentBody->next);
        } else if (i==SNAKE->size-1) {
            paint_snake_tail(currentBody,lastBody);
        } else {
            paint_snake_body_helper(lastBody,currentBody,currentBody->next);
        }
        lastBody = currentBody;
        currentBody = currentBody->next;
    }
}

// Free all malloced memory of snake
void delete_snake() {
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


// REQURIRES: SNAKE is NULL
// constructs a new snake with size 2
void init_snake() {
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
}

// Recompute the spacings and grid size
void compute_globals() {
    
    // recompute global vargs
    g_grid_size = MIN(GetScreenWidth()/g_horizontal_grid_count,GetScreenHeight()/g_vertical_grid_count);
    g_margin_size = ((double)g_grid_size)*g_margin_ratio;
    
    int leftPadding = GetScreenWidth()-(g_grid_size*g_horizontal_grid_count);
    int topPadding = GetScreenHeight()-(g_grid_size*g_vertical_grid_count);

    g_screen_padding.x = leftPadding/2;
    g_screen_padding.y = topPadding/2;
    g_screen_padding.width = GetScreenWidth()-leftPadding;
    g_screen_padding.height = GetScreenHeight()-topPadding;
}

// initializes the global variables to start a new game
void init_playing_state() {

    g_score = 0;
    compute_globals();
    init_snake();
    place_new_food();
}

// Initializes global vars
void init_global_variables() {
    // Food
    SetRandomSeed(time(NULL));
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
        init_playing_state();
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
    
    drawWindowBorder();
    drawgrid();
    draw_snake();

    // End game banner
    int bannerPadding = GetScreenHeight()/7;

    DrawRectangle(bannerPadding,bannerPadding,GetScreenWidth()-2*bannerPadding,GetScreenHeight()-2*bannerPadding,Fade(RED,0.85));

    // SCORE display
    char* buffer = (char*)malloc(20 * sizeof(char)); // Allocate enough space for the string
    if (buffer != NULL) {
        sprintf(buffer, "SCORE: %d", g_score);
        draw_window_title(buffer);
        free(buffer);
    } else {
        draw_window_title("SCORE ERROR");
    }

    // back button
    Rectangle backButton = draw_button("BACK TO MENU",0);
    if (is_button_clicked(backButton)) {
        delete_snake();
        g_game_state = MENU;
    }
}

// Does the drawing when game is played 
void draw_playing() {
    ClearBackground(g_background_color);

    drawWindowBorder();
    drawgrid();

    move_snake_if_possible();
    if (check_food_eaten()) {
        // Food eaten
        place_new_food();
        SNAKE->increase_length = true;
        computeScore(SNAKE->size);
    }
    if (is_snake_dead()) {
        g_game_state = DEATH;
    }

    draw_snake();
    draw_food();
    draw_score();
    draw_pause_button();
#if defined(PLATFORM_WEB)
    handle_web_input();
#else
    handle_desktop_input();
#endif
}

// Game Loop function
void update_draw_frame(void)
{
    if (IsWindowResized()) {
        compute_globals();
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
#if defined(PLATFORM_WEB)
    resetInput();
#endif
}

int main(void)
{
    SetGesturesEnabled(GESTURE_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(900, 600, "Classic Snake Game");
    init_global_variables();

#if defined(PLATFORM_WEB)
    TraceLog(LOG_INFO,"WEB MODE");
    emscripten_set_main_loop(update_draw_frame, 0, 1);
#else
    SetTargetFPS(244);
    TraceLog(LOG_INFO,"DESKTOP MODE");

    while (!WindowShouldClose())
    {
        update_draw_frame();
    }
#endif

    delete_snake();
    CloseWindow();

    return 0;
}
