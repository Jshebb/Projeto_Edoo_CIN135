#include "inventory.h"
#include "player.h"
#include <cmath> // Necessário para std::floor
#include <algorithm> 
#include "raymath.h"
#include <iostream> 


// Construtor dos itens default
Item::Item(): 
name(""), 
id(-1), 
quantity(0),
position({0, 0}),
dropSprite({0}),
basePosition({0, 0}) // usado na animacao de floating
{ 
}


// Construtor parametrizado de itens
Item::Item(std::string name, int id, int quantity, Vector2 position, Texture2D dropSprite, Vector2 basePosition)
    : name(name), id(id), quantity(quantity), position(position), dropSprite(dropSprite), basePosition(basePosition) {};

// DropManager constructor
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
    // Find first empty slot or stackable slot
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
    // Draw the slots and items
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

    // Draw the grabbed item near the cursor if it exists
    if (hasGrabbedItem) {
        Vector2 mousePos = GetMousePosition();
        Rectangle grabbedSourceRect = {static_cast<float>((grabbedItem.id - 1) * 32), 0.0f, 32.0f, 32.0f};
        Rectangle grabbedDestRect = {mousePos.x - 32.0f, mousePos.y - 32.0f, 64.0f, 64.0f};

        DrawTexturePro(grabbedItem.dropSprite, grabbedSourceRect, grabbedDestRect, {0.0f, 0.0f}, 0.0f, WHITE);
        DrawText(TextFormat("x%d", grabbedItem.quantity), mousePos.x + 10, mousePos.y + 10, 20, WHITE);
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

Item Inventory::Update() {
    Vector2 mousePos = GetMousePosition();
    bool clickedInsideInventory = false;

    // Default return value
    Item defaultItem = {}; // Create an empty/default item to return if nothing is dropped

    // Handle number key selection (keys 1 to 8)
    for (int key = KEY_ONE; key <= KEY_EIGHT; ++key) {
        if (IsKeyPressed(key)) {
            selectedIndex = key - KEY_ONE; // Convert key to slot index (0-based)
            printf("Slot %d selected via key press.\n", selectedIndex + 1); // Debug message
            break;
        }
    }

    // Handle mouse clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < maxSlots; i++) {
            if (CheckCollisionPointRec(mousePos, slotRects[i])) {
                clickedInsideInventory = true; // Mouse clicked inside inventory
                
                // Select the item when clicking a slot
                selectedIndex = i;
                
                // If holding an item, place it in the selected slot
                if (hasGrabbedItem) {
                    if (items[i].id == -1) { // Empty slot, place the grabbed item
                        items[i] = grabbedItem;
                        grabbedItem = {}; // Clear the grabbed item
                        hasGrabbedItem = false;
                    } else if (items[i].id == grabbedItem.id && items[i].quantity < 99) {
                        // Stack the grabbed item if possible
                        int space = 99 - items[i].quantity;
                        int transfer = std::min(space, grabbedItem.quantity);
                        items[i].quantity += transfer;
                        grabbedItem.quantity -= transfer;

                        if (grabbedItem.quantity <= 0) {
                            grabbedItem = {}; // Clear the grabbed item
                            hasGrabbedItem = false;
                        }
                    }
                } else {
                    // If not holding an item, grab the item from the selected slot
                    if (items[i].id != -1) {
                        grabbedItem = items[i];
                        hasGrabbedItem = true;
                        items[i] = {}; // Clear the slot
                    }
                }
                break; // Exit the loop after processing the click
            }
        }

        // Drop item if clicked outside inventory while holding an item
        if (!clickedInsideInventory && hasGrabbedItem) {
            grabbedItem.position = mousePos; // Drop the item at the mouse position
            hasGrabbedItem = false;         // Release the item
            Item out = grabbedItem;
            grabbedItem = {};               // Clear the grabbed item
            return out;                     // Return the dropped item
        }
    }

    // Handle mouse wheel scrolling
    float mouseWheel = GetMouseWheelMove(); // Get mouse wheel movement
    if (mouseWheel != 0) {
        selectedIndex += (mouseWheel > 0) ? -1 : 1; // Scroll up decreases index, scroll down increases it

        // Wrap around if selectedIndex is out of bounds
        if (selectedIndex < 0) {
            selectedIndex = maxSlots - 1; // Wrap around to the last slot
        } else if (selectedIndex >= maxSlots) {
            selectedIndex = 0; // Wrap around to the first slot
        }
    }

    // Return default item if no item was dropped
    return defaultItem;
}

Item& Inventory::getSelectedItem() {
    if (selectedIndex >= 0 && selectedIndex < items.size()) {
        return items[selectedIndex];
    }
    // Return a reference to a default static "empty" item if no valid selection
    static Item emptyItem("empty", -1, 0, {0, 0}, Texture2D{}, {0, 0});
    return emptyItem;
}

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

