#include "tilemap.h"
#include <cmath>
#include <random>
#include <raylib.h>
#include "player.h"
#include "raymath.h"
#include "inventory.h"
#include <iostream>
#include "SimplexNoise.h"


// Tile Class Implementations
Tile::Tile(float x, float y, float size, bool solid, Color color, int id) 
    : rect({x, y, size, size})
    , solid(solid)
    , color(color) 
    , texture({0})
    , id (id)
{
}

// Draw function to render the tile
void Tile::Draw(Vector2 position) const{
        // Calculate the source rectangle based on the tile ID
        Rectangle sourceRect = {
            static_cast<float>((id - 1) * 32), // Horizontal offset (ID-1 because IDs start from 1)
            0.0f,                             // Vertical offset (row 0 in the sprite sheet)
            32.0f,                            // Width of the tile
            32.0f                             // Height of the tile
        };
        if (id == 0){
            DrawTextureRec({0}, sourceRect, position, WHITE);
        }
        else{
            // Draw the tile using the source rectangle
            DrawTextureRec(texture, sourceRect, position, WHITE);
        }
}


Rectangle Tile::getRect() const { 
    return rect; 
}

bool Tile::isSolid() const { 
    return solid; 
}

void Tile::setTexture(Texture2D texture) {
    this->texture = texture;
}

int Tile::getID() {
    return id;
}

void Tile::SetID(int ID) {
    id = ID;
}

void Tile::SetSolid(bool solid) {
    this->solid = solid;
}


// Tilemap Class Implementations
Tilemap::Tilemap(int rows, int cols, float tileSize, DropManager dropManager)
    : rows(rows), cols(cols), tileSize(tileSize), dropManager(dropManager)
    {
    // Initialize the map with default non-solid tiles
    for (int y = 0; y < rows; y++) {
        vector<Tile> row;
        for (int x = 0; x < cols; x++) {
            row.emplace_back(x * tileSize, y * tileSize, tileSize, false, DARKGRAY);
        }
        tiles.push_back(row);
    }
    
}

void Tilemap::setTile(int x, int y, bool solid, Color color, int id) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        tiles[y][x] = Tile(x * tileSize, y * tileSize, tileSize, solid, color, id);
    }
}

void Tilemap::Draw(Camera2D camera, int tileSize) const {
    // Calculate the visible tile range based on camera position and screen size
    int startX = static_cast<int>(camera.target.x - GetScreenWidth() / 2) / tileSize;
    int endX = static_cast<int>(camera.target.x + GetScreenWidth() / 2) / tileSize;
    int startY = static_cast<int>(camera.target.y - GetScreenHeight() / 2) / tileSize;
    int endY = static_cast<int>(camera.target.y + GetScreenHeight() / 2) / tileSize;

    // Clamp the values to make sure they stay within the bounds of the world
    startX = std::max(0, startX);
    endX = std::min(cols - 1, endX);
    startY = std::max(0, startY);
    endY = std::min(rows - 1, endY);

    // Draw only the visible tiles
    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            tiles[y][x].Draw({tiles[y][x].getRect().x, tiles[y][x].getRect().y});
        }
    }
}


const Tile& Tilemap::getTileAt(float x, float y) const {
    int col = static_cast<int>(std::round((x - tileSize / 2) / tileSize));  // Adjust for tile center
    int row = static_cast<int>(std::round((y - tileSize / 2) / tileSize));  // Adjust for tile center


    // Clamp col and row within bounds of the tile map
    col = std::max(0, std::min(col, cols - 1));  // Clamp col to [0, cols - 1]
    row = std::max(0, std::min(row, rows - 1));  // Clamp row to [0, rows - 1]

    return tiles[row][col];
}


float Tilemap::getTileSize() const { 
    return tileSize; 
}

bool Tilemap::checkCollision(const Rectangle& rect) const {
    // Set the size of the collision check area around the player
    int checkRadiusX = 2; // Check 2 tiles to the left and right
    int checkRadiusY = 3; // Check 3 tiles below the player for ground detection
    
    // Calculate the range of columns and rows based on the player's rect
    int startCol = std::max(0, static_cast<int>((rect.x) / tileSize) - checkRadiusX);
    int endCol = std::min(cols - 1, static_cast<int>((rect.x + rect.width) / tileSize) + checkRadiusX);
    int startRow = std::max(0, static_cast<int>((rect.y) / tileSize) - checkRadiusY);
    int endRow = std::min(rows - 1, static_cast<int>((rect.y + rect.height) / tileSize) + checkRadiusY);


    // Check for collision with tiles in the expanded collision box
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            const Tile& tile = tiles[row][col];

            // Check if the tile is solid and if it collides with the player's rectangle
            if (tile.isSolid() && CheckCollisionRecs(rect, tile.getRect())) {
                return true; // Collision detected
            }
        }
    }

    return false; // No collision detected
}

