#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "tilemap.h"
#include "inventory.h"

/// --- CLASSE PLAYER ---  
// Controla todas as propriedades e comportamentos do jogador no jogo, incluindo:  
// - Movimentação e física (gravidade, atrito, colisões)  
// - Animação e renderização do sprite  
// - Interação com a câmera e limites do mundo  
//--------------------------------------------------------------------------------  

class Player {  
private:  
    // ======================  
    // PROPRIEDADES FÍSICAS  
    // ======================  
    Vector2 position;          // Posição central do jogador no mundo (coordenadas X,Y)  
    Vector2 speed;             // Velocidade atual nos eixos (X: horizontal, Y: vertical)  
    int width, height;         // Dimensões da hitbox (normalmente menor que o sprite visual)  
    float maxSpeed;            // Velocidade máxima permitida (evita aceleração infinita)  
    float gravity;             // Força aplicada a cada frame quando no ar (simula gravidade)  
    float friction;            // Resistência que reduz velocidade horizontal quando solta tecla  
    bool grounded;             // Flag que indica contato com superfície (afeta pulo/queda)  

    // ======================  
    // SISTEMA DE COLISÃO  
    // ======================  
    Rectangle playerRec;       // Área de colisão principal (usada para interagir com objetos)  
    Rectangle belowRec;        // Sensor de chão (detecta plataformas abaixo do jogador)  

    // ======================  
    // ANIMAÇÃO E SPRITE  
    // ======================  
    Texture2D playerSprite;    // Textura contendo todos os frames de animação (spritesheet)  
    int currentFrame;          // Índice do frame atual na spritesheet (0 a n)  
    float frameTime;           // Cronômetro para controle de tempo entre frames  
    float frameDuration;       // Tempo necessário (em segundos) para avançar para próximo frame  
    bool isFlipped;            // Controla espelhamento horizontal do sprite (direção esquerda)  

    // ======================  
    // CONTROLE DE CÂMERA  
    // ======================  
    Camera2D camera;           // Configurações da câmera que segue o jogador suavemente  

    // ======================  
    // MÉTODOS INTERNOS  
    // ======================  
    void updateRectangles();   // Atualiza geometria de colisão após movimento  
    void updateAnimation();    // Gerencia transições de animação baseadas no estado do jogador  


    // [Exemplo de fluxo de uso:]  
    // 1. Update() atualiza posição via input/física  
    // 2. updateRectangles() ajusta hitboxes  
    // 3. handleCollisions() resolve colisões com tilemap  
    // 4. updateAnimation() avança frames conforme estado  
    // 5. Draw() renderiza sprite na posição final  
public:
    Player();                  // Construtor

    // Estados do jogador para animações
    enum PlayerState { 
        IDLE,                  // Estado parado
        RUNNING,               // Estado correndo
        JUMPING,               // Estado pulando
        FALLING                // Estado caindo
    };

    PlayerState currentState;  // Estado atual do jogador

    // Métodos públicos
    void Update(const Tilemap& tilemap, float deltaTime);  // Atualiza o movimento, colisões e animações do jogador
    void Draw() const;                    // Desenha o jogador na tela

    // Getters
    Vector2 getPosition() const;
    Vector2 getSpeed() const;
    Rectangle getRec() const;
    bool isGrounded() const;
    Camera2D getCamera() const; 
    int getHeight();
    int getcurrentStateFrameCount(PlayerState state) const;

    // Setters
    void setPosition(Vector2 newPosition);
    void setSpeed(Vector2 newSpeed);
    void setSprite(Texture2D sprite);

    // inicializacao da camera
    void initializeCamera(Tilemap tilemap);
};

#endif // PLAYER_H
