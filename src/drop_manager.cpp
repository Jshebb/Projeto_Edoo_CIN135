#include "drop_manager.h"
#include "player.h"
#include <cmath> // Necessário para std::floor


DropManager::DropManager(float gravity, float tileSize) 
    : gravity(gravity), tileSize(tileSize) {}

void DropManager::addDrop(Vector2 position, Texture2D sprite) {
    drops.push_back({position, {0, 0}, sprite}); // Inicia com velocidade zero
}

void DropManager::updateDrops() {
    for (auto& drop : drops) {
        drop.speed.y += gravity; // Aplica gravidade
        drop.position.y += drop.speed.y;

        // Alinha ao chão
        if (drop.position.y > tileSize * std::floor(drop.position.y / tileSize)) {
            drop.speed.y = 0; // Para o movimento
            drop.position.y = tileSize * std::floor(drop.position.y / tileSize);
        }
    }
}

void DropManager::collectDrops(Player& player, std::vector<Texture2D>& inventory) {
    for (auto it = drops.begin(); it != drops.end();) {
        Rectangle dropRect = {it->position.x, it->position.y, tileSize, tileSize};
        if (CheckCollisionCircleRec(player.getPosition(), 16.0f, dropRect)) {
            inventory.push_back(it->sprite); // Adiciona ao inventário
            it = drops.erase(it); // Remove o item
        } else {
            ++it;
        }
    }
}

void DropManager::drawDrops() const {
    for (const auto& drop : drops) {
        DrawTextureEx(drop.sprite, drop.position, 0.0f, 0.5f, WHITE);
    }
}
