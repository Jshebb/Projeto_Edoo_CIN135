#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <deque>
#include <string>
#include "raylib.h"

// Class to represent an item that can be dropped
class Item {
public:
    std::string name;
    int id;
    int quantity;
    Vector2 position;
    Vector2 basePosition;
    Texture2D dropSprite;
    Item();

    Item(std::string name, int id, int quantity, Vector2 position, Texture2D dropSprite, Vector2 basePosition);
};

class Inventory {
    public:
    Inventory(float x, float y, Texture2D& sprite ,float slotSize = 48.0f, float padding = 10.0f);
    bool addItem(const Item& item);
    void removeItem(int slotIndex);
    void Update();
    void Draw();

    private:
    std::vector<Item> items;
    static const int maxSlots = 8;
    Rectangle slotRects[maxSlots];
    int selectedIndex = -1;
    Texture2D sprite;

    void initializeSlotRects(float x, float y, float slotSize, float padding);
};

// Drop manager class to manage block drops
class DropManager {
public:
    DropManager(int maxDropsm);

    void addDrop(const Item& item);
    void updateDrops(Vector2 playerPosition, float renderDistance, Inventory& inventory);
    void renderDrops(Vector2 playerPosition, float renderDistance);
    void drawDrops();

private:
    std::deque<Item> drops;
    int maxDrops;
};

#endif // INVENTORY_H