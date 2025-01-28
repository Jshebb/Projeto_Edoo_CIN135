#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "tilemap.h"
#include "inventory.h"

class Player {
private:
    // Player properties
    Vector2 position;          // Player position
    Vector2 speed;             // Player speed
    int width, height;         // Dimensions of the player
    float maxSpeed;            // Maximum speed
    float gravity;             // Gravity strength
    float friction;            // Horizontal friction
    bool grounded;             // Whether the player is on the ground

    // Collision rectangles
    Rectangle playerRec;       // Rectangle representing the player's bounds
    Rectangle belowRec;        // Rectangle below the player for ground collision

    // Sprite and animation
    Texture2D playerSprite;    // Player sprite sheet
    int currentFrame;          // Current animation frame
    float frameTime;           // Time accumulator for frame updates
    float frameDuration;       // Duration of each frame (in seconds)
    bool isFlipped; //Se o sprite do player for espelhado

    // Camera
    Camera2D camera;           // Camera

    // Private helper methods
    void updateRectangles();   // Updates `playerRec` and `belowRec`
    void updateAnimation();    // Updates animation frames based on player state

public:
    Player();                  // Constructor

    // Player states for animations
    enum PlayerState { 
        IDLE,                  // Idle state
        RUNNING,               // Running state
        JUMPING,               // Jumping state
        FALLING                // Falling state
    };

    PlayerState currentState;  // Current state of the player

    // Public methods
    void Update(const Tilemap& tilemap, float deltaTime);  // Updates player movement, collisions, and animations
    void Draw() const;                    // Draws the player on the screen

    // Getters
    Vector2 getPosition() const;
    Vector2 getSpeed() const;
    Rectangle getRec() const;
    bool isGrounded() const;
    Camera2D getCamera() const; // Obtain the camera
    int getHeight();
    int getcurrentStateFrameCount(PlayerState state) const;

    // Setters
    void setPosition(Vector2 newPosition);
    void setSpeed(Vector2 newSpeed);
    void setSprite(Texture2D sprite);

    // Initialization
    void initializeCamera(Tilemap tilemap);
};

#endif // PLAYER_H
