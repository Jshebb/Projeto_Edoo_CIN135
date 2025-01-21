#include <raylib.h>
#include <string>
#include "player.h"
#include "tilemap.h"
#include "inventory.h"

int main() 
{
    const Color Black = {0, 0, 0, 255};
    
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;

    DropManager dropManager(30);
    
    Tilemap tilemap(50, 10000, 32.0f, dropManager);
    Player player;
    tilemap.generateWorld(); 

    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

    Texture2D BlocksSheet = LoadTexture("sprites/BlocksSpriteSheet.png");
    Texture2D DropsSheet = LoadTexture("sprites/DropsSpriteSheet.png");
    Texture2D playerSprite = LoadTexture("sprites/Player.png");
    Texture2D BackGround = LoadTexture("sprites/basesemnuvens.png");
   

    // Initialize the player sprite
    player.setSprite(playerSprite);
    tilemap.setTexture(BlocksSheet);

    int x = (10000 / 2) - 32;
    int y = 0;

    Vector2 playerPos = {x, 0};

    playerPos.y = tilemap.getGroundLevel(x);
    player.setPosition(playerPos);
    player.initializeCamera(tilemap);
    dropManager = tilemap.getDropManager();


    

    while (!WindowShouldClose())
    {
        // Update the player
        player.Update(tilemap);

        BeginDrawing();
            ClearBackground(Black);
            DrawTexture(BackGround, 0, 0, WHITE);

            BeginMode2D(player.getCamera());
                
                // Draw the tilemap and player
                tilemap.Draw(player.getCamera(), 32);
                player.Draw();
                tilemap.TilePlacement(player.getCamera(), tilemap.getTileSize(), player.getPosition(), DropsSheet, BlocksSheet);
            EndMode2D();

            // Draw debug information on screen
            Vector2 playerPosition = player.getPosition();  // Assuming Player class has a getPosition method
            Vector2 playerSpeed = player.getSpeed();        // Assuming Player class has a getSpeed method
            bool isGrounded = player.isGrounded();          // Assuming Player class has an isGrounded method
            Rectangle playerRec = player.getRec();    // Assuming Player class has a getRectangle method
            DrawText(TextFormat("Position: (%.2f, %.2f)", playerPosition.x, playerPosition.y), 10, 10, 20, RAYWHITE);
            DrawText(TextFormat("Speed: (%.2f, %.2f)", playerSpeed.x, playerSpeed.y), 10, 40, 20, RAYWHITE);
            DrawText(TextFormat("Grounded: %s", isGrounded ? "True" : "False"), 10, 70, 20, RAYWHITE);
            DrawText(TextFormat("Rectangle: (x=%.2f, y=%.2f, w=%.2f, h=%.2f)", 
                                playerRec.x, playerRec.y, playerRec.width, playerRec.height), 10, 100, 20, RAYWHITE);

        EndDrawing();
    }
    
    CloseWindow();
}
