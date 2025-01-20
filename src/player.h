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
    Camera2D camera;           // Camera
    
    void updateRectangles();   // Updates `playerRec` and `belowRec`

public:
    Player(Tilemap tilemap); // Constructor

    void Update(Tilemap& tilemap, vector<Texture2D>& inventory); // Atualiza o jogador e coleta drops
    void Draw() const; // Desenha o jogador na tela

    // Getters
    Vector2 getPosition() const; // Retorna a posição do jogador
    Vector2 getSpeed() const;    // Retorna a velocidade do jogador
    Rectangle getRec() const;    // Retorna o retângulo do jogador
    bool isGrounded() const;     // Retorna se o jogador está no chão
    Camera2D getCamera() const;  // Retorna a câmera associada ao jogador

    // Setters
    void setPosition(Vector2 newPosition); // Define a posição do jogador
    void setSpeed(Vector2 newSpeed);       // Define a velocidade do jogador
    void setSprite(Texture2D sprite);      // Define o sprite do jogador
};

#endif // PLAYER_H
