#include "inventory.h"
#include "player.h"
#include <cmath> // Necessário para std::floor
#include <algorithm> 
#include "raymath.h"
#include <iostream> 


/// --- CLASSE ITEM ---
// Representa um item no jogo, com propriedades como ID, nome, quantidade e posição.
// Responsável por armazenar informações visuais (sprite) e lógicas dos itens.
// --------------------------------------------------
    // Construtor padrão: inicializa um item "vazio" com valores inválidos/resetados
    Item::Item(): 
        id(-1),           // ID -1 indica item inválido/vazio
        name(""),         // Nome vazio
        quantity(0),      // Quantidade zero
        position({0, 0}), // Posição inicial (x, y)
        dropSprite({0}),  // Textura padrão vazia
        basePosition({0, 0}) // Posição base para cálculos de animação
    { 
    }
    // Construtor parametrizado: cria um item específico com todos os atributos
    // Parâmetros:
    // - name: Nome do item (ex: "Poção de Cura")
    // - id: Identificador único (ex: 123)
    // - quantity: Quantidade inicial (ex: 3)
    // - position: Posição inicial no mundo 2D (x, y)
    // - dropSprite: Textura do item quando dropado no chão
    // - basePosition: Posição de referência para animações
    Item::Item(std::string name, int id, int quantity, Vector2 position, Texture2D dropSprite, Vector2 basePosition)
        : id(id), 
          name(name),
          quantity(quantity),
          position(position),
          dropSprite(dropSprite),
          basePosition(basePosition) 
    {};

// --- CLASSE DROPMANAGER ---
// Gerencia a criação, atualização e remoção de itens dropados no chão.
// Controla efeitos visuais, interação com o jogador e limite máximo de drops.
// --------------------------------------------------
    // Construtor: define a capacidade máxima de drops simultâneos
    // Parâmetro:
    // - maxDrops: Quantidade máxima de itens que podem existir no chão ao mesmo tempo
    // (Drops excedentes são removidos automaticamente)
    DropManager::DropManager(int maxDrops) : maxDrops(maxDrops) {}



// Inicializacao de slots do inventario
void Inventory::initializeSlotRects(float x, float y, float slotSize, float padding)
{
    for (int i = 0; i < maxSlots; i++) {
        slotRects[i] = {
            x + i * (slotSize + padding), 
            y, 
            slotSize, 
            slotSize
        };
    }
}

Inventory::Inventory( float x, float y,  Texture2D& sprite, float slotSize, float padding):
sprite(sprite)
{
    items.resize(maxSlots);
    initializeSlotRects(x, y, slotSize, padding);
}

bool Inventory::addItem(const Item& item) {
    // Encontra o primeiro slot vazio do inventario
    for (int i = 0; i < maxSlots; i++) {
        if (items[i].id == -1){
            items[i] = item;
            return true;
        }
        else if (items[i].id == item.id && items[i].quantity < 99) {
            items[i].quantity += 1;
            return true;
        }
    }
    return false;
}

void Inventory::removeItem(int slotIndex) {
    if (slotIndex >= 0 && slotIndex < maxSlots) {
            items[slotIndex] = {};
    }
}



