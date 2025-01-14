#include "player.h"
#include <raylib.h>

Player::Player() 
    : posX(100)
    , posY(100)
    , speedX(0)
    , speedY(0)
    , maxSpeed(10)
    , width(50)
    , height(50)
    , friction(1)
    , grounded (false)
    , worldSize({2000, 2000}) // Definindo o tamanho do mundo (2000x2000) by LM
{
    // Inicializar a câmera by LM
    camera.target = (Vector2){ posX + width / 2, posY + height / 2 };
    camera.offset = (Vector2){ 400.0f, 300.0f }; // Centro da tela
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    //initialize the player rectangle
    playerRec.x = posX;
    playerRec.y = posY;
    playerRec.width = width;
    playerRec.height = height;

    //initialize the rectangle below the player
    belowRec.x = playerRec.x;
    belowRec.y = playerRec.y + height;
    belowRec.width = width;
    belowRec.height = 5;
}

void Player::Update() 
{
   if (IsKeyDown(KEY_D))
    {
        if (speedX < maxSpeed)
        {
            speedX += 1;
        }
    }
    else if (IsKeyDown(KEY_A))
    {
        if (speedX > ( -1 * maxSpeed))
        {
            speedX -= 1;
        }
    }
    else{
        if (speedX > 0)
        {
            speedX -= friction;
        }
        else if (speedX < 0)
        {
            speedX += friction;
        }
    }

    if (speedX > maxSpeed)
    {
        speedX = maxSpeed;
    }
    else if (speedX < ( -1 * maxSpeed))
    {
        speedX = ( -1 * maxSpeed);
    }

    posX += speedX;

    // Jumping
    if (IsKeyPressed(KEY_SPACE) && grounded)
    {
        speedY = -15;
        grounded = false;
    }

    // Apply gravity
    if (!grounded)
    {
        if (speedY < maxSpeed)
        speedY += 0.5; // Gravity strength
    }
    else
    {
        speedY = 0;
    }

    posY += speedY;

    // Update the belowRec position so it moves with the player
    belowRec.x = posX;
    belowRec.y = posY + height;

    // Atualizar a câmera para seguir o jogador by LM
    camera.target = (Vector2){ posX + width / 2, posY + height / 2 };

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
// by LM
Camera2D Player::getCamera() const
{
    return camera;
}

void Player::Draw() const
{
    DrawRectangle(posX, posY, width, height, BLUE);
    DrawRectangle(belowRec.x, belowRec.y, belowRec.width, belowRec.height, RED);
}

//getters
int Player::getSpeedX() const
{
    return speedX;
}

int Player::getSpeedY() const
{
    return speedY;
}

int Player::getPosX() const
{
    return posX;
}

int Player::getPosY() const
{
    return posY;
}

int Player::getHeight() const
{
    return height;
}

int Player::getWidth() const
{
    return width;
}

Rectangle Player::getRec()
{
    return playerRec;
}

Rectangle Player::getBelowRec()
{
    return belowRec;
}

bool Player::getGrounded() const
{
    return grounded;
}

//setters
void Player::setPosX(int x)
{
    posX = x;
}

void Player::setPosY(int y)
{
    posY = y;
}

void Player::setSpeedX(int x)
{
    speedX = x;
}

void Player::setSpeedY(int y)
{
    speedY = y;
}

void Player::setGrounded(bool ground)
{
    grounded = ground;
}