int Tilemap::getRows() const { 
    return rows; 
}

int Tilemap::getCols() const { 
    return cols; 
}



// Function to generate cave noise using Simplex noise
float caveNoise(int x, int y, const SimplexNoise& simplex, float frequency, float threshold) {
    float noiseValue = simplex.noise(x * frequency, y * frequency);
    return noiseValue > threshold ? 1.0f : 0.0f;
}

void Tilemap::generateWorld() {
    std::random_device rd;
    int seed = rd(); // Random seed for reproducibility
    SimplexNoise simplex(seed); // Create Simplex noise generator

    int baseGroundLevel = rows / 2; // Set the approximate ground level at half the height of the map
    int maxHeightVariation = 8;     // Maximum height variation for terrain
    float frequency = 0.02f;        // Base frequency for noise (lower for smoother terrain)
    float persistence = 0.7f;       // Controls how quickly noise diminishes in higher octaves
    int octaves = 6;                // Number of noise layers (more = smoother)

    // Cave generation parameters - Adjusted for smaller caves
    float caveFrequency = 0.05f;    // Higher frequency for smaller cave structures
    float caveThreshold = 0.5f;     // Higher threshold for fewer cave openings

    // Temporary height array to smooth terrain
    std::vector<int> heights(cols, baseGroundLevel);

    // Generate initial noise-based terrain heights
    for (int x = 0; x < cols; ++x) {
        heights[x] = baseGroundLevel + static_cast<int>(
            simplex.noise(x * frequency, seed * frequency) * maxHeightVariation);
    }

    // Smooth terrain heights with a localized smoothing pass
    for (int x = 1; x < cols - 1; ++x) {
        heights[x] = (heights[x - 1] + heights[x] + heights[x + 1]) / 3; // Simple averaging
    }

    // Populate the tilemap based on the smoothed heights
    for (int x = 0; x < cols; ++x) {
        int columnHeight = heights[x];

        for (int y = 0; y < rows; ++y) {
            int id = 0;

            // Prevent cave generation below bedrock layer
            if (y == rows - 1) {
                // Bedrock layer at the bottom row
                id = 5; // Bedrock
            }
            else {
                    // Generate cave noise (but only above the bedrock)
                float noiseValue = caveNoise(x, y, simplex, caveFrequency, caveThreshold);

                if (y >= columnHeight && noiseValue == 1.0f && y < rows - 1) {
                    // Check if the cave is within the dirt or grass layers
                    if (y == columnHeight) {
                        // Grass layer becomes id 6 if a cave is present
                        id = 6;
                    } else if (y > columnHeight && y <= columnHeight + 3) {
                        // Dirt layer becomes id 6 if a cave is present
                        id = 6;
                    } else {
                        // Create cave space (air) if not in grass or dirt layers
                        id = 4; // Air (cave space)
                    }
                } else if (y < columnHeight) {
                    // Above the terrain (air)
                    id = 0;
                } else if (y == columnHeight) {
                    // Grass layer (only on the top)
                    id = 1; // Grass
                } else if (y > columnHeight && y <= columnHeight + 3) {
                    // Dirt layer (up to 3 tiles below the grass layer)
                    id = 2; // Dirt
                } else {
                    // Stone layer (below the dirt layer, above bedrock)
                    id = 3; // Stone
                }
            }

            // Set the tile in the map based on id
            switch (id) {
                case 1:
                    setTile(x, y, true, GREEN, id); // Grass
                    break;
                case 2:
                    setTile(x, y, true, BROWN, id); // Dirt
                    break;
                case 3:
                    setTile(x, y, true, GRAY, id); // Stone
                    break;
                case 4:
                    setTile(x, y, false, DARKGRAY, id); // cave wall
                    break;
                case 5:
                    setTile(x, y, true, DARKGRAY, id); // bedrock
                    break;
                default:
                    setTile(x, y, false, BLACK, id); // Air (optional)
                    break;
            }
        }
    }

}

void Tilemap::setTexture(Texture2D spriteSheet) {
    for (auto& row : tiles) {
        for (auto& tile : row) {
            tile.setTexture(spriteSheet); // Assign the sprite sheet directly to the tile
        }
    }
}