void Inventory::Draw() {
    // Renderiza slots e itens
    for (int i = 0; i < maxSlots; i++) {
        Rectangle slotSourceRect = {0.0f, 0.0f, 64.0f, 64.0f};
        Rectangle slotDestRect = {slotRects[i].x, slotRects[i].y, 64.0f, 64.0f};
        Color slotTint = (i == selectedIndex) ? YELLOW : WHITE;

        DrawTexturePro(sprite, slotSourceRect, slotDestRect, {0.0f, 0.0f}, 0.0f, slotTint);

        if (items[i].id > 0) {
            Rectangle itemSourceRect = {static_cast<float>((items[i].id - 1) * 32), 0.0f, 32.0f, 32.0f};
            Rectangle itemDestRect = {slotRects[i].x, slotRects[i].y, 64.0f, 64.0f};

            DrawTexturePro(items[i].dropSprite, itemSourceRect, itemDestRect, {0.0f, 0.0f}, 0.0f, WHITE);
            DrawText(TextFormat("x%d", items[i].quantity), slotRects[i].x + 10, slotRects[i].y + 40, 20, WHITE);
        }
    }

    // Renderiza itens selecionados pelo mouse do jogador
    if (hasGrabbedItem) {
        Vector2 mousePos = GetMousePosition();
        Rectangle grabbedSourceRect = {static_cast<float>((grabbedItem.id - 1) * 32), 0.0f, 32.0f, 32.0f};
        Rectangle grabbedDestRect = {mousePos.x - 32.0f, mousePos.y - 32.0f, 64.0f, 64.0f};

        DrawTexturePro(grabbedItem.dropSprite, grabbedSourceRect, grabbedDestRect, {0.0f, 0.0f}, 0.0f, WHITE);
        DrawText(TextFormat("x%d", grabbedItem.quantity), mousePos.x + 10, mousePos.y + 10, 20, WHITE);
    }
}

// Adiciona novos itens "jogados" dropados ao dropmanager
void DropManager::addDrop(const Item& item) {
    if (drops.size() >= maxDrops) {
        drops.pop_front();  // Remove the oldest drop if we exceed the max number of drops
    }
    drops.push_back(item);
}

// Aplicacao de animacoes de flutuar e coleta ao colidir com o jogador
void DropManager::updateDrops(Vector2 playerPosition, float renderDistance, Inventory& inventory) {
    float time = GetTime();           // tempo para onda senoidal
    float floatingAmplitude = 4.0f;  // Amplitude de oscilação vertical
    float floatingSpeed = 4.0f;      // Velocidade de oscilação
    float triggerRadius = 128.0f;    // Raio para acionar movimento em direção ao jogador
    float attractionSpeed = 40.0f;   // Velocidade de movimento em direção ao jogador
    float vanishRadius = 40.0f;      // Raio para considerar o drop "coletado"

    drops.erase(std::remove_if(drops.begin(), drops.end(), [&](Item& drop) {
        // Efeito de flutuação usando onda senoidal (sempre aplicado)
        drop.position.y = drop.basePosition.y + std::sin(time * floatingSpeed + drop.id) * floatingAmplitude;

        // Calcular distância até o jogador
        float distanceToPlayer = Vector2Distance(playerPosition, drop.position);

        // Se dentro do raio de ativação, mover em direção ao jogador
        if (distanceToPlayer <= triggerRadius) {
            Vector2 direction = Vector2Subtract(playerPosition, drop.position); // Direção completa (inclui X e Y)
            direction = Vector2Normalize(direction); // Normalizar a direção
            drop.position = Vector2Add(drop.position, Vector2Scale(direction, attractionSpeed * GetFrameTime()));

            // Se o drop estiver dentro do raio de desaparecimento (centrado no jogador), removê-lo
            if (distanceToPlayer <= vanishRadius) {
                inventory.addItem(drop);
                return true; // Remover o drop
            }
        }

        // Remover drops fora da distância de renderização
        return distanceToPlayer > renderDistance;
    }), drops.end());
}

// Renderizacao com base em uma spritesheet e ids
void DropManager::drawDrops() {
    for (const auto& drop : drops) {
        // Determina o retângulo de origem com base no ID do drop
        Rectangle sourceRect = {
            static_cast<float>((drop.id - 1) * 32), // Posição X na spritesheet (32px por tile)
            0.0f,                                  // Posição Y na spritesheet (linha 0)
            32.0f,                                 // Largura de cada tile
            32.0f                                  // Altura de cada tile
        };

        // Desenha o drop usando os retângulos de origem e destino
        DrawTextureRec(drop.dropSprite, sourceRect, drop.position, WHITE);
    }
}

