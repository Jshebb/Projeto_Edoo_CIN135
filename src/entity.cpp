#include "entity.h"
#include <cmath>

// Tile Class Implementations
Tile::Tile(float x, float y, float size, bool solid, Color color) 
    : rect({x, y, size, size})
    , solid(solid)
    , color(color) 
    , texture({0})
{
}

void Tile::Draw() const {
    Rectangle destRec = { rect.x, rect.y, rect.width, rect.height };

    if (solid) {
        Rectangle sourceRec = { 0, 0, (int)texture.width, (int)texture.height };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
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

void Tilemap::setTile(int x, int y, bool solid, Color color) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        tiles[y][x] = Tile(x * tileSize, y * tileSize, tileSize, solid, color);
    }
}

void Tilemap::setTexture(Texture2D newTexture) {
    texture = newTexture;
    for (auto& row : tiles) {
        for (auto& tile : row) {
            if (tile.isSolid()){
                tile.setTexture(texture);   //textura pra tiles solidos
                } 
        }
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

    int mapWidth = cols * tileSize;
    int mapHeight = rows * tileSize;

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

    // Debug: Draw the collision check area
    DrawRectangleLines(startCol * tileSize, startRow * tileSize, 
                        (endCol - startCol + 1) * tileSize, 
                        (endRow - startRow + 1) * tileSize, BLUE);

    // Check for collision with tiles in the expanded collision box
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            const Tile& tile = tiles[row][col];

            // Debug: Highlight tiles being checked (in Yellow)
            float tileX = col * tileSize;
            float tileY = row * tileSize;
            DrawRectangleLines(tileX, tileY, tileSize, tileSize, YELLOW);

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
