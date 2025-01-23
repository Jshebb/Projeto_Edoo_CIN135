#ifndef TILEMAP_H
#define TILEMAP_H

#include <raylib.h>
#include <vector>
#include "inventory.h"
using namespace std;

class Tile {
private:
    Rectangle rect;   // The tile's bounding box
    bool solid;       // Whether the tile is solid (for collision)
    Color color;      // The color to draw the tile (debug only)
    Texture2D texture; // The texture to draw the tile
    int id;    // The tile's ID

public:
    Tile(float x, float y, float size, bool solid, Color color, int id = 0);

    void Draw(Vector2 position) const;

    void setTexture(Texture2D texture);

    Rectangle getRect() const;
    bool isSolid() const;
    void SetID(int ID);
    void SetSolid(bool solid);
    int getID();
};

class Tilemap {
private:
    vector<vector<Tile>> tiles;
    int rows, cols;
    float tileSize;
    Texture2D texture;
    DropManager dropManager;
    Inventory inventory;
    

public:
    Tilemap(int rows, int cols, float tileSize, DropManager dropManager, Inventory inventory);

    void setTile(int x, int y, bool solid, Color color, int id);

    void setTexture(Texture2D SpriteSheet);

    void Draw(Camera2D camera, int tileSize) const;

    const Tile& getTileAt(float x, float y) const;

    float getTileSize() const;

    bool checkCollision(const Rectangle& rect) const;

    int getRows() const;

    int getCols() const;

    void generateWorld();

    void generateCaves(int iterations, int seed);

    void TilePlacement(Camera2D const &camera, int tileSize, Vector2 PlayerPos, Texture2D SpriteSheetDrops, Texture2D SpriteSheetBlocks);

    int getGroundLevel(int x);

    DropManager& getDropManager();

    void DrawInventory();

    void UpdateInventory();
};

#endif // TILEMAP_H
