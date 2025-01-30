#include "tilemap.h"
#include <cmath>
#include <random>
#include <raylib.h>
#include "player.h"
#include "raymath.h"
#include "inventory.h"
#include <iostream>
#include "SimplexNoise.h"


/// --- CLASSE TILE ---  
// Representa um bloco/tile no mundo do jogo com propriedades de:  
// - Colisão e interação física  
// - Aparência visual (cor/textura)  
// - Identificação lógica para sistemas de gameplay  
//----------------------------------------------------------------  

// Implementações da Classe Tile  
Tile::Tile(float x, float y, float size, bool solid, Color color, int id) 
    : rect({x, y, size, size})  // Geometria do tile (posição + tamanho)
    , solid(solid)                 // Se bloqueia movimento (true) ou é passável (false)
    , color(color)                    // Cor base para renderização (caso não tenha textura)
    , texture({0})                  // Textura gráfica (inicialmente vazia)
    , id(id)                        // ID único para identificação lógica (ex: grama=1, pedra=2)
{
}

// Função Draw para renderizar o tile
void Tile::Draw(Vector2 position) const {
    // Calcula o retângulo de origem com base no ID do tile
    Rectangle sourceRect = {
        static_cast<float>((id - 1) * 32), // Deslocamento horizontal (ID-1 porque os IDs começam em 1)
        0.0f,                             // Deslocamento vertical (linha 0 na folha de sprites)
        32.0f,                            // Largura do tile
        32.0f                             // Altura do tile
    };
    if (id == 0) {
        DrawTextureRec({0}, sourceRect, position, WHITE);
    } else {
        // Desenha o tile usando o retângulo de origem
        DrawTextureRec(texture, sourceRect, position, WHITE);
    }
}
// Retorna o retângulo de colisão/visualização do tile
// Útil para verificação de colisões ou renderização precisa
Rectangle Tile::getRect() const { 
    return rect; 
}

// Verifica se o tile é sólido (bloqueia movimento)
// Retorna true para blocos intransponíveis (ex: paredes)
bool Tile::isSolid() const { 
    return solid; 
}

// Define uma textura para o tile (substitui a cor sólida)
// Parâmetro: texture - Textura carregada a ser aplicada
void Tile::setTexture(Texture2D texture) {
    this->texture = texture;
}

// Obtém o ID único do tile (usado para identificação lógica)
// Retorno: Número inteiro representando o tipo do tile
int Tile::getID() {
    return id;
}

// Altera o ID do tile (para mudança dinâmica de tipo)
// Útil para sistemas de destruição/transformação de blocos
// Parâmetro: ID - Novo identificador (ex: 0=vazio, 1=terra, 2=pedra)
void Tile::SetID(int ID) {
    id = ID;
}

// Modifica a propriedade de solidez do tile em tempo real
// Permite criar blocos que podem se tornar intransponíveis
// Parâmetro: solid - Novo estado de colisão (true/false)
void Tile::SetSolid(bool solid) {
    this->solid = solid;
}

/// --- CLASSE TILEMAP ---  
// Representa o mapa de tiles do mundo do jogo com funcionalidades de:  
// - Inicialização e configuração dos tiles  
// - Renderização dos tiles visíveis na tela  
// - Verificação de colisões com os tiles  
// - Geração procedural do mundo usando Simplex Noise  
// - Manipulação de tiles (colocação e remoção)  
// - Gerenciamento de drops e inventário  
//----------------------------------------------------------------  

Tilemap::Tilemap(int rows, int cols, float tileSize, DropManager dropManager, Inventory inventory)
    : rows(rows), cols(cols), tileSize(tileSize), dropManager(dropManager), inventory(inventory)
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

// Altera as propriedades de um tile específico no mapa
// Parâmetros:
// - x, y:       Coordenadas no grid do mapa (não no mundo)
// - solid:      Se o tile bloqueia movimento
// - color:      Cor visual do tile
// - id:         Identificador lógico para game design
void Tilemap::setTile(int x, int y, bool solid, Color color, int id) {
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        // Substitui o tile existente por um novo com propriedades específicas
        tiles[y][x] = Tile(
            x * tileSize,   // Calcula posição X real no mundo
            y * tileSize,   // Calcula posição Y real no mundo
            tileSize,       // Mantém tamanho consistente
            solid,          // Nova propriedade de solidez
            color,          // Nova cor
            id              // Novo ID lógico
        );
    }
}

