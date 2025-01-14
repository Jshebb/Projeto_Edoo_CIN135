#include "entity.h"
#include <cmath>
#include <random>

// Tile Class Implementations
Tile::Tile(float x, float y, float size, bool solid, Color color, int id) 
    : rect({x, y, size, size})
    , solid(solid)
    , color(color) 
    , texture({0})
    , id (id)
{
}

void Tile::Draw() const {
    if (id == 1) {
        DrawTexture(texture, rect.x, rect.y, WHITE);
    }
    else if (id == 2) {
        DrawTexture(texture, rect.x, rect.y, WHITE);
    }
    else if (id == 3) {
        DrawTexture(texture, rect.x, rect.y, WHITE);
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
Tilemap::Tilemap(int rows, int cols, float tileSize)
    : rows(rows), cols(cols), tileSize(tileSize) {
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

void Tilemap::Draw() const {
    for (const auto& row : tiles) {
        for (const auto& tile : row) {
            tile.Draw();
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

            // Debug: Highlight tiles being checked (in Yellow)
            float tileX = col * tileSize;
            float tileY = row * tileSize;
            

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

// Function to generate smoother Perlin noise using multiple octaves
float smoothNoise(int x, int seed, int octaves = 4, float persistence = 0.5f, float frequency = 0.1f) {
    float noise = 0.0f;
    float amplitude = 1.0f;
    float maxAmplitude = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        std::mt19937 rng(seed + x * (i + 1));
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        noise += std::sin((x * frequency) * (1 << i)) * amplitude * dist(rng);
        maxAmplitude += amplitude;
        amplitude *= persistence; // Reduce amplitude with each octave
    }

    return noise / maxAmplitude; // Normalize the result
}

void Tilemap::generateWorld() {
    std::random_device rd;
    int seed = rd(); // Random seed for reproducibility

    int baseGroundLevel = rows / 2; // Set the approximate ground level at half the height of the map
    int maxHeightVariation = 8;    // Maximum height variation for terrain
    float frequency = 0.02f;       // Base frequency for noise (lower for smoother terrain)
    float persistence = 0.7f;      // Controls how quickly noise diminishes in higher octaves
    int octaves = 6;               // Number of noise layers (more = smoother)

    // Temporary height array to smooth terrain
    std::vector<int> heights(cols, baseGroundLevel);

    // Generate initial noise-based terrain heights
    for (int x = 0; x < cols; ++x) {
        heights[x] = baseGroundLevel + static_cast<int>(
            smoothNoise(x, seed, octaves, persistence, frequency) * maxHeightVariation);
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

            if (y < columnHeight) {
                // Above the terrain (air)
                id = 0;
            } else if (y == columnHeight) {
                // Grass layer (only on the top)
                id = 1; // Grass
            } else if (y > columnHeight && y <= columnHeight + 3) {
                // Dirt layer (up to 3 tiles below the grass layer)
                id = 2; // Dirt
            } else {
                // Stone layer (below the dirt layer)
                id = 3; // Stone
            }

            // Set the tile in the map
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
                default:
                    setTile(x, y, false, BLACK, id); // Air (optional)
                    break;
            }
        }
    }
}




// Call this after generating the world to assign textures properly
void Tilemap::setTexture(Texture2D grassTexture, Texture2D dirtTexture, Texture2D stoneTexture) {
    for (auto& row : tiles) {
        for (auto& tile : row) {
                if (tile.getID() == 1) {
                    tile.setTexture(grassTexture);   // Grass texture for grass tiles
                } 
                else if (tile.getID() == 2) {
                    tile.setTexture(dirtTexture);   // Dirt texture for dirt tiles
                } 
                else if (tile.getID() == 3) {
                    tile.setTexture(stoneTexture);  // Stone texture for stone tiles
                }
            }
    }
}
