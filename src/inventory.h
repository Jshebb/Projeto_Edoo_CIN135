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

// Drop manager class to manage block drops
class DropManager {
public:
    DropManager(int maxDrops);

    void addDrop(const Item& item);
    void updateDrops(Vector2 playerPosition, float renderDistance);
    void renderDrops(Vector2 playerPosition, float renderDistance);
    void drawDrops();

private:
    std::deque<Item> drops;
    int maxDrops;
};

class Inventory {
    public:
    Inventory(int maxSlots);
    void addItem(const Item& item);
    void removeItem(const Item& item);
    void updateItem(const Item& item);
    void renderItems();

    private:
    std::vector<Item> items;
    int maxSlots;
};

#endif // INVENTORY_H