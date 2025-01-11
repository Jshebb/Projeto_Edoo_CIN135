#pragma once
#include <raylib.h>

class Player
{
public:
    Player();
    void Update();
    void Draw() const;

    int getSpeedX() const;
    int getSpeedY() const;
    int getPosX() const;
    int getPosY() const;
    int getHeight() const;
    int getWidth() const;
    bool getGrounded() const;
    Rectangle getRec();
    Rectangle getBelowRec();

    void setPosX(int x);
    void setPosY(int y);
    void setSpeedX(int x);
    void setSpeedY(int y);
    void setHeight(int h);
    void setWidth(int w);
    void setGrounded(bool ground);

private:
    int posX;
    int posY;
    int width;
    int height;
    int speedX;
    float speedY;
    int maxSpeed;
    int friction;
    bool grounded;
    Rectangle playerRec;
    Rectangle belowRec;
    int static const gravity = 1;
};