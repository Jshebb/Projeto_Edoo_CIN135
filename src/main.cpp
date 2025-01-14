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
    
    
    Block block(1000, 50, 0, 550);
    Block block2(400, 50, 200, 400);
    
    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

    // Initialize the player sprite
    player.setSprite(LoadTexture("sprites/image.png"));
    tilemap.setTexture(LoadTexture("sprites/stone.jpg"));

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(Black);
            player.Draw();
            block.Draw();
            block2.Draw();
            
            Rectangle playerRec = player.getRec();
            Rectangle playerBelowRec = player.getBelowRec();
            Rectangle floor = block.getTopRec();
            Rectangle floor2 = block2.getTopRec();

            //collision detection
            playerRec.x = player.getPosX();
            playerRec.y = player.getPosY();

            playerBelowRec.x = player.getPosX();
            playerBelowRec.y = player.getPosY() + player.getHeight();
            
            bool coli = CheckCollisionRecs(playerBelowRec, floor);
            bool coli2 = CheckCollisionRecs(playerBelowRec, floor2);

            if (coli || coli2) { 
                player.setSpeedY(0); //stop the player from falling, player not jumping
                player.setGrounded(true);
            } else {
                player.setGrounded(false);
            }
            

        
            player.Update();


            DrawText(("posY: " + std::to_string(player.getPosY())).c_str(), 100, 50, 20, WHITE);
            DrawText(("speedY: " + std::to_string(player.getSpeedY())).c_str(), 300, 50, 20, WHITE);
            DrawText(("grounded: " + std::to_string(player.getGrounded())).c_str(), 600, 50, 20, WHITE);
            DrawText(("Colliding with floor 2: " + std::to_string((coli2))).c_str(), 400, 200, 20, WHITE);


        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
