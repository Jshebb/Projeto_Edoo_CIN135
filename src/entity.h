#ifndef TILEMAP_H
#define TILEMAP_H

#include <raylib.h>
#include <vector>
using namespace std;

class Tile {
private:
    Rectangle rect;   // The tile's bounding box
    bool solid;       // Whether the tile is solid (for collision)
    Color color;      // The color to draw the tile (debug only)
    Texture2D texture; // The texture to draw the tile

public:
    Tile(float x, float y, float size, bool solid, Color color);

    void Draw() const;

    void setTexture(Texture2D texture);

    Rectangle getRect() const;
    bool isSolid() const;
};

class Tilemap {
private:
    vector<vector<Tile>> tiles;
    int rows, cols;
    float tileSize;
    Texture2D texture;

public:
    Tilemap(int rows, int cols, float tileSize);

    void setTile(int x, int y, bool solid, Color color);

    void setTexture(Texture2D newTexture);

    void Draw() const;

    const Tile& getTileAt(float x, float y) const;

    float getTileSize() const;

    bool checkCollision(const Rectangle& rect) const;

    int getRows() const;

    int getCols() const;
};

#endif // TILEMAP_H