// Renderiza o tilemap de forma otimizada, desenhando apenas os tiles visíveis na câmera
void Tilemap::Draw(Camera2D camera, int tileSize) const {
    // ================================================
    // CÁLCULO DA ÁREA VISÍVEL
    // ================================================
    // Determina o range de tiles visíveis com base na posição da câmera
    int startX = static_cast<int>(camera.target.x - GetScreenWidth() / 2) / tileSize;  // Tile inicial no eixo X
    int endX = static_cast<int>(camera.target.x + GetScreenWidth() / 2) / tileSize;    // Tile final no eixo X
    int startY = static_cast<int>(camera.target.y - GetScreenHeight() / 2) / tileSize; // Tile inicial no eixo Y
    int endY = static_cast<int>(camera.target.y + GetScreenHeight() / 2) / tileSize;   // Tile final no eixo Y

    // ================================================
    // LIMITAÇÃO
    // ================================================
    // Garante que os valores fiquem dentro dos limites do mundo
    startX = std::max(0, startX);                // Não permite valores negativos
    endX = std::min(cols - 1, endX);             // Não ultrapassa coluna máxima
    startY = std::max(0, startY);                // Não permite valores negativos
    endY = std::min(rows - 1, endY);             // Não ultrapassa linha máxima

    // ================================================
    // RENDERIZAÇÃO OTIMIZADA
    // ================================================
    // Desenha apenas os tiles na área visível calculada
    for (int y = startY; y <= endY; ++y) {                // Loop vertical
        for (int x = startX; x <= endX; ++x) {            // Loop horizontal
            // Obtém a posição mundial do tile e renderiza
            Vector2 tilePos = {
                tiles[y][x].getRect().x,  // Posição X real no mundo
                tiles[y][x].getRect().y   // Posição Y real no mundo
            };
            tiles[y][x].Draw(tilePos);  // Renderiza o tile na posição calculada
        }
    }
}


const Tile& Tilemap::getTileAt(float x, float y) const {
    int col = static_cast<int>(std::round((x - tileSize / 2) / tileSize));  // Ajusta para o centro do tile
    int row = static_cast<int>(std::round((y - tileSize / 2) / tileSize));  // Ajusta para o centro do tile

    // Limita col e row dentro dos limites do mapa de tiles
    col = std::max(0, std::min(col, cols - 1));  // Limita col para [0, cols - 1]
    row = std::max(0, std::min(row, rows - 1));  // Limita row para [0, rows - 1]

    return tiles[row][col];
}


float Tilemap::getTileSize() const { 
    return tileSize; 
}

bool Tilemap::checkCollision(const Rectangle& rect) const {
    // Define o tamanho da área de verificação de colisão ao redor do jogador
    int raioVerificacaoX = 2; // Verifica 2 tiles à esquerda e à direita
    int raioVerificacaoY = 3; // Verifica 3 tiles abaixo do jogador para detecção de solo
    
    // Calcula o intervalo de colunas e linhas com base no retângulo do jogador
    int colunaInicio = std::max(0, static_cast<int>((rect.x) / tileSize) - raioVerificacaoX);
    int colunaFim = std::min(cols - 1, static_cast<int>((rect.x + rect.width) / tileSize) + raioVerificacaoX);
    int linhaInicio = std::max(0, static_cast<int>((rect.y) / tileSize) - raioVerificacaoY);
    int linhaFim = std::min(rows - 1, static_cast<int>((rect.y + rect.height) / tileSize) + raioVerificacaoY);

    // Verifica colisão com tiles na área expandida de colisão
    for (int linha = linhaInicio; linha <= linhaFim; ++linha) {
        for (int coluna = colunaInicio; coluna <= colunaFim; ++coluna) {
            const Tile& tile = tiles[linha][coluna];

            // Verifica se o tile é sólido e se colide com o retângulo do jogador
            if (tile.isSolid() && CheckCollisionRecs(rect, tile.getRect())) {
                return true; // Colisão detectada
            }
        }
    }

    return false; // Nenhuma colisão detectada
}

int Tilemap::getRows() const { 
    return rows; 
}

int Tilemap::getCols() const { 
    return cols; 
}

// Função para gerar ruído de caverna usando ruído Simplex
float caveNoise(int x, int y, const SimplexNoise& simplex, float frequency, float threshold) {
    float noiseValue = simplex.noise(x * frequency, y * frequency);
    return noiseValue > threshold ? 1.0f : 0.0f;
}

