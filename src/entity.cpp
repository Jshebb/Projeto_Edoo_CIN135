#include <raylib.h>
#include "entity.h"

Block::Block()  //default constructor
    : width(50)
    , height(50)
    , posX(100)
    , posY(100)
{
     //initialize the block rectangle
    blockRec.x = posX;
    blockRec.y = posY;
    blockRec.width = width;
    blockRec.height = height;

    //initialize the rectangle top
    topRec.x = posX;
    topRec.y = height;
    topRec.width = width;
    topRec.height = 5;
}

Block::Block(int w, int h, int posX, int posY)  //constructor with parameters
    : width(w)
    , height(h)
    , posX(posX)
    , posY(posY)
{
    //initialize the block rectangle
    blockRec.x = posX;
    blockRec.y = posY;
    blockRec.width = width;
    blockRec.height = height;

    //initialize the rectangle top
    topRec.x = blockRec.x;
    topRec.y = blockRec.y;
    topRec.width = width;
    topRec.height = 5;
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

//get the rectangle of the block

Rectangle Block::getRec()
{
    return blockRec;
}

Rectangle Block::getTopRec()
{
    return topRec;
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
    DrawRectangle(topRec.x, topRec.y, width, topRec.height, BLUE);
}