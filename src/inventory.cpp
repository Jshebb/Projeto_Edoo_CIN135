#include "inventory.h"
#include "player.h"
#include <cmath> // Necessário para std::floor
#include <algorithm> // Necessário para std::remove_if
#include "raymath.h"
#include <iostream> 

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
    // Find first empty slot or stackable slot
    for (int i = 0; i < maxSlots; i++) {
        if (items[i].id == -1){
            items[i] = item;
            std::cout << "Item added: " << item.name.c_str() << items[i].id << std::endl;
            return true;
        }
        else if (items[i].id == item.id && items[i].quantity < 99) {
            items[i].quantity += 1;
            std::cout << "Item added: " << item.name.c_str() << items[i].id << std::endl;
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

void Inventory::Update() {
    // Handle mouse interactions
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
            
        // Check slot selection
        for (int i = 0; i < maxSlots; i++) {
            if (CheckCollisionPointRec(mousePos, slotRects[i])) {
                    selectedIndex = i;
                    break;
            }
        }
    }
}

void Inventory::Draw() {
    // Draw slot backgrounds using the inventorySprite
    for (int i = 0; i < maxSlots; i++) {
        // Calculate source rectangle for slot background sprite
        Rectangle slotSourceRect = { 0.0f, 0.0f, 64.0f, 64.0f }; // Slot sprite size (64x64)
        
        // Determine destination rectangle for slot background
        Rectangle slotDestRect = {
            slotRects[i].x,         // X position
            slotRects[i].y,         // Y position
            64.0f,                  // Width of the slot
            64.0f                   // Height of the slot
        };

        // Highlight selected slot by modifying the tint
        Color slotTint = (i == selectedIndex) 
            ? YELLOW
            : WHITE;

        // Draw the slot sprite
        DrawTexturePro(
            sprite,            // Texture for slot background
            slotSourceRect,    // Source rectangle
            slotDestRect,      // Destination rectangle
            { 0.0f, 0.0f },    // Origin (top-left corner)
            0.0f,              // Rotation
            slotTint           // Tint color
        );

        std::cout << "Id " << items[i].id << std::endl;

        // Draw item if it exists
        if (items[i].id > 0) {
            // Calculate the source rectangle based on the item's ID (32x32 spritesheet)
            Rectangle itemSourceRect = {
                static_cast<float>((items[i].id - 1) * 32), // X position in the spritesheet (32px per tile)
                0.0f,                                       // Y position in the spritesheet (row 0)
                32.0f,                                      // Width of the item sprite
                32.0f                                       // Height of the item sprite
            };

            // Define the destination rectangle to scale the item to 64x64
            Rectangle itemDestRect = {
                slotRects[i].x,       // X position in the slot
                slotRects[i].y,       // Y position in the slot
                64.0f,                // Stretch width to 64px
                64.0f                 // Stretch height to 64px
            };

            // Draw the item's sprite stretched to 64x64
            DrawTexturePro(
                items[i].dropSprite,  // Texture (spritesheet)
                itemSourceRect,       // Source rectangle
                itemDestRect,         // Destination rectangle (scaled to 64x64)
                { 0.0f, 0.0f },       // Origin (top-left corner)
                0.0f,                 // Rotation
                WHITE                 // Tint color
            );

            // Draw the item's quantity
            DrawText(
                TextFormat("x%d", items[i].quantity), 
                slotRects[i].x + 10, 
                slotRects[i].y + 40, 
                20, 
                WHITE
            );
        }
    }
}

// Adds a drop item to the manager
void DropManager::addDrop(const Item& item) {
    if (drops.size() >= maxDrops) {
        drops.pop_front();  // Remove the oldest drop if we exceed the max number of drops
    }
    drops.push_back(item);
}

void DropManager::updateDrops(Vector2 playerPosition, float renderDistance, Inventory& inventory) {
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
                    inventory.addItem(drop);
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



