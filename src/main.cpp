#include <raylib.h>
#include <string>
#include "player.h"
#include "entity.h"

int main() 
{
    const Color Black = {0, 0, 0, 255};
    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    
    Player player;
    Block block(800, 50, 0, 550);
    Block block2(600, 50, 200, 400);
    
    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

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
}