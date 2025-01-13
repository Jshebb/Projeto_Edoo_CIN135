#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "entity.h"

class Player {
private:
    Vector2 position;          // Player position
    Vector2 speed;             // Player speed
    int width, height;         // Dimensions of the player
    float maxSpeed;            // Maximum speed
    float gravity;             // Gravity strength
    float friction;            // Horizontal friction
    bool grounded;             // Whether the player is on the ground
    Rectangle playerRec;       // Rectangle representing the player's bounds
    Rectangle belowRec;        // Rectangle below the player for ground collision
    Texture2D playerSprite;    // Player sprite     

    void updateRectangles();   // Updates `playerRec` and `belowRec`

public:
    Player();                  // Constructor

    void Update(const Tilemap& tilemap);  // Updates player movement and collisions
    void Draw() const;                    // Draws the player on the screen

    // Getters
    Vector2 getPosition() const;
    Vector2 getSpeed() const;
    Rectangle getRec() const;
    bool isGrounded() const;

    // Setters
    void setPosition(Vector2 newPosition);
    void setSpeed(Vector2 newSpeed);
    void setSprite(Texture2D sprite);

};

#endif // PLAYER_H