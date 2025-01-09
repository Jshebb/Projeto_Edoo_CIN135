#pragma once

class Ball
{
public:
    Ball();
    void Update();
    void Draw() const;

private:
    int posX;
    int posY;
    int speedX;
    int speedY;
    int radius;
    int friction;
};