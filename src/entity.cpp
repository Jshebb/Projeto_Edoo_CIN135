#include "entity.h"
#include "player.h"
#include <cmath>
#include <random>

// Tile Class Implementations modify by LM
Tile::Tile(float x, float y, float size, bool solid, Color color, int id, bool breakable, Texture2D dropSprite)
    : rect({x, y, size, size}), solid(solid), color(color), texture({0}), dropSprite(dropSprite), breakable(breakable), id(id) {}

// LM
void Tile::setBreakable(bool value) { breakable = value; }
bool Tile::isBreakable() const { return breakable; }

// LM
void Tile::setDropSprite(Texture2D sprite) { dropSprite = sprite; }
Texture2D Tile::getDropSprite() const { return dropSprite; }

void Tile::Draw() const {
    if (id > 0) {
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

int Tile::getID() const {
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

void Tilemap::setTile(int x, int y, bool solid, Color color, int id, bool breakable, Texture2D dropSprite) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        tiles[y][x] = Tile(x * tileSize, y * tileSize, tileSize, solid, color, id, breakable, dropSprite);
    }
}

void Tilemap::addDrop(Vector2 position, Texture2D sprite) {
    Vector2 dropPosition = {position.x + tileSize * 0.25f, position.y + tileSize * 0.25f}; // Centraliza o drop
    drops.push_back({dropPosition, {0, 0}, sprite}); // Inicia com velocidade zero
}

void Tilemap::collectDrops(Player& player, vector<Texture2D>& inventory) {
    for (auto it = drops.begin(); it != drops.end();) {
        // Ajustar retângulo de colisão do drop
        Rectangle dropRect = {it->position.x, it->position.y, tileSize * 0.5f, tileSize * 0.5f};
        
        if (CheckCollisionCircleRec(player.getPosition(), 16.0f, dropRect)) {
            inventory.push_back(it->sprite); // Adiciona ao inventário
            it = drops.erase(it); // Remove o drop do mapa
        } else {
            ++it;
        }
    }
}

// Atualiza os drops para aplicar gravidade e colisões
void Tilemap::UpdateDrops() {
    for (auto& drop : drops) {
        drop.speed.y += 0.5f; // Aplicar gravidade
        drop.position.y += drop.speed.y;

        // Retângulo para colisão do drop
        Rectangle dropRect = {drop.position.x, drop.position.y, tileSize * 0.5f, tileSize * 0.5f}; 
        if (checkCollision(dropRect)) {
            drop.speed.y = 0; // Para o movimento vertical
            drop.position.y = std::floor(drop.position.y / tileSize) * tileSize; // Ajusta ao bloco
        }
    }
}


void Tilemap::Draw() const {
    for (const auto& row : tiles) {
        for (const auto& tile : row) {
            tile.Draw();
        }
    }

    for (const auto& drop : drops) {
        DrawTextureEx(drop.sprite, drop.position, 0.0f, tileSize / 32.0f, WHITE);
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
float smoothNoise(int x, int seed, int octaves, float persistence, float frequency) {
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

void Tilemap::generateWorld(Texture2D grassSprite, Texture2D dirtSprite, Texture2D stoneSprite) {
    std::random_device rd;
    int seed = rd();

    int baseGroundLevel = rows / 2;
    int maxHeightVariation = 8;
    float frequency = 0.02f;
    float persistence = 0.7f;
    int octaves = 6;

    std::vector<int> heights(cols, baseGroundLevel);

    for (int x = 0; x < cols; ++x) {
        heights[x] = baseGroundLevel + static_cast<int>(
            smoothNoise(x, seed, octaves, persistence, frequency) * maxHeightVariation);
    }

    for (int x = 1; x < cols - 1; ++x) {
        heights[x] = (heights[x - 1] + heights[x] + heights[x + 1]) / 3;
    }

    for (int x = 0; x < cols; ++x) {
        int columnHeight = heights[x];

        for (int y = 0; y < rows; ++y) {
            int id = 0;

            if (y < columnHeight) {
                id = 0; // Air
            } else if (y == columnHeight) {
                id = 1; // Grass
                setTile(x, y, true, GREEN, id, true, grassSprite);
            } else if (y > columnHeight && y <= columnHeight + 3) {
                id = 2; // Dirt
                setTile(x, y, true, BROWN, id, true, dirtSprite);
            } else {
                id = 3; // Stone
                setTile(x, y, true, GRAY, id, true, stoneSprite);
            }
        }
    }
}


void Tilemap::setTexture(Texture2D grassTexture, Texture2D dirtTexture, Texture2D stoneTexture) {
    for (auto& row : tiles) {
        for (auto& tile : row) {
            if (tile.getID() == 1) tile.setTexture(grassTexture);
            else if (tile.getID() == 2) tile.setTexture(dirtTexture);
            else if (tile.getID() == 3) tile.setTexture(stoneTexture);
        }
    }
}
