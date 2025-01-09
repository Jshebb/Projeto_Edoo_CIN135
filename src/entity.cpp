#include <raylib.h>
#include "entity.h"

Block::Block()  //default constructor
    : width(50)
    , height(50)
    , posX(100)
    , posY(100)
{
}

Block::Block(int w, int h, int posX, int posY)  //constructor with parameters
    : width(w)
    , height(h)
    , posX(posX)
    , posY(posY)
{
}

//dimensions of the block getters
int Block::getHeight() const
{
    return height;
}

int Block::getWidth() const
{
    return width;
}

int Block::getPosX() const
{
    return posX;
}

int Block::getPosY() const
{
    return posY;
}

//setters
void Block::setHeight(int h)
{
    height = h;
}

void Block::setWidth(int w)
{
    width = w;
}

void Block::setPosX(int x)
{
    posX = x;
}

void Block::setPosY(int y)
{
    posY = y;
}

void Block::Draw() const
{
    DrawRectangle(posX, posY, width, height, RED);
}