#include "Player.h"
#include "entity.h"
#include <raylib.h>
#include <vector>
#include <cmath>
#include <string>

// Constructor
Player::Player(Tilemap tilemap)
    : position({300, 20}),
      speed({0, 0}),
      width(32),
      height(64),
      maxSpeed(7.0f),
      gravity(0.5f),
      friction(1.0f),
      grounded(true),
      playerSprite() {
    updateRectangles();

    // Inicializar a câmera by LM
    camera.target = {position.x + width / 2, position.y + height / 2};
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    Vector2 worldSize = {tilemap.getCols() * tilemap.getTileSize(), tilemap.getRows() * tilemap.getTileSize()};

    // Atualizar a câmera para seguir o jogador
    camera.target = {position.x + width / 2, position.y + height / 2};
}

// Updates the player's rectangles (bounds)
void Player::updateRectangles() {
    playerRec = {position.x, position.y, (float)width, (float)height};
    belowRec = {position.x, position.y + height, (float)width, 1};
}

void Player::Update(Tilemap& tilemap, DropManager& dropManager, vector<Texture2D>& inventory) {
    // Verificar cliques do mouse para destruir blocos
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePosition = GetMousePosition();
        Vector2 worldPosition = GetScreenToWorld2D(mousePosition, camera);

        // Obter posição alinhada ao grid
        float tileSize = tilemap.getTileSize();
        int tileX = static_cast<int>(worldPosition.x / tileSize);
        int tileY = static_cast<int>(worldPosition.y / tileSize);

        const Tile& tile = tilemap.getTileAt(tileX * tileSize, tileY * tileSize);
        if (tile.isBreakable()) {
            tilemap.setTile(tileX, tileY, false, BLACK, 0);
            dropManager.addDrop({tileX * tileSize, tileY * tileSize}, tile.getDropSprite());
        }
    }

    // Coletar drops
    tilemap.collectDrops(*this, inventory);

    // Movimentação horizontal
    if (IsKeyDown(KEY_D)) {
        speed.x = fmin(speed.x + 1.0f, maxSpeed);
    } else if (IsKeyDown(KEY_A)) {
        speed.x = fmax(speed.x - 1.0f, -maxSpeed);
    } else {
        speed.x = (speed.x > 0) ? fmax(speed.x - friction, 0) : fmin(speed.x + friction, 0);
    }

    // Pular
    if (IsKeyPressed(KEY_SPACE) && grounded) {
        speed.y = -15.0;
        grounded = false;
    }

    // Aplicar gravidade
    speed.y = fmin(speed.y + gravity, maxSpeed);

    // Movimentação horizontal e subir escadas
    position.x += speed.x;
    updateRectangles();
    if (tilemap.checkCollision(playerRec)) {
        position.x -= speed.x;
        speed.x = 0;
    }

    // Movimentação vertical e colisão
    position.y += speed.y;
    updateRectangles();
    if (tilemap.checkCollision(playerRec)) {
        position.y -= speed.y;
        speed.y = 0;
        grounded = true;
    } else {
        grounded = false;
    }

    // Atualizar a câmera para seguir o jogador
    float cameraSmoothness = 0.1f; // Menor valor para movimentos mais suaves
    Vector2 desiredTarget = {position.x + width / 2, position.y + height / 2};

    camera.target.x += (desiredTarget.x - camera.target.x) * cameraSmoothness;
    camera.target.y += (desiredTarget.y - camera.target.y) * cameraSmoothness;

    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    // Limitar a câmera às bordas do mundo
    Vector2 worldSize = {tilemap.getCols() * tilemap.getTileSize(), tilemap.getRows() * tilemap.getTileSize()};
    float halfScreenWidth = GetScreenWidth() / 2.0f;
    float halfScreenHeight = GetScreenHeight() / 2.0f;

    if (camera.target.x < halfScreenWidth)
        camera.target.x = halfScreenWidth;
    if (camera.target.y < halfScreenHeight)
        camera.target.y = halfScreenHeight;
    if (camera.target.x > worldSize.x - halfScreenWidth)
        camera.target.x = worldSize.x - halfScreenWidth;
    if (camera.target.y > worldSize.y - halfScreenHeight)
        camera.target.y = worldSize.y - halfScreenHeight;
}


// Draws the player on the screen
void Player::Draw() const {
    Rectangle destRec = {position.x, position.y, (float)width, (float)height};
    Rectangle sourceRec = {0, 0, (float)playerSprite.width, (float)playerSprite.height};
    Vector2 origin = {0, 0};
    DrawTexturePro(playerSprite, sourceRec, destRec, origin, 0.0f, WHITE);
}

// Getters
Vector2 Player::getPosition() const { return position; }
Vector2 Player::getSpeed() const { return speed; }
bool Player::isGrounded() const { return grounded; }

// Setters
void Player::setPosition(Vector2 newPosition) {
    position = newPosition;
    updateRectangles();
}

void Player::setSpeed(Vector2 newSpeed) { speed = newSpeed; }
void Player::setSprite(Texture2D sprite) { playerSprite = sprite; }

Rectangle Player::getRec() const { return playerRec; }

Camera2D Player::getCamera() const { return camera; }
