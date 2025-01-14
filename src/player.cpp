#include "Player.h"
#include "entity.h"
#include "Player.h"
#include <raylib.h>
#include <vector>
#include <cmath>
#include <string>


// Constructor
Player::Player()
    : position({ 0, 20 }),
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
    
    // Horizontal collision
    position.x += speed.x;
    updateRectangles();
    if (tilemap.checkCollision(playerRec)) {
        position.x -= speed.x;
        speed.x = 0;
        updateRectangles();
    }

    // Vertical collision
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
