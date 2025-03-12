#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 20
#define MAX_SNAKE_LENGTH 1000

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    Vector2 position;
    Color color;
} Food;

typedef struct {
    Vector2 positions[MAX_SNAKE_LENGTH];
    int length;
    Direction direction;
    Color color;
    bool alive;
} Snake;

void InitSnake(Snake *snake) {
    snake->length = 1;
    snake->positions[0] = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    snake->direction = RIGHT;
    snake->color = GREEN;
    snake->alive = true;
}

void SpawnFood(Food *food, Snake snake) {
    bool validPosition = false;
    int gridWidth = SCREEN_WIDTH / GRID_SIZE;
    int gridHeight = SCREEN_HEIGHT / GRID_SIZE;
    
    while (!validPosition) {
        int x = GetRandomValue(0, gridWidth - 1) * GRID_SIZE;
        int y = GetRandomValue(0, gridHeight - 1) * GRID_SIZE;
        food->position = (Vector2){ (float)x, (float)y };
        
        validPosition = true;
        for (int i = 0; i < snake.length; i++) {
            if (snake.positions[i].x == food->position.x && 
                snake.positions[i].y == food->position.y) {
                validPosition = false;
                break;
            }
        }
    }
    
    food->color = RED;
}

void UpdateSnake(Snake *snake, Food *food) {
    if (!snake->alive) return;
    
    // Move the body segments
    for (int i = snake->length - 1; i > 0; i--) {
        snake->positions[i] = snake->positions[i - 1];
    }
    
    // Move the head based on direction
    switch (snake->direction) {
        case UP:
            snake->positions[0].y -= GRID_SIZE;
            break;
        case DOWN:
            snake->positions[0].y += GRID_SIZE;
            break;
        case LEFT:
            snake->positions[0].x -= GRID_SIZE;
            break;
        case RIGHT:
            snake->positions[0].x += GRID_SIZE;
            break;
    }
    
    // Check for wall collision
    if (snake->positions[0].x < 0 || 
        snake->positions[0].x >= SCREEN_WIDTH ||
        snake->positions[0].y < 0 || 
        snake->positions[0].y >= SCREEN_HEIGHT) {
        snake->alive = false;
    }
    
    // Check for self collision
    for (int i = 1; i < snake->length; i++) {
        if (snake->positions[0].x == snake->positions[i].x &&
            snake->positions[0].y == snake->positions[i].y) {
            snake->alive = false;
            break;
        }
    }
    
    // Check for food collision
    if (snake->positions[0].x == food->position.x &&
        snake->positions[0].y == food->position.y) {
        snake->length++;
        SpawnFood(food, *snake);
    }
}

void HandleInput(Snake *snake) {
    if (IsKeyPressed(KEY_UP) && snake->direction != DOWN) {
        snake->direction = UP;
    }
    if (IsKeyPressed(KEY_DOWN) && snake->direction != UP) {
        snake->direction = DOWN;
    }
    if (IsKeyPressed(KEY_LEFT) && snake->direction != RIGHT) {
        snake->direction = LEFT;
    }
    if (IsKeyPressed(KEY_RIGHT) && snake->direction != LEFT) {
        snake->direction = RIGHT;
    }
}

void DrawGame(Snake snake, Food food, int score) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Draw food
    DrawRectangleV(food.position, (Vector2){ GRID_SIZE, GRID_SIZE }, food.color);
    
    // Draw snake
    for (int i = 0; i < snake.length; i++) {
        DrawRectangleV(snake.positions[i], (Vector2){ GRID_SIZE, GRID_SIZE }, snake.color);
    }
    
    // Draw score
    DrawText(TextFormat("Score: %d", snake.length - 1), 10, 10, 20, WHITE);
    
    // Game over message
    if (!snake.alive) {
        const char *gameOverText = "GAME OVER";
        const char *restartText = "Press ENTER to restart";
        
        int gameOverWidth = MeasureText(gameOverText, 40);
        int restartWidth = MeasureText(restartText, 20);
        
        DrawText(gameOverText, SCREEN_WIDTH/2 - gameOverWidth/2, SCREEN_HEIGHT/2 - 40, 40, RED);
        DrawText(restartText, SCREEN_WIDTH/2 - restartWidth/2, SCREEN_HEIGHT/2 + 20, 20, WHITE);
    }
    
    EndDrawing();
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game - Raylib");
    SetTargetFPS(60);  // Lower FPS for slower snake movement
    
    Snake snake;
    Food food;
    float frameTime = 0.0f;
    float updateTime = 0.15f;  // Time between snake updates
    
    InitSnake(&snake);
    SpawnFood(&food, snake);
    
    while (!WindowShouldClose()) {
        HandleInput(&snake);
        
        frameTime += GetFrameTime();
        if (frameTime >= updateTime) {
            UpdateSnake(&snake, &food);
            frameTime = 0.0f;
        }
        
        DrawGame(snake, food, snake.length - 1);
        
        // Game restart
        if (!snake.alive && IsKeyPressed(KEY_ENTER)) {
            InitSnake(&snake);
            SpawnFood(&food, snake);
        }
    }
    
    CloseWindow();
    return 0;
}