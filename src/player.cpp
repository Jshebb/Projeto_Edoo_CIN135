#include "Player.h"
#include "tilemap.h"
#include "inventory.h"
#include <raylib.h>
#include <vector>
#include <cmath>
#include <string>
#include <iostream>

// Constructor
Player::Player()
    : position({ 0, 0 }),
      speed({ 0, 0 }),
      width(32),
      height(32),
      maxSpeed(5.0f),
      gravity(0.5f),
      friction(1.0f),
      grounded(true),
      playerSprite({ 0 }),
      currentState(IDLE),    // Default currentState is idle
      currentFrame(0),       // Default frame is 0
      frameTime(0.0f),       // Initialize frame time to 0
      frameDuration(0.1f)    // Duration per frame (in seconds)
{
    updateRectangles();
}

// Update the player's rectangles (bounds)
void Player::updateRectangles() {
    playerRec = { position.x, position.y, (float)width, (float)height };
    belowRec = { position.x, position.y + height, (float)width, 1 };
}

// Initialize the camera
void Player::initializeCamera(Tilemap tilemap) {
    camera.target = { position.x, position.y };
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

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

void Player::Update(const Tilemap& tilemap, float deltaTime) {
    // Horizontal movement
    if (IsKeyDown(KEY_D)) {
        speed.x = fmin(speed.x + 1.0f, maxSpeed);
        currentState = RUNNING;  // Change to running currentState when moving
    } else if (IsKeyDown(KEY_A)) {
        speed.x = fmax(speed.x - 1.0f, -maxSpeed);
        currentState = RUNNING;  // Change to running currentState when moving
    } else {
        speed.x = (speed.x > 0) ? fmax(speed.x - friction, 0) : fmin(speed.x + friction, 0);
        if (grounded) {
            currentState = IDLE;  // Change to idle when not moving
        }
    }

    // Jumping
    if (IsKeyPressed(KEY_SPACE) && grounded) {
        speed.y = -10.0;
        grounded = false;  // Ensure the player is not grounded after jumping
        currentState = JUMPING;  // Change to jumping currentState
    }

    // Apply gravity
    speed.y = fmin(speed.y + gravity, maxSpeed);

    // Horizontal movement and stair climbing
    position.x += speed.x;
    updateRectangles();
    if (tilemap.checkCollision(playerRec)) {
        // Check for a "stair" in front of the player
        Rectangle stepCheck = { playerRec.x, playerRec.y - playerRec.height, playerRec.width, playerRec.height };
        if (!tilemap.checkCollision(stepCheck)) {
            // Climb the step
            position.y -= 32;  // Adjust the height to climb the step
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
        if (speed.x == 0){
            currentState = IDLE;  // Reset currentState when grounded
        }
        updateRectangles();
    } else {
        grounded = false;
        if (speed.y > 0) currentState = JUMPING;  // Update currentState to falling if in the air
    }

    // Adjust the frame duration for idle state to make the animation slower
    if (currentState == IDLE) {
        frameDuration = 0.2f; // Slower frame duration for idle state
    } else {
        frameDuration = 0.1f; // Default frame duration for other states
    }

    // Update animation frame
    frameTime += deltaTime; // Accumulate time
    if (frameTime >= frameDuration) {
        frameTime = 0.0f;
        if (currentState == JUMPING) {
            if (speed.y > 0){
                currentFrame = 1;
            }
            else if (speed.y < 0){
                currentFrame = 0;
            }
        } else {
            // Cycle through frames for other states
            currentFrame = (currentFrame + 1) % getcurrentStateFrameCount(currentState);
        }
    }

    // Smoothly update camera to follow the player
    float cameraSmoothness = 0.1f;  // Lower values for slower, smoother movement
    Vector2 desiredTarget = { position.x + width / 2, position.y + height / 2 };
    camera.target.x += (desiredTarget.x - camera.target.x) * cameraSmoothness;
    camera.target.y += (desiredTarget.y - camera.target.y) * cameraSmoothness;

}


int Player::getcurrentStateFrameCount(PlayerState currentState) const {
    switch (currentState) {
        case IDLE: return 4;  // Full row count
        case RUNNING: return 6;
        case JUMPING: return 2;  // Fixed frame count for jumping
        default: return 1;  // Fallback
    }
}


void Player::Draw() const {
    // Get the current row based on the player state
    int row = static_cast<int>(currentState); // Assuming PlayerState corresponds to the row index

    // Cap the current frame to ensure it doesn't exceed the frame count for the current state
    int frameCount = getcurrentStateFrameCount(currentState);
    int validFrame = currentFrame % frameCount; // Ensure frame stays within bounds

    // Define the portion of the sprite sheet to render (source rectangle)
    Rectangle sourceRec = { validFrame * 32, row * 32, 32, 32 };

    // Define the destination rectangle (scaled to 64x64) with a height offset
    Rectangle destRec = { position.x - 16, position.y - 32.0f, 64.0f, 64.0f };

    // Flip horizontally if moving left
    if (speed.x < 0) {
        sourceRec.width = -sourceRec.width; // Flip the width of the source rectangle
        sourceRec.x += sourceRec.width;     // Adjust the x position of the source rectangle to compensate for the flip
        destRec.x += 16.0f;                // Adjust the position for the flipped sprite
    }

    // Origin is set to (0, 0), no rotation
    Vector2 origin = { 0, 0 };

    // Draw the sprite (flip handled by modifying the source rectangle)
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

Camera2D Player::getCamera() const {
    return camera;
}

int Player::getHeight() {
    return height;
}
