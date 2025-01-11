#ifndef BLOCK_H
#define BLOCK_H

class Block{
    public:

        Block(); //default constructor

        Block(int w, int h, int posX, int posY);    //constructor with parameters

        //dimensions of the block
        void setWidth(int w);
        void setHeight(int h);
        int getWidth() const;
        int getHeight() const;

        //position of the block
        void setPosX(int posX);
        void setPosY(int posY);
        int getPosX() const;
        int getPosY() const;

        //draw the block
        void Draw() const;

        //get the rectangle of the block
        Rectangle getRec();
        Rectangle getTopRec();

    private:
        int width;
        int height;
        int posX;
        int posY;
        Rectangle blockRec;
        Rectangle topRec;
};
#endif // BLOCK_H