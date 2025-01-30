#ifndef TILEMAP_H
#define TILEMAP_H

#include <raylib.h>
#include <vector>
#include "inventory.h"
using namespace std;

/// --- CLASSE TILE ---  
// Representa um bloco/tile no mundo do jogo com:  
// - Sistema de colisão e propriedades físicas  
// - Renderização visual (cor/textura)  
// - Identificação única para lógica de gameplay  
//----------------------------------------------------------------  

class Tile {  
private:  
    // ======================  
    // PROPRIEDADES BÁSICAS  
    // ======================  
    Rectangle rect;    // Retângulo de colisão/visualização (posição + tamanho)  
    bool solid;        // Indica se o tile é sólido (bloqueia movimento)  
    Color color;       // Cor para renderização de depuração (usada se não houver textura)  
    Texture2D texture; // Textura gráfica (prioritária sobre a cor)  
    int id;            // Identificador único para lógica do jogo (ex: 1=grama, 2=pedra)  

public:  
    // ======================  
    // MÉTODOS PÚBLICOS  
    // ======================  
    // Construtor: inicializa tile com propriedades específicas  
    // Parâmetros:  
    // - x, y:    Posição inicial no mundo  
    // - size:    Tamanho do tile (quadrado)  
    // - solid:   Comportamento de colisão  
    // - color:   Cor base para renderização  
    // - id:      Identificador lógico (opcional)  
    Tile(float x, float y, float size, bool solid, Color color, int id = 0);  

    // Renderiza o tile na posição especificada  
    // Parâmetro: position - Coordenadas do mundo onde será desenhado  
    void Draw(Vector2 position) const;  

    // Define/Substitui a textura do tile  
    // Parâmetro: texture - Nova textura a ser aplicada  
    void setTexture(Texture2D texture);  

    // ======================  
    // GETTERS & SETTERS  
    // ======================  
    Rectangle getRect() const;  // Retorna retângulo de colisão  
    bool isSolid() const;       // Verifica se o tile é sólido  
    void SetID(int ID);         // Altera o ID para mudança dinâmica de tipo  
    void SetSolid(bool solid);  // Modifica propriedade de solidez em tempo real  
    int getID();                // Obtém ID para identificação lógica  
};  

/* Funcionalidades-chave:  
1. Colisão precisa via retângulo  
2. Sistema dual de renderização (textura > cor)  
3. IDs para sistemas de salvamento/destruição  
4. Modificação dinâmica de propriedades */  

/// --- CLASSE TILEMAP ---  
// Gerencia o mundo do jogo composto por tiles, incluindo:  
// - Geração procedural de mundos e cavernas  
// - Renderização otimizada baseada na câmera  
// - Sistema de colisão e interação jogador-ambiente  
// - Integração com inventário e sistema de drops  
//----------------------------------------------------------------------------------------  

class Tilemap {  
private:  
    // ======================  
    // DADOS INTERNOS  
    // ======================  
    vector<vector<Tile>> tiles;  // Grid 2D de tiles (linhas x colunas)  
    int rows, cols;              // Dimensões do mapa em número de tiles  
    float tileSize;              // Tamanho de cada tile em pixels  
    Texture2D texture;           // Spritesheet para renderização dos tiles  

    // ======================  
    // COMPONENTES EXTERNOS  
    // ======================  
    DropManager dropManager;     // Gerenciador de itens dropados no chão  
    Inventory inventory;         // Inventário do jogador para interações  

public:  
    // ======================  
    // CONSTRUTOR  
    // ======================  
    // Cria mapa vazio com dimensões especificadas  
    // Parâmetros:  
    // - rows, cols: Dimensões do mapa em tiles  
    // - tileSize:   Tamanho visual de cada tile  
    // - dropManager: Referência ao gerenciador de drops  
    // - inventory:   Referência ao inventário do jogador  
    Tilemap(int rows, int cols, float tileSize, DropManager dropManager, Inventory inventory);  

    // ======================  
    // MANIPULAÇÃO DE TILES  
    // ======================  
    // Altera propriedades de um tile específico  
    // Parâmetros:  
    // - x, y:   Posição no grid (não coordenadas mundo)  
    // - solid:  Novo estado de colisão  
    // - color:  Nova cor visual  
    // - id:     Novo ID lógico  
    void setTile(int x, int y, bool solid, Color color, int id);  

    // Define a spritesheet para todos os tiles  
    void setTexture(Texture2D SpriteSheet);  

    // ======================  
    // RENDERIZAÇÃO  
    // ======================  
    // Desenha apenas os tiles visíveis na câmera  
    // Parâmetros:  
    // - camera:    Câmera para cálculo de visibilidade  
    // - tileSize:  Tamanho dos tiles (para cálculo preciso)  
    void Draw(Camera2D camera, int tileSize) const;  

    // ======================  
    // CONSULTA DE DADOS  
    // ======================  
    // Obtém tile em coordenadas mundo (útil para interações)  
    const Tile& getTileAt(float x, float y) const;  
    
    // Retorna tamanho dos tiles (para cálculos de posicionamento)  
    float getTileSize() const;  

    // Verifica colisão entre retângulo e tiles sólidos (usado na física do jogador)  
    bool checkCollision(const Rectangle& rect) const;  

    // Retorna dimensões do mapa  
    int getRows() const;  
    int getCols() const;  

    // ======================  
    // GERAÇÃO DE MUNDO  
    // ======================  
    // Cria terreno procedural (superfície + subsolo)  
    void generateWorld();  

    // Gera cavernas usando autômato celular  
    // Parâmetros:  
    // - iterations: Número de passos de suavização  
    // - seed:       Semente para geração aleatória  
    void generateCaves(int iterations, int seed);  

    // ======================  
    // INTERAÇÃO JOGADOR  
    // ======================  
    // Gerencia colocação/remoção de tiles com mouse  
    void TilePlacement(const Camera2D &camera, int tileSize, Vector2 PlayerPos,  
                      Texture2D SpriteSheetDrops, Texture2D SpriteSheetBlocks);  

    // ======================  
    // UTILIDADES  
    // ======================  
    // Encontra a altura do terreno em uma coluna (útil para spawn de entidades)  
    int getGroundLevel(int x);  

    // ======================  
    // ACESSO A COMPONENTES  
    // ======================  
    DropManager& getDropManager();  // Permite acesso externo ao gerenciador de drops  
    void DrawInventory();           // Renderiza interface do inventário  
    void UpdateInventory();         // Atualiza estado do inventário  
};  

/* Funcionalidades-Chave:  
1. Geração procedural com 2 técnicas distintas (superfície + cavernas)  
2. Sistema de colisão otimizado para grandes mapas  
3. Integração bidirecional com inventário (coleta/uso de itens)  
4. Renderização seletiva baseada na visão da câmera */  

// Exemplo de Uso:  
// Tilemap mapa(100, 100, 64.0f, dropManager, inventario);  
// mapa.generateWorld();  
// mapa.generateCaves(4, 12345);  
// mapa.Draw(camera, 64);  

// Comparação com Classe Player:  
// - Foco em gerenciamento de estado global vs. controle de entidade única  
// - Uso intensivo de algoritmos procedural vs. física real-time  
// - Sistema de renderização otimizado para grande volume de objetos  

#endif // TILEMAP_H
