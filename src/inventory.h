#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <deque>
#include <string>
#include "raylib.h"


// Classe de itens
class Item {
public:
    std::string name;
    int id;
    int quantity;
    Vector2 position;   // vector 2 par ordenado de posicao
    Vector2 basePosition;   // posicao base para animacao de flutuar
    Texture2D dropSprite;
    Item();

    Item(std::string name, int id, int quantity, Vector2 position, Texture2D dropSprite, Vector2 basePosition);
};


// classe do inventario do jogador
class Inventory {
    public:
    Inventory(float x, float y, Texture2D& sprite ,float slotSize = 48.0f, float padding = 10.0f);
    bool addItem(const Item& item);
    void removeItem(int slotIndex);
    Item Update();
    void Draw();
    //Item selecionado
    Item& getSelectedItem();
    // Limpa item selecionado
    void clearSelectedItem();

    private:
    std::vector<Item> items;    // Vector1 para uma lista de itens
    static const int maxSlots = 8;
    Rectangle slotRects[maxSlots];  // recs de colisao do inventario
    int selectedIndex = -1;
    Texture2D sprite;
    Item grabbedItem;
    bool hasGrabbedItem = false; // Se o jogador segura um item

    void initializeSlotRects(float x, float y, float slotSize, float padding); // inicializa os slots do inventario
};

// Drop manager, classe para manejar animacao e drops de itens no geral
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