// geracao de mundo procedural com cavernas, utiliza Simplex Noise
void Tilemap::generateWorld() {
    std::random_device rd;
    int seed = rd(); // Semente aleatória para reprodutibilidade
    SimplexNoise simplex(seed); // Cria gerador de ruído Simplex

    int baseGroundLevel = rows / 2; // Define o nível do solo aproximadamente na metade da altura do mapa
    int maxHeightVariation = 8;     // Variação máxima de altura para o terreno
    float frequency = 0.02f;        // Frequência base para o ruído (menor para terreno mais suave)
    float persistence = 0.7f;       // Controla a rapidez com que o ruído diminui em oitavas superiores
    int octaves = 6;                // Número de camadas de ruído (mais = mais suave)

    // Parâmetros de geração de caverna - Ajustados para cavernas menores
    float caveFrequency = 0.05f;    // Frequência mais alta para estruturas de caverna menores
    float caveThreshold = 0.5f;     // Limite mais alto para menos aberturas de caverna

    // Array temporário de alturas para suavizar o terreno
    std::vector<int> heights(cols, baseGroundLevel);

    // Gera alturas iniciais do terreno baseadas em ruído
    for (int x = 0; x < cols; ++x) {
        heights[x] = baseGroundLevel + static_cast<int>(
            simplex.noise(x * frequency, seed * frequency) * maxHeightVariation);
    }

    // Suaviza as alturas do terreno com uma passagem de suavização localizada
    for (int x = 1; x < cols - 1; ++x) {
        heights[x] = (heights[x - 1] + heights[x] + heights[x + 1]) / 3; // Média simples
    }

    // Popula o tilemap com base nas alturas suavizadas
    for (int x = 0; x < cols; ++x) {
        int columnHeight = heights[x];

        for (int y = 0; y < rows; ++y) {
            int id = 0;

            // Previne a geração de cavernas abaixo da camada de bedrock
            if (y == rows - 1) {
                // Camada de bedrock na última linha
                id = 5; // Bedrock (rocha matriz)
            }
            else {
                // Gera ruído de caverna (mas apenas acima da rocha matriz)
                float noiseValue = caveNoise(x, y, simplex, caveFrequency, caveThreshold);

                if (y >= columnHeight && noiseValue == 1.0f && y < rows - 1) {
                    // Verifica se a caverna está dentro das camadas de terra ou grama
                    if (y == columnHeight) {
                        // Camada de grama se torna id 6 se houver uma caverna
                        id = 6;
                    } else if (y > columnHeight && y <= columnHeight + 3) {
                        // Camada de terra se torna id 6 se houver uma caverna
                        id = 6;
                    } else {
                        // Cria espaço de caverna (ar) se não estiver nas camadas de grama ou terra
                        id = 4; // Ar (espaço de caverna)
                    }
                } else if (y < columnHeight) {
                    // Acima do terreno (ar)
                    id = 0;
                } else if (y == columnHeight) {
                    // Camada de grama (apenas no topo)
                    id = 1; // Grama
                } else if (y > columnHeight && y <= columnHeight + 3) {
                    // Camada de terra (até 3 tiles abaixo da camada de grama)
                    id = 2; // Terra
                } else {
                    // Camada de pedra (abaixo da camada de terra, acima da rocha matriz)
                    id = 3; // Pedra
                }
            }

            // Define o tile no mapa com base no id
            switch (id) {
                case 1:
                    setTile(x, y, true, GREEN, id); // Grama
                    break;
                case 2:
                    setTile(x, y, true, BROWN, id); // Terra
                    break;
                case 3:
                    setTile(x, y, true, GRAY, id); // Pedra
                    break;
                case 4:
                    setTile(x, y, false, DARKGRAY, id); // Parede de caverna
                    break;
                case 5:
                    setTile(x, y, true, DARKGRAY, id); // Rocha matriz
                    break;
                default:
                    setTile(x, y, false, BLACK, id); // Ar (opcional)
                    break;
            }
        }
    }

}

void Tilemap::setTexture(Texture2D spriteSheet) {
    for (auto& row : tiles) {
        for (auto& tile : row) {
            tile.setTexture(spriteSheet); // Atribui a folha de sprites diretamente ao tile
        }
    }
}

// Função para encontrar o nível do solo (tile sólido mais alto, correspondente ao menor Y) para a posição X do jogador
int Tilemap::getGroundLevel(int x) {
    // Converte a posição X para o índice da coluna no array de tiles
    int column = x / tileSize; 

    // Começa do topo do mapa e move-se para baixo
    for (int y = 0; y < rows; ++y) {
        if (tiles[y][column].isSolid()) {
            return ((y * tileSize) - 64);  // Retorna a posição Y do primeiro tile sólido encontrado
        }
    }
    return -1; // Retorna -1 se nenhum tile sólido for encontrado
}