// Atualiza os itens no inventário e os estados dos slots
Item Inventory::Update() {
    Vector2 mousePos = GetMousePosition();
    bool clickedInsideInventory = false;

    // Valor de retorno padrão
    Item defaultItem = {}; // Cria um item vazio/padrão para retornar se nada for dropado

    // Manipula a seleção por teclas numéricas (teclas 1 a 8)
    for (int key = KEY_ONE; key <= KEY_EIGHT; ++key) {
        if (IsKeyPressed(key)) {
            selectedIndex = key - KEY_ONE; // Converte a tecla para o índice do slot (baseado em 0)
            break;
        }
    }

    // Manipula cliques do mouse
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < maxSlots; i++) {
            if (CheckCollisionPointRec(mousePos, slotRects[i])) {
                clickedInsideInventory = true; // Mouse clicou dentro do inventário
                
                // Seleciona o item ao clicar em um slot
                selectedIndex = i;
                
                // Se estiver segurando um item, coloca-o no slot selecionado
                if (hasGrabbedItem) {
                    if (items[i].id == -1) { // Slot vazio, coloca o item segurado
                        items[i] = grabbedItem;
                        grabbedItem = {}; // Limpa o item segurado
                        hasGrabbedItem = false;
                    } else if (items[i].id == grabbedItem.id && items[i].quantity < 99) {
                        // Empilha o item segurado se possível
                        int space = 99 - items[i].quantity;
                        int transfer = std::min(space, grabbedItem.quantity);
                        items[i].quantity += transfer;
                        grabbedItem.quantity -= transfer;

                        if (grabbedItem.quantity <= 0) {
                            grabbedItem = {}; // Limpa o item segurado
                            hasGrabbedItem = false;
                        }
                    }
                } else {
                    // Se não estiver segurando um item, pega o item do slot selecionado
                    if (items[i].id != -1) {
                        grabbedItem = items[i];
                        hasGrabbedItem = true;
                        items[i] = {}; // Limpa o slot
                    }
                }
                break; // Sai do loop após processar o clique
            }
        }

        // Solta o item se clicado fora do inventário enquanto segura um item
        if (!clickedInsideInventory && hasGrabbedItem) {
            grabbedItem.position = mousePos; // Solta o item na posição do mouse
            hasGrabbedItem = false;         // Solta o item
            Item out = grabbedItem;
            grabbedItem = {};               // Limpa o item segurado
            return out;                     // Retorna o item solto
        }
    }

    // Manipula a rolagem do mouse
    float mouseWheel = GetMouseWheelMove(); // Obtém o movimento da roda do mouse
    if (mouseWheel != 0) {
        selectedIndex += (mouseWheel > 0) ? -1 : 1; // Rolagem para cima diminui o índice, rolagem para baixo aumenta

        // Envolve o índice se estiver fora dos limites
        if (selectedIndex < 0) {
            selectedIndex = maxSlots - 1; // Envolve para o último slot
        } else if (selectedIndex >= maxSlots) {
            selectedIndex = 0; // Envolve para o primeiro slot
        }
    }

    // Retorna item padrão se nenhum item foi solto
    return defaultItem;
}

// Getter para item selecionado
Item& Inventory::getSelectedItem() {
    if (selectedIndex >= 0 && selectedIndex < items.size()) {
        return items[selectedIndex];
    }
    // Retorna uma referência a um item "vazio" padrão estático se nenhuma seleção válida
    static Item emptyItem("empty", -1, 0, {0, 0}, Texture2D{}, {0, 0});
    return emptyItem;
}

// Limpa a selecao de itens
void Inventory::clearSelectedItem() {
    if (selectedIndex >= 0 && selectedIndex < items.size()) {
        if (items[selectedIndex].quantity > 1) {
            // Decrease the quantity if there's more than one item
            items[selectedIndex].quantity -= 1;
        } else {
            // Remove the item entirely if only one remains
            items[selectedIndex] = {}; // Replace with a default empty item
        }
    }
}

