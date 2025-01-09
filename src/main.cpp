#include <raylib.h>
#include "ball.h"
#include "entity.h"

int main() 
{
    const Color darkGreen = {20, 160, 133, 255};
    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    
    Ball ball;
    Block block(800, 50, 0, 550);
    
    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(darkGreen);
            ball.Draw();
            ball.Update();
            block.Draw();
        EndDrawing();
    }
    
    CloseWindow();
}