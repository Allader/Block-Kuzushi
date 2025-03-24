#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 8
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 10
#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
#define BLOCK_WIDTH 75
#define BLOCK_HEIGHT 20
#define MAX_LIVES 3
#define BOMB_CHANCE 5  // 5% chance for a bomb block

typedef struct {
    Rectangle rect;
    bool active;
    bool isBomb;
} Block;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Block Kuzushi");
    SetTargetFPS(60);
    srand(time(NULL));

    // Paddle
    Rectangle paddle = {SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 30, PADDLE_WIDTH, PADDLE_HEIGHT};

    // Ball
    Vector2 ball = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    Vector2 ballSpeed = {4, -4};

    // Blocks
    Block blocks[BLOCK_ROWS * BLOCK_COLUMNS];
    for (int i = 0; i < BLOCK_ROWS; i++) {
        for (int j = 0; j < BLOCK_COLUMNS; j++) {
            blocks[i * BLOCK_COLUMNS + j].rect = (Rectangle){j * (BLOCK_WIDTH + 5) + 35, i * (BLOCK_HEIGHT + 5) + 50, BLOCK_WIDTH, BLOCK_HEIGHT};
            blocks[i * BLOCK_COLUMNS + j].active = true;
            blocks[i * BLOCK_COLUMNS + j].isBomb = (rand() % 100) < BOMB_CHANCE;
        }
    }

    bool gameWon = false;
    bool gameOver = false;
    int lives = MAX_LIVES;

    while (!WindowShouldClose()) {
        if (!gameWon && !gameOver) {
            // Paddle movement
            if (IsKeyDown(KEY_LEFT)) paddle.x -= 6;
            if (IsKeyDown(KEY_RIGHT)) paddle.x += 6;
            if (paddle.x < 0) paddle.x = 0;
            if (paddle.x > SCREEN_WIDTH - PADDLE_WIDTH) paddle.x = SCREEN_WIDTH - PADDLE_WIDTH;

            // Ball movement
            ball.x += ballSpeed.x;
            ball.y += ballSpeed.y;

            // Wall collisions
            if (ball.x <= 0 || ball.x >= SCREEN_WIDTH - BALL_RADIUS * 2) ballSpeed.x *= -1;
            if (ball.y <= 0) ballSpeed.y *= -1;

            // Paddle collision
            if (CheckCollisionCircleRec(ball, BALL_RADIUS, paddle)) ballSpeed.y *= -1;

            // Block collisions
            int blocksRemaining = 0;
            for (int i = 0; i < BLOCK_ROWS * BLOCK_COLUMNS; i++) {
                if (blocks[i].active) {
                    blocksRemaining++;
                    if (CheckCollisionCircleRec(ball, BALL_RADIUS, blocks[i].rect)) {
                        if (blocks[i].isBomb) {
                            // Destroy surrounding blocks
                            int row = i / BLOCK_COLUMNS;
                            int col = i % BLOCK_COLUMNS;
                            for (int dx = -1; dx <= 1; dx++) {
                                for (int dy = -1; dy <= 1; dy++) {
                                    int index = (row + dx) * BLOCK_COLUMNS + (col + dy);
                                    if (index >= 0 && index < BLOCK_ROWS * BLOCK_COLUMNS) {
                                        blocks[index].active = false;
                                    }
                                }
                            }
                        } else {
                            blocks[i].active = false;
                        }
                        ballSpeed.y *= -1;
                        break;
                    }
                }
            }

            // Check win condition
            if (blocksRemaining == 0) {
                gameWon = true;
            }

            // Ball out of bounds
            if (ball.y > SCREEN_HEIGHT) {
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                } else {
                    ball = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
                    ballSpeed = (Vector2){4, -4};
                }
            }

            // Debug: Remove all blocks when pressing 'P'
            if (IsKeyPressed(KEY_P)) {
                for (int i = 0; i < BLOCK_ROWS * BLOCK_COLUMNS; i++) {
                    blocks[i].active = false;
                }
                gameWon = true;
            }

            // Instantly lose game when pressing 'X'
            if (IsKeyPressed(KEY_X)) {
                gameOver = true;
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        if (gameWon) {
            DrawText("YOU WIN!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 40, GREEN);
        } else if (gameOver) {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 20, 40, RED);
        } else {
            DrawRectangleRec(paddle, WHITE);
            DrawCircleV(ball, BALL_RADIUS, WHITE);

            for (int i = 0; i < BLOCK_ROWS * BLOCK_COLUMNS; i++) {
                if (blocks[i].active) {
                    if (blocks[i].isBomb) {
                        DrawRectangleRec(blocks[i].rect, GRAY);
                        DrawRectangle(blocks[i].rect.x + BLOCK_WIDTH / 4, blocks[i].rect.y + BLOCK_HEIGHT / 4, BLOCK_WIDTH / 2, BLOCK_HEIGHT / 2, BLACK);
                    } else {
                        DrawRectangleRec(blocks[i].rect, WHITE);
                    }
                }
            }

            // Draw lives in top-left corner
            for (int i = 0; i < lives; i++) {
                DrawRectangle(10 + (i * 30), 10, 20, 20, RED);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
