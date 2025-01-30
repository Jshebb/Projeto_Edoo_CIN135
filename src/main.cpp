#include <raylib.h>
#include <string>
#include "player.h"
#include "tilemap.h"
#include "inventory.h"

int main() 
{
    
    const Color Black = {0, 0, 0, 255}; // definicao de cor para fundo de tela
    constexpr int screenWidth = 1280;   // dimensoes de tela do jogo
    constexpr int screenHeight = 720;   // dimensoes de tela do jogo

    InitWindow(screenWidth, screenHeight, "C+Mine");  // inializa a tela do jogo
    SetTargetFPS(60); // taxa de quadros por segundo

    bool showMenu = true;
    bool chooseMapSize = false; // LM Controle de escolha do tamanho do mapa
    int mapSize = 0;           // LM Tamanho do mapa (0 = não escolhido, 1 = pequeno, 2 = médio, 3 = grande)
    // parallax
    float backgroundWidth = 0;  // largura do fundo
    float backgroundHeight = 0; //altura do fundo
    float backgroundOffsetX = 0;
    float backgroundOffsetY = 0;
    float verticalAdjustment = -300; // Ajuste vertical para subir a imagem
    float cloudsVerticalAdjustment = -360; // Ajuste vertical para elevar as nuvens
    float cloudsOffsetX = 0; //deslocamento das nuvens
    float cloudsOffsetY = 0; //deslocamento das nuvens

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
            chooseMapSize = true; // LM tela de escolha do tamanho do mapa
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

    // imagem de fundo para a tela de escolha do mapa
    Texture2D chooseMapBackground = LoadTexture("sprites/choosemapnovo.png");

    // Tela para escolher o tamanho do mapa
    Rectangle smallMapButton = { screenWidth / 2 - 188, screenHeight / 2 + 34, 377, 61 };
    Rectangle mediumMapButton = { screenWidth / 2 - 188, screenHeight / 2 + 140, 377, 61 };
    Rectangle largeMapButton = { screenWidth / 2 - 188, screenHeight / 2 + 244, 377, 61 };

    while (chooseMapSize && !WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();
        bool smallButtonHovered = CheckCollisionPointRec(mousePoint, smallMapButton);
        bool mediumButtonHovered = CheckCollisionPointRec(mousePoint, mediumMapButton);
        bool largeButtonHovered = CheckCollisionPointRec(mousePoint, largeMapButton);

        if (smallButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            mapSize = 1; // mapa pequeno
            chooseMapSize = false;
        }
        if (mediumButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            mapSize = 2; // mapa médio
            chooseMapSize = false;
        }
        if (largeButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            mapSize = 3; // mapa grande
            chooseMapSize = false;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // imagem de fundo do menu de escolha do mapa
        DrawTexture(chooseMapBackground, 0, 0, WHITE);

        // Botões invisíveis com borda verde ao passar o mouse
        if (smallButtonHovered) {
            DrawRectangleLinesEx(smallMapButton, 2, GREEN);
        }
        if (mediumButtonHovered) {
            DrawRectangleLinesEx(mediumMapButton, 2, GREEN);
        }
        if (largeButtonHovered) {
            DrawRectangleLinesEx(largeMapButton, 2, GREEN);
        }

        EndDrawing();
    }

    // Após terminar o uso, libere a textura
    UnloadTexture(chooseMapBackground);

    // Tela de loading
    bool loading = true;
    int progress = 0;
    const int totalTasks = 7; // Tasks de loading

    Texture2D BlocksSheet;
    Texture2D DropsSheet;
    Texture2D playerSprite;
    Texture2D BackGround;
    Texture2D InventorySprite;
    Texture2D clouds;
    Texture2D loadingBackground = LoadTexture("sprites/loadingdesfocado.png");
    Texture2D InventoryTile = LoadTexture("sprites/InventoryTile.png");

    Tilemap* tilemap = nullptr; // Ponteiro pro Tilemap
    Player player;
    Inventory inventory(770, 22, InventoryTile);
    DropManager dropManager(30);
    

while (loading && !WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Desenhar a imagem de fundo
    DrawTexture(loadingBackground, 0, 0, WHITE);

    // Render de texto de loading e barra de progresso
    DrawText("Loading...", screenWidth / 2 - MeasureText("Loading...", 40) / 2, screenHeight / 2 - 50, 40, WHITE);
    int barWidth = 400;
    int barHeight = 30;
    int barX = screenWidth / 2 - barWidth / 2;
    int barY = screenHeight / 2;
    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, barY, (barWidth * progress) / totalTasks, barHeight, GREEN);

    // Display task atual
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

    // Loading em partes
    int mapWidth = 0;
    int mapHeight = 0;

    if (progress == 0) {
        // Ajustar o tamanho do mapa com base na escolha
        if (mapSize == 1) { 
            mapWidth = 5000;  // Mapa pequeno
            mapHeight = 50;
        } else if (mapSize == 2) {
            mapWidth = 10000; // Mapa médio
            mapHeight = 80;
        } else if (mapSize == 3) {
            mapWidth = 100000; // Mapa grande
            mapHeight = 200;
        }

        // Inicializar o Tilemap com os valores escolhidos
        tilemap = new Tilemap(mapHeight, mapWidth, 32.0f, dropManager, inventory);

    } else if (progress == 1) {
        tilemap->generateWorld(); // gera o mundo
    } else if (progress == 2) {
        BlocksSheet = LoadTexture("sprites/BlocksSpriteSheet.png");
    } else if (progress == 3) {
        DropsSheet = LoadTexture("sprites/DropsSpriteSheet.png");
    } else if (progress == 4) {
        playerSprite = LoadTexture("sprites/playerSheet.png");
        player.setSprite(playerSprite); // incializa o sprite do player
    } else if (progress == 5) {
        BackGround = LoadTexture("sprites/basesemnuvens.png");
        clouds = LoadTexture("sprites/nuvensfrente.png");
    } else if (progress == 6) {
        InventorySprite = LoadTexture("sprites/inventario.png");
        tilemap->setTexture(BlocksSheet); // texturas do tilemap
        loading = false; // termina o loading
    }

    progress++;
}

    // inicializacao de posicao do player
    int x = (10000 / 2) - 32;
    int y = 0;
    Vector2 playerPos = {x, 0};
    dropManager = tilemap->getDropManager();
    playerPos.y = tilemap->getGroundLevel(x);
    player.setPosition(playerPos);
    player.initializeCamera(*tilemap);
    
    //PARALLAX DO LM
    BackGround = LoadTexture("sprites/basesemnuvens.png");
    backgroundWidth = BackGround.width;
    backgroundHeight = BackGround.height + tilemap->getRows();


    // Loop do jogo
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        // Atualizar o jogador
        player.Update(*tilemap, deltaTime);

        // Atualizar o deslocamento do fundo com base na velocidade do jogador
        Vector2 playerSpeed = player.getSpeed();
        backgroundOffsetX -= playerSpeed.x * 0.2f; // Movimento horizontal (parallax suave)
        backgroundOffsetY -= playerSpeed.y * 0.2f; // Movimento vertical (parallax suave)

        // Garantir o looping
        if (backgroundOffsetX <= -backgroundWidth) backgroundOffsetX += backgroundWidth;
        if (backgroundOffsetX >= backgroundWidth) backgroundOffsetX -= backgroundWidth;
        if (backgroundOffsetY <= -backgroundHeight) backgroundOffsetY += backgroundHeight;
        if (backgroundOffsetY >= backgroundHeight) backgroundOffsetY -= backgroundHeight;

        // Atualizar o deslocamento das nuvens (movem-se mais devagar dando sensação de profundidade)
        cloudsOffsetX -= playerSpeed.x * 0.1f; // Movimento horizontal das nuvens
        cloudsOffsetY -= playerSpeed.y * 0.1f; // Movimento vertical das nuvens

        // Garantir o looping das nuvens
        if (cloudsOffsetX <= -backgroundWidth) cloudsOffsetX += backgroundWidth;
        if (cloudsOffsetX >= backgroundWidth) cloudsOffsetX -= backgroundWidth;
        if (cloudsOffsetY <= -backgroundHeight) cloudsOffsetY += backgroundHeight;
        if (cloudsOffsetY >= backgroundHeight) cloudsOffsetY -= backgroundHeight;

        BeginDrawing();
        ClearBackground(Black);

        // Desenhar o fundo com efeito de looping
        DrawTexture(BackGround, backgroundOffsetX, backgroundOffsetY + verticalAdjustment, WHITE);
        DrawTexture(BackGround, backgroundOffsetX + backgroundWidth, backgroundOffsetY + verticalAdjustment, WHITE);
        DrawTexture(BackGround, backgroundOffsetX - backgroundWidth, backgroundOffsetY + verticalAdjustment, WHITE);

        DrawTexture(BackGround, backgroundOffsetX, backgroundOffsetY + backgroundHeight + verticalAdjustment, WHITE);
        DrawTexture(BackGround, backgroundOffsetX, backgroundOffsetY - backgroundHeight + verticalAdjustment, WHITE);

        DrawTexture(BackGround, backgroundOffsetX + backgroundWidth, backgroundOffsetY + backgroundHeight + verticalAdjustment, WHITE);
        DrawTexture(BackGround, backgroundOffsetX - backgroundWidth, backgroundOffsetY + backgroundHeight + verticalAdjustment, WHITE);
        DrawTexture(BackGround, backgroundOffsetX + backgroundWidth, backgroundOffsetY - backgroundHeight + verticalAdjustment, WHITE);
        DrawTexture(BackGround, backgroundOffsetX - backgroundWidth, backgroundOffsetY - backgroundHeight + verticalAdjustment, WHITE);

        // Desenhar as nuvens com ajuste vertical e parallax mais lento
        DrawTexture(clouds, cloudsOffsetX, cloudsOffsetY + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX + backgroundWidth, cloudsOffsetY + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX - backgroundWidth, cloudsOffsetY + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX, cloudsOffsetY + backgroundHeight + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX, cloudsOffsetY - backgroundHeight + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX + backgroundWidth, cloudsOffsetY + backgroundHeight + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX - backgroundWidth, cloudsOffsetY + backgroundHeight + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX + backgroundWidth, cloudsOffsetY - backgroundHeight + cloudsVerticalAdjustment, WHITE);
        DrawTexture(clouds, cloudsOffsetX - backgroundWidth, cloudsOffsetY - backgroundHeight + cloudsVerticalAdjustment, WHITE);


        // Desenhar o restante do jogo
        // DrawTexture(clouds, 0, -400, WHITE); // Camada de nuvens (caso necessário)

        // Mode 2d para usar a camera 2d
        BeginMode2D(player.getCamera());
            tilemap->Draw(player.getCamera(), 32);
            player.Draw();
            tilemap->TilePlacement(player.getCamera(), tilemap->getTileSize(), player.getPosition(), DropsSheet, BlocksSheet);
        EndMode2D();

        // inventario (render e update)
        tilemap->UpdateInventory();
        tilemap->DrawInventory();

   
    
        // Debug
        Vector2 playerPosition = player.getPosition(); 
        playerSpeed = player.getSpeed(); //lm ctrl z     
        bool isGrounded = player.isGrounded();         
        Rectangle playerRec = player.getRec();         
        DrawText(TextFormat("Position: (%.2f, %.2f)", playerPosition.x, playerPosition.y), 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Speed: (%.2f, %.2f)", playerSpeed.x, playerSpeed.y), 10, 40, 20, RAYWHITE);
        DrawText(TextFormat("Grounded: %s", isGrounded ? "True" : "False"), 10, 70, 20, RAYWHITE);
        DrawText(TextFormat("Rectangle: (x=%.2f, y=%.2f, w=%.2f, h=%.2f)", 
                            playerRec.x, playerRec.y, playerRec.width, playerRec.height), 10, 100, 20, RAYWHITE);

        EndDrawing();
    }

    delete tilemap; // limpa memoria alocada
    CloseWindow();
}
