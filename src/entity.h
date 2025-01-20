#ifndef TILEMAP_H
#define TILEMAP_H

#include <raylib.h>
#include <vector>
using namespace std;

class Tile {
private:
    Rectangle rect;          // The tile's bounding box
    bool solid;              // Whether the tile is solid (for collision)
    Color color;             // The color to draw the tile (debug only)
    Texture2D texture;       // The texture to draw the tile
    Texture2D dropSprite;    // Sprite do drop do tile
    bool breakable;          // Indica se o tile é quebrável
    int id;                  // The tile's ID

public:
    Tile(float x, float y, float size, bool solid, Color color, int id = 0, bool breakable = false, Texture2D dropSprite = {});

    void Draw() const;                   // Draws the tile
    void setTexture(Texture2D texture);  // Sets the tile texture
    void setDropSprite(Texture2D sprite); // Define o sprite do drop
    void setBreakable(bool value);       // Define se o tile é quebrável
    void SetID(int ID);                  // Define o ID do tile
    void SetSolid(bool solid);           // Define se o tile é sólido

    Rectangle getRect() const;           // Retorna o retângulo do tile
    bool isSolid() const;                // Retorna se o tile é sólido
    bool isBreakable() const;            // Retorna se o tile é quebrável
    int getID() const;                   // Retorna o ID do tile
    Texture2D getDropSprite() const;     // Retorna o sprite do drop
};

// Estrutura para gerenciar drops
struct Drop {
    Vector2 position; // Posição atual do drop
    Vector2 speed;    // Velocidade do drop (para gravidade)
    Texture2D sprite; // Sprite do drop
};

// Forward declaration de Player
class Player;

class Tilemap {
private:
    vector<vector<Tile>> tiles; // Matriz de tiles
    vector<Drop> drops;         // Lista de drops no mapa
    int rows, cols;             // Número de linhas e colunas
    float tileSize;             // Tamanho de cada tile

public:
    Tilemap(int rows, int cols, float tileSize); // Construtor

    void setTile(int x, int y, bool solid, Color color, int id, bool breakable = false, Texture2D dropSprite = {}); // Configura um tile
    void addDrop(Vector2 position, Texture2D sprite); // Adiciona drops ao mapa
    void collectDrops(Player& player, vector<Texture2D>& inventory); // Coleta drops
    void UpdateDrops(); // Atualiza a física dos drops
    void Draw() const; // Desenha o mapa e os drops

    const Tile& getTileAt(float x, float y) const; // Obtém um tile com base na posição
    bool checkCollision(const Rectangle& rect) const; // Verifica colisão com retângulo

    int getRows() const; // Retorna o número de linhas
    int getCols() const; // Retorna o número de colunas
    float getTileSize() const; // Retorna o tamanho do tile

    void generateWorld(); // Gera o mundo
    void setTexture(Texture2D grassTexture, Texture2D dirtTexture, Texture2D stoneTexture); // Define texturas dos tiles
};

#endif // TILEMAP_H
