#include "inventory.h"
#include "player.h"
#include <cmath> // Necessário para std::floor
#include <algorithm> // Necessário para std::remove_if
#include "raymath.h"

// Default constructor
Item::Item(): 
name(""), 
id(-1), 
quantity(0),
position({0, 0}),
dropSprite({0}),
basePosition({0, 0}) // Add this for consistent floating
{ 
}

// Item constructor
Item::Item(std::string name, int id, int quantity, Vector2 position, Texture2D dropSprite, Vector2 basePosition)
    : name(name), id(id), quantity(quantity), position(position), dropSprite(dropSprite), basePosition(basePosition) {} // Add this for consistent floating) {}

// DropManager constructor
DropManager::DropManager(int maxDrops) : maxDrops(maxDrops) {}

// Adds a drop item to the manager
void DropManager::addDrop(const Item& item) {
    if (drops.size() >= maxDrops) {
        drops.pop_front();  // Remove the oldest drop if we exceed the max number of drops
    }
    drops.push_back(item);
}

void DropManager::updateDrops(Vector2 playerPosition, float renderDistance) {
    float time = GetTime();           // Time for sine wave calculation
    float floatingAmplitude = 4.0f;  // Vertical oscillation amplitude
    float floatingSpeed = 4.0f;      // Oscillation speed
    float triggerRadius = 128.0f;    // Radius for triggering movement toward the player
    float attractionSpeed = 40.0f;  // Speed for movement toward the player
    float vanishRadius = 40.0f;     // Radius for considering the drop "collected"

    drops.erase(std::remove_if(drops.begin(), drops.end(), [&](Item& drop) {
        // Floating effect using sine wave (always applied)
        drop.position.y = drop.basePosition.y + std::sin(time * floatingSpeed + drop.id) * floatingAmplitude;

        // Calculate distance to the player
        float distanceToPlayer = Vector2Distance(playerPosition, drop.position);

        // If within the trigger radius, move toward the player
        if (distanceToPlayer <= triggerRadius) {
            Vector2 direction = Vector2Subtract(playerPosition, drop.position); // Full direction (includes X and Y)
            direction = Vector2Normalize(direction); // Normalize the direction
            drop.position = Vector2Add(drop.position, Vector2Scale(direction, attractionSpeed * GetFrameTime()));

            // If the drop is within the vanish radius (centered on the player), remove it
            if (distanceToPlayer <= vanishRadius) {
                return true; // Remove the drop
            }
        }

        // Remove drops outside the render distance
        return distanceToPlayer > renderDistance;
    }), drops.end());
}
void DropManager::drawDrops() {
    for (const auto& drop : drops) {
        // Determine the source rectangle based on the drop's ID
        Rectangle sourceRect = {
            static_cast<float>((drop.id - 1) * 32), // X position in the spritesheet (32px per tile)
            0.0f,                                  // Y position in the spritesheet (row 0)
            32.0f,                                 // Width of each tile
            32.0f                                  // Height of each tile
        };

        // Draw the drop using the source and destination rectangles
        DrawTextureRec(drop.dropSprite, sourceRect, drop.position, WHITE);
    }
}

