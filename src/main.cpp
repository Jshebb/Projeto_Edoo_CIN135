#include <raylib.h>
#include <string>
#include "player.h"
#include "entity.h"
#include "drop_manager.h"

int main() 
{
    const Color Black = {0, 0, 0, 255};
    
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;
    
    Tilemap tilemap(100, 100, 32.0f);
    Player player(tilemap);
    tilemap.generateWorld(grassDropSprite, dirtDropSprite, stoneDropSprite);

    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

    // Inicializa os sprites
    Texture2D playerSprite = LoadTexture("sprites/image.png");
    player.setSprite(playerSprite);

    Texture2D grassSprite = LoadTexture("sprites/terradropada.png");
    Texture2D dirtSprite = LoadTexture("sprites/terradropada.png");
    Texture2D stoneSprite = LoadTexture("sprites/terradropada.png");

    tilemap.setTexture(
        LoadTexture("sprites/Terra_Nova.png"), 
        LoadTexture("sprites/Terra.png"), 
        LoadTexture("sprites/Pedra.png")
    );

    // Inicializa o DropManager
    DropManager dropManager(0.5f, 32.0f); // Gravidade 0.5f, tamanho do tile 32.0f

    // Vetor para armazenar o inventário
    vector<Texture2D> inventory;

    while (!WindowShouldClose())
    {
        // Atualiza o jogador e o DropManager
        player.Update(tilemap, dropManager, inventory);
        dropManager.updateDrops(); // Atualiza a física dos drops

        BeginDrawing();
            ClearBackground(Black);

            BeginMode2D(player.getCamera());
                // Desenha o mapa de tiles e o jogador
                tilemap.Draw();
                player.Draw();
                dropManager.drawDrops(); // Desenha os drops
            EndMode2D();

            // Exibe o inventário
            int slotSize = 50; // Tamanho de cada slot no inventário
            int xStart = 10, yStart = 10; // Posição inicial do inventário

            for (size_t i = 0; i < inventory.size(); ++i) {
                int x = xStart + (i % 10) * slotSize; // Posição X do slot
                int y = yStart + (i / 10) * slotSize; // Posição Y do slot
                DrawRectangle(x, y, slotSize, slotSize, DARKGRAY); // Fundo do slot
                DrawTextureEx(inventory[i], {float(x + 5), float(y + 5)}, 0.0f, 0.4f, WHITE); // Sprite do item
            }

            // Debug: Exibe informações do jogador
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

    // Libera texturas antes de fechar
    UnloadTexture(playerSprite);
    UnloadTexture(grassSprite);
    UnloadTexture(dirtSprite);
    UnloadTexture(stoneSprite);

    CloseWindow();
    return 0;
}
