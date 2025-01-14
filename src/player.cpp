#include "Player.h"
#include "entity.h"
#include "Player.h"
#include <raylib.h>
#include <vector>
#include <cmath>
#include <string>


// Constructor
Player::Player(Tilemap tilemap)
    : position({ 300, 20 }),
      speed({ 0, 0 }),
      width(32),
      height(64),
      maxSpeed(7.0f),
      gravity(0.5f),
      friction(1.0f),
      grounded(true),
      playerSprite()
                        {
    updateRectangles();

    // Inicializar a câmera by LM
    camera.target = { position.x + width / 2, position.y + height / 2 };
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f }; 
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    Vector2 worldSize = { tilemap.getCols() * tilemap.getTileSize(), tilemap.getRows() * tilemap.getTileSize() };

    // Atualizar a câmera para seguir o jogador by LM
    camera.target = { position.x + width / 2, position.y + height / 2 };

    // Limitar a câmera às bordas do mundo by LM
    if (camera.target.x < GetScreenWidth() / 2.0f)
        camera.target.x = GetScreenWidth() / 2.0f;
    if (camera.target.y < GetScreenHeight() / 2.0f)
        camera.target.y = GetScreenHeight() / 2.0f;
    if (camera.target.x > worldSize.x - GetScreenWidth() / 2.0f)
        camera.target.x = worldSize.x - GetScreenWidth() / 2.0f;
    if (camera.target.y > worldSize.y - GetScreenHeight() / 2.0f)
        camera.target.y = worldSize.y - GetScreenHeight() / 2.0f;
}

// Updates the player's rectangles (bounds)
void Player::updateRectangles() {
    playerRec = { position.x, position.y, (float)width, (float)height };
    belowRec = { position.x, position.y + height, (float)width, 1 };
}

void Player::Update(const Tilemap& tilemap) {
    // Horizontal movement
    if (IsKeyDown(KEY_D)) {
        speed.x = fmin(speed.x + 1.0f, maxSpeed);
    } else if (IsKeyDown(KEY_A)) {
        speed.x = fmax(speed.x - 1.0f, -maxSpeed);
    } else {
        speed.x = (speed.x > 0) ? fmax(speed.x - friction, 0) : fmin(speed.x + friction, 0);
    }

    // Jumping
    if (IsKeyPressed(KEY_SPACE) && grounded) {
        speed.y = -15.0;
        grounded = false; // Ensure the player is not grounded after jumping
    }

    // Apply gravity
    speed.y = fmin(speed.y + gravity, maxSpeed);
    
    // Horizontal movement and stair climbing
    position.x += speed.x;
    updateRectangles();
    if (tilemap.checkCollision(playerRec)) {
        // Check for a "stair" in front of the player
        Rectangle stepCheck = {
            playerRec.x, 
            playerRec.y - playerRec.height / 2, // Check slightly above the player
            playerRec.width, 
            playerRec.height
        };

        if (!tilemap.checkCollision(stepCheck)) {
            // Climb the step
            position.y -= 32; // Adjust the height to climb the step
            updateRectangles();
        } else {
            // Reset position if unable to climb
            position.x -= speed.x;
            speed.x = 0;
            updateRectangles();
        }
    }

    // Vertical movement and collision
    position.y += speed.y;
    updateRectangles();
    if (tilemap.checkCollision(playerRec)) {
        position.y -= speed.y;
        speed.y = 0;
        grounded = true;
        updateRectangles();
    } else {
        grounded = false;
    }

    // Smoothly update camera to follow the player
    float cameraSmoothness = 0.1f; // Lower values for slower, smoother movement

    // Desired camera target (centered on the player)
    Vector2 desiredTarget = { position.x + width / 2, position.y + height / 2 };

    // Interpolate camera target to smooth out movement
    camera.target.x += (desiredTarget.x - camera.target.x) * cameraSmoothness;
    camera.target.y += (desiredTarget.y - camera.target.y) * cameraSmoothness;

    // Offset remains centered on the screen
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    // Limit the camera to the edges of the world
    Vector2 worldSize = { tilemap.getCols() * tilemap.getTileSize(), tilemap.getRows() * tilemap.getTileSize() };

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
    Rectangle destRec = { position.x, position.y, (float)width, (float)height };
    Rectangle sourceRec = { 0, 0, (float)playerSprite.width, (float)playerSprite.height };
    Vector2 origin = { 0, 0 };
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

Rectangle Player::getRec() const {
    return playerRec;
}

// by LM
Camera2D Player::getCamera() const
{
    return camera;
}
