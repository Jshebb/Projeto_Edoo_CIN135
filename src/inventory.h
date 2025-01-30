#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <deque>
#include <string>
#include "raylib.h"

//----------------------------------------------------------
// CLASSE ITEM
// Representa um item genérico no jogo com todas suas propriedades
//----------------------------------------------------------
class Item {
public:
    // Propriedades básicas
    std::string name;      // Nome exibível do item
    int id;                // ID único para identificação
    int quantity;          // Quantidade no stack
    Vector2 position;      // Posição atual no mundo 2D (x,y)
    Vector2 basePosition;  // Posição de referência para cálculos de animação
    Texture2D dropSprite;  // Textura quando o item está no chão

    // Construtor padrão (item vazio)
    Item();

    // Construtor completo
    Item(std::string name, int id, int quantity, Vector2 position, 
         Texture2D dropSprite, Vector2 basePosition);
};

//----------------------------------------------------------
// CLASSE INVENTORY
// Gerencia o sistema de inventário do jogador com slots fixos
//----------------------------------------------------------
class Inventory {
public:
    // Construtor: inicializa posição e aparência
    Inventory(float x, float y, Texture2D& sprite, float slotSize = 48.0f, float padding = 10.0f);
    
    // Métodos principais
    bool addItem(const Item& item);  // Adiciona item ao inventário (true se conseguiu)
    void removeItem(int slotIndex);  // Remove item de slot específico
    Item Update();                   // Atualiza estado (inputs e movimentação de itens)
    void Draw();                     // Renderiza o inventário na tela
    
    // Gerenciamento de seleção
    Item& getSelectedItem();         // Retorna referência ao item selecionado
    void clearSelectedItem();        // Remove/Reduz quantidade do item selecionado

private:
    // Dados internos
    std::vector<Item> items;         // Lista de itens armazenados
    static const int maxSlots = 8;   // Número fixo de slots
    Rectangle slotRects[maxSlots];   // Áreas clicáveis dos slots
    int selectedIndex = -1;          // Slot selecionado (-1 = nenhum)
    Texture2D sprite;                // Textura base dos slots
    Item grabbedItem;                // Item sendo arrastado
    bool hasGrabbedItem = false;     // Estado de arraste

    // Inicialização interna
    void initializeSlotRects(float x, float y, float slotSize, float padding);
};

//----------------------------------------------------------
// CLASSE DROPMANAGER
// Controla a lógica de itens dropados no ambiente do jogo
//----------------------------------------------------------
class DropManager {
public:
    DropManager(int maxDrops);  // Construtor com limite máximo de drops

    // Controle de drops
    void addDrop(const Item& item);                      // Adiciona novo drop
    void updateDrops(Vector2 playerPosition,             // Atualiza lógica (movimento, coleta)
                     float renderDistance, 
                     Inventory& inventory);
    void drawDrops();                                    // Renderiza todos os drops visíveis

private:
    std::deque<Item> drops;    // Lista de drops (deque para remoção eficiente)
    int maxDrops;              // Capacidade máxima simultânea
};

#endif // INVENTORY_H