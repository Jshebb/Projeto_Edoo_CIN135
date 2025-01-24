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

    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

    bool showMenu = true;

    // Carregar a imagem do menu
    Texture2D menuBackground = LoadTexture("sprites/menupixel.png");
    //botões invisíveis
    Rectangle startButton = { screenWidth / 2 - 188, screenHeight / 2 + 34, 377, 61 };
    Rectangle exitButton = { screenWidth / 2 - 188, screenHeight / 2 + 140, 377, 61 };

    while (showMenu && !WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();
        bool startButtonHovered = CheckCollisionPointRec(mousePoint, startButton);
        bool exitButtonHovered = CheckCollisionPointRec(mousePoint, exitButton);

        if (startButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showMenu = false;
        }
        if (exitButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            CloseWindow();
            return 0;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Desenhar a imagem do menu
        DrawTexture(menuBackground, 0, 0, WHITE);

        // Botões invisíveis (para depuração, as bordas podem ser desenhadas)
        if (startButtonHovered) {
            DrawRectangleLinesEx(startButton, 2, GREEN); // Borda vermelha para depuração
        }
        if (exitButtonHovered) {
            DrawRectangleLinesEx(exitButton, 2, GREEN); // Borda vermelha para depuração
        }

        EndDrawing();
    }

    // Loading screen with real tasks
    bool loading = true;
    int progress = 0;
    const int totalTasks = 7; // Total number of tasks to load (increased by 1 for Tilemap initialization)

    Texture2D BlocksSheet;
    Texture2D DropsSheet;
    Texture2D playerSprite;
    Texture2D BackGround;
    Texture2D InventorySprite;
    Texture2D clouds;
    Texture2D loadingBackground = LoadTexture("sprites/loadingdesfocado.png");
    Texture2D InventoryTile = LoadTexture("sprites/InventoryTile.png");

    Tilemap* tilemap = nullptr; // Pointer for the Tilemap instance
    Player player;
    Inventory inventory(770, 22, InventoryTile);
    DropManager dropManager(30);
    

while (loading && !WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Desenhar a imagem de fundo
    DrawTexture(loadingBackground, 0, 0, WHITE);

    // Draw loading text and progress bar
    DrawText("Loading...", screenWidth / 2 - MeasureText("Loading...", 40) / 2, screenHeight / 2 - 50, 40, WHITE);
    int barWidth = 400;
    int barHeight = 30;
    int barX = screenWidth / 2 - barWidth / 2;
    int barY = screenHeight / 2;
    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, barY, (barWidth * progress) / totalTasks, barHeight, GREEN);

    // Display the current task being loaded
    const char* loadingText;
    switch (progress) {
        case 0: loadingText = "Initializing tilemap..."; break;
        case 1: loadingText = "Generating world..."; break;
        case 2: loadingText = "Loading block textures..."; break;
        case 3: loadingText = "Loading drop textures..."; break;
        case 4: loadingText = "Loading player sprite..."; break;
        case 5: loadingText = "Loading background textures..."; break;
        case 6: loadingText = "Loading inventory textures..."; break;
        default: loadingText = "Finalizing..."; break;
    }
    DrawText(loadingText, screenWidth / 2 - MeasureText(loadingText, 20) / 2, screenHeight / 2 + 50, 20, WHITE);

    EndDrawing();

    // Perform loading tasks step by step
    if (progress == 0) {
        tilemap = new Tilemap(20, 10000, 32.0f, dropManager, inventory); // Initialize the Tilemap
    } else if (progress == 1) {
        tilemap->generateWorld(); // Simulate a heavy task
        WaitTime(0.5);
    } else if (progress == 2) {
        BlocksSheet = LoadTexture("sprites/BlocksSpriteSheet.png");
    } else if (progress == 3) {
        DropsSheet = LoadTexture("sprites/DropsSpriteSheet.png");
    } else if (progress == 4) {
        playerSprite = LoadTexture("sprites/Player.png");
        player.setSprite(playerSprite); // Initialize the player sprite
    } else if (progress == 5) {
        BackGround = LoadTexture("sprites/basesemnuvens.png");
        clouds = LoadTexture("sprites/nuvensfrente.png");
    } else if (progress == 6) {
        InventorySprite = LoadTexture("sprites/inventario.png");
        tilemap->setTexture(BlocksSheet); // Link textures to the tilemap
        loading = false; // Finish loading
    }

    progress++;
}


    int x = (10000 / 2) - 32;
    int y = 0;
    Vector2 playerPos = {x, 0};
    dropManager = tilemap->getDropManager();
    playerPos.y = tilemap->getGroundLevel(x);
    player.setPosition(playerPos);
    player.initializeCamera(*tilemap);
    
    
    


    while (!WindowShouldClose())
    {
        // Update the player
        player.Update(*tilemap);

        BeginDrawing();
        ClearBackground(Black);
        DrawTexture(BackGround, 0, -400, WHITE);
        DrawTexture(clouds, 0, -400, WHITE);

        BeginMode2D(player.getCamera());
            tilemap->Draw(player.getCamera(), 32);
            player.Draw();
            tilemap->TilePlacement(player.getCamera(), tilemap->getTileSize(), player.getPosition(), DropsSheet, BlocksSheet);
        EndMode2D();
        tilemap->UpdateInventory();
        tilemap->DrawInventory();


        Vector2 playerPosition = player.getPosition(); 
        Vector2 playerSpeed = player.getSpeed();        
        bool isGrounded = player.isGrounded();         
        Rectangle playerRec = player.getRec();         
        DrawText(TextFormat("Position: (%.2f, %.2f)", playerPosition.x, playerPosition.y), 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Speed: (%.2f, %.2f)", playerSpeed.x, playerSpeed.y), 10, 40, 20, RAYWHITE);
        DrawText(TextFormat("Grounded: %s", isGrounded ? "True" : "False"), 10, 70, 20, RAYWHITE);
        DrawText(TextFormat("Rectangle: (x=%.2f, y=%.2f, w=%.2f, h=%.2f)", 
                            playerRec.x, playerRec.y, playerRec.width, playerRec.height), 10, 100, 20, RAYWHITE);

        EndDrawing();
    }

    delete tilemap; // Clean up dynamically allocated memory
    CloseWindow();
}