void Tilemap::TilePlacement(const Camera2D& camera, int tileSize, Vector2 PlayerPos, Texture2D SpriteSheetDrops, Texture2D SpriteSheetBlocks) {
    // Get world mouse position (adjusted for camera)
    Vector2 mousePosition = GetMousePosition();
    Vector2 worldMousePos = GetScreenToWorld2D(mousePosition, camera);

    // Calculate tile indices
    int mouseTileX = static_cast<int>(worldMousePos.x / tileSize);
    int mouseTileY = static_cast<int>(worldMousePos.y / tileSize);

    // Check bounds
    if (mouseTileX >= 0 && mouseTileX < cols && 
        mouseTileY >= 0 && mouseTileY < rows) {
        // Draw highlight
        Rectangle highlightRect = { 
            static_cast<float>(mouseTileX * tileSize), 
            static_cast<float>(mouseTileY * tileSize), 
            static_cast<float>(tileSize), 
            static_cast<float>(tileSize) 
        };
        float interactionRange = 100.0f; // Define interactionRange
        if (Vector2Distance(PlayerPos, {static_cast<float>(mouseTileX * tileSize), static_cast<float>(mouseTileY * tileSize)}) <= interactionRange){
            // Check if the tile is solid (assuming you have a method like isSolid)
            if (tiles[mouseTileY][mouseTileX].isSolid()) {
                // Draw a transparent overlay: solid tiles get a transparent black overlay, non-solid tiles get white
                DrawRectangleRec(highlightRect, (Color){ 0, 0, 0, 32 });
            } 
            else {
                DrawRectangleRec(highlightRect, (Color){ 255, 255, 255, 32 });
            }


            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && tiles[mouseTileY][mouseTileX].isSolid() && tiles[mouseTileY][mouseTileX].getID() != 4 &&
            tiles[mouseTileY][mouseTileX].getID() != 5 ){
                Tile dropTile = getTileAt(worldMousePos.x, worldMousePos.y);

                // Calculate the source rectangle for the drop sprite based on the tile ID
                Rectangle sourceRect = {
                    static_cast<float>((dropTile.getID() - 1) * 32), // Horizontal offset based on ID
                    0.0f,                                            // Row 0 (only one row in this spritesheet)
                    32.0f,                                           // Tile width
                    32.0f                                            // Tile height
                };

                // Create a drop item with the correct sourceRect for the sprite
                if (dropTile.getID() == 1) {
                    Item drop("grass", 1, 1, {mouseTileX * tileSize, mouseTileY * tileSize}, SpriteSheetDrops, {mouseTileX * tileSize, mouseTileY * tileSize});
                    dropManager.addDrop(drop);
                } else if (dropTile.getID() == 2) {
                    Item drop("dirt", 2, 1, {mouseTileX * tileSize, mouseTileY * tileSize}, SpriteSheetDrops, {mouseTileX * tileSize, mouseTileY * tileSize});
                    dropManager.addDrop(drop);
                } else if (dropTile.getID() == 3) {
                    Item drop("stone", 3, 1, {mouseTileX * tileSize, mouseTileY * tileSize}, SpriteSheetDrops, {mouseTileX * tileSize, mouseTileY * tileSize});
                    dropManager.addDrop(drop);
                }
            

            if (dropTile.getID() == 3) {
                // Replace the broken tile with an id == 4 tile
                setTile(mouseTileX, mouseTileY, false, DARKGRAY, 4); // Example replacement color and ID
                tiles[mouseTileY][mouseTileX].setTexture(SpriteSheetBlocks);
                
            } 
            else if (dropTile.getID() == 2){
                // Replace the broken tile with an id == 6 tile
                setTile(mouseTileX, mouseTileY, false, DARKGRAY, 6);
                tiles[mouseTileY][mouseTileX].setTexture(SpriteSheetBlocks);
            }
            else {
                    // Remove the tile for other IDs
                    setTile(mouseTileX, mouseTileY, false, BLACK, 0);
                }
            }
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !tiles[mouseTileY][mouseTileX].isSolid()) {
                // Check if the placement position overlaps with the player's position
                Rectangle playerRect = { PlayerPos.x, PlayerPos.y, static_cast<float>(tileSize), static_cast<float>(tileSize * 2) }; // Adjusted for player height
                Rectangle tileRect = { static_cast<float>(mouseTileX * tileSize), static_cast<float>(mouseTileY * tileSize), static_cast<float>(tileSize), static_cast<float>(tileSize) };

                if (!CheckCollisionRecs(playerRect, tileRect)) {
                    setTile(mouseTileX, mouseTileY, true, GREEN, 5); 
                    tiles[mouseTileY][mouseTileX].setTexture(SpriteSheetBlocks);
                }
            }
        }
        dropManager.drawDrops();
        dropManager.updateDrops(PlayerPos, 300.0f);
    }
}

// Function to find the ground level (highest solid tile, corresponding to smallest Y) for the player's X position
int Tilemap::getGroundLevel(int x) {
    // Convert the X position to the column index in the tile array
    int column = x / tileSize; 

    // Start from the top of the map and move downward
    for (int y = 0; y < rows; ++y) {
        if (tiles[y][column].isSolid()) {
            return ((y * tileSize) - 64);  // Return the Y position of the first solid tile found
        }
    }
    return -1; // Return -1 if no solid tile is found
}

DropManager Tilemap::getDropManager(){
    return dropManager;
}