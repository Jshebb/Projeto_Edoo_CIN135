#include "ball.h"
#include <raylib.h>

Ball::Ball() 
    : posX(100)
    , posY(100)
    , speedX(5)
    , speedY(5)
    , radius(15) 
    , friction(0.1)
{
}

void Ball::Update() 
{
   if (IsKeyDown(KEY_D))
    {
        posX += speedX;
    }
    if (IsKeyDown(KEY_A))
    {
        posX -= speedX;
    }
    if (IsKeyDown(KEY_W))
    {
        posY -= speedY;
    }
    if (IsKeyDown(KEY_S))
    {
        posY += speedY;
    }
    else{

        if(speedX > 0)
        {
            speedX -= friction;
        }
        if(speedX < 0)
        {
            speedX += friction;
        }
        if(speedY > 0)
        {
            speedY -= friction;
        }
        if(speedY < 0)
        {
            speedY += friction;
        }
    }
}

void Ball::Draw() const
{
    DrawCircle(posX, posY, radius, WHITE);
}