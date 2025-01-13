#include <raylib.h>
#include <string>
#include "player.h"
#include "entity.h"

int main() 
{
    const Color Black = {0, 0, 0, 255};
    
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;
    
    Player player;
    Tilemap tilemap(100, 100, 32.0f);
    tilemap.setTile(0, 10, true, RED);
    tilemap.setTile(0, 9, true, RED); // Add a single solid tile for testing
    tilemap.setTile(1, 10, true, RED);
    tilemap.setTile(2, 10, true, RED);
    tilemap.setTile(3, 10, true, RED);
    tilemap.setTile(4, 10, true, RED);
    tilemap.setTile(4, 10, true, RED);
    tilemap.setTile(4, 10, true, RED);
    tilemap.setTile(4, 7, true, RED);
    tilemap.setTile(5, 8, true, RED);
    tilemap.setTile(6, 8, true, RED);
    tilemap.setTile(7, 9, true, RED);
    
    
    
    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

    // Initialize the player sprite
    player.setSprite(LoadTexture("sprites/image.png"));
    tilemap.setTexture(LoadTexture("sprites/stone.jpg"));

    while (!WindowShouldClose())
    {
        // Update the player
        player.Update(tilemap);

        BeginDrawing();
            ClearBackground(Black);

            // Draw the tilemap and player
            tilemap.Draw();
            player.Draw();

            // Draw debug information on screen
            Vector2 playerPosition = player.getPosition();  // Assuming Player class has a getPosition method
            Vector2 playerSpeed = player.getSpeed();        // Assuming Player class has a getSpeed method
            bool isGrounded = player.isGrounded();          // Assuming Player class has an isGrounded method
            Rectangle playerRec = player.getRec();    // Assuming Player class has a getRectangle method
            Rectangle feetCheck = { player.getPosition().x, player.getPosition().y + player.getRec().height + 0.1f, player.getRec().width, 0.1f };
            DrawRectangleLines(feetCheck.x, feetCheck.y, feetCheck.width, feetCheck.height, GREEN);

        

            DrawText(TextFormat("Position: (%.2f, %.2f)", playerPosition.x, playerPosition.y), 10, 10, 20, RAYWHITE);
            DrawText(TextFormat("Speed: (%.2f, %.2f)", playerSpeed.x, playerSpeed.y), 10, 40, 20, RAYWHITE);
            DrawText(TextFormat("Grounded: %s", isGrounded ? "True" : "False"), 10, 70, 20, RAYWHITE);
            DrawText(TextFormat("Rectangle: (x=%.2f, y=%.2f, w=%.2f, h=%.2f)", 
                                playerRec.x, playerRec.y, playerRec.width, playerRec.height), 10, 100, 20, RAYWHITE);

        EndDrawing();
    }
    
    CloseWindow();
}