//manejo da destruicao e colocao de tiles no tilemap
void Tilemap::TilePlacement(const Camera2D& camera, int tileSize, Vector2 PlayerPos, 
                            Texture2D SpriteSheetDrops, Texture2D SpriteSheetBlocks) {
    // Obtém a posição do mouse no mundo (ajustada para a câmera)
    Vector2 mousePosition = GetMousePosition();
    Vector2 worldMousePos = GetScreenToWorld2D(mousePosition, camera);

    // Calcula os índices dos tiles
    int mouseTileX = static_cast<int>(worldMousePos.x / tileSize);
    int mouseTileY = static_cast<int>(worldMousePos.y / tileSize);

    // Verifica os limites
    if (mouseTileX >= 0 && mouseTileX < cols && 
        mouseTileY >= 0 && mouseTileY < rows) {
        // Desenha o destaque
        Rectangle highlightRect = { 
            static_cast<float>(mouseTileX * tileSize), 
            static_cast<float>(mouseTileY * tileSize), 
            static_cast<float>(tileSize), 
            static_cast<float>(tileSize) 
        };
        float interactionRange = 100.0f; // Define o alcance de interação
        if (Vector2Distance(PlayerPos, {static_cast<float>(mouseTileX * tileSize), static_cast<float>(mouseTileY * tileSize)}) <= interactionRange) {
            // Verifica se o tile é sólido e desenha o destaque
            if (tiles[mouseTileY][mouseTileX].isSolid()) {
                DrawRectangleRec(highlightRect, (Color){ 0, 0, 0, 32 });
            } else {
                DrawRectangleRec(highlightRect, (Color){ 255, 255, 255, 32 });
            }

            // Lida com o clique esquerdo (quebra de tiles)
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && tiles[mouseTileY][mouseTileX].isSolid() && 
                tiles[mouseTileY][mouseTileX].getID() != 4 && tiles[mouseTileY][mouseTileX].getID() != 5) {
                // Lógica existente para quebrar tiles e gerar drops
                Tile dropTile = getTileAt(worldMousePos.x, worldMousePos.y);
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

                // Substitui ou remove o tile
                if (dropTile.getID() == 3) {
                    setTile(mouseTileX, mouseTileY, false, DARKGRAY, 4);
                    tiles[mouseTileY][mouseTileX].setTexture(SpriteSheetBlocks);
                } else if (dropTile.getID() == 2) {
                    setTile(mouseTileX, mouseTileY, false, DARKGRAY, 6);
                    tiles[mouseTileY][mouseTileX].setTexture(SpriteSheetBlocks);
                } else {
                    setTile(mouseTileX, mouseTileY, false, BLACK, 0);
                }
            }

            // Lida com o clique direito (colocação de tiles)
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !tiles[mouseTileY][mouseTileX].isSolid()) {
                // Verifica se a posição de colocação sobrepõe a posição do jogador
                Rectangle playerRect = { PlayerPos.x, PlayerPos.y, static_cast<float>(tileSize), static_cast<float>(tileSize * 2) }; // Ajustado para a altura do jogador
                Rectangle tileRect = { static_cast<float>(mouseTileX * tileSize), static_cast<float>(mouseTileY * tileSize), static_cast<float>(tileSize), static_cast<float>(tileSize) };

                if (!CheckCollisionRecs(playerRect, tileRect)) {
                    // Verifica se o slot selecionado no inventário tem um bloco válido para colocar
                    Item& selectedItem = inventory.getSelectedItem();
                    if (selectedItem.id != -1 && selectedItem.quantity > 0) {
                        // Determina o tipo de bloco e coloca-o
                        Color blockColor = BLACK; // Cor padrão
                        int blockID = 0;          // ID padrão
                        if (selectedItem.id == 1) {
                            blockColor = GREEN; // Grama
                            blockID = 1;
                        } else if (selectedItem.id == 2) {
                            blockColor = BROWN; // Terra
                            blockID = 2;
                        } else if (selectedItem.id == 3) {
                            blockColor = GRAY;  // Pedra
                            blockID = 3;
                        }

                        if (blockID != 0) { // Bloco válido
                            setTile(mouseTileX, mouseTileY, true, blockColor, blockID);
                            tiles[mouseTileY][mouseTileX].setTexture(SpriteSheetBlocks);

                            // Diminui a quantidade no inventário
                            selectedItem.quantity--;
                            if (selectedItem.quantity <= 0) {
                                inventory.clearSelectedItem(); // Limpa o slot se estiver vazio
                            }
                        }
                    }
                }
            }
        }
        dropManager.drawDrops();
        dropManager.updateDrops(PlayerPos, 300.0f, inventory);
    }
}

DropManager& Tilemap::getDropManager(){
    return dropManager;
}

// Desenha o inventário
void Tilemap::DrawInventory(){
    inventory.Draw();
}

// Atualiza o inventário
void Tilemap::UpdateInventory(){
    Item out = inventory.Update();
    if(out.id != -1){
        dropManager.addDrop(out);
    }
}