#include "Player.h"
#include "tilemap.h"
#include "inventory.h"
#include <raylib.h>
#include <vector>
#include <cmath>
#include <string>
#include <iostream>

// Construtor da classe Player
// Inicializa todas as propriedades do jogador com valores padrão
Player::Player()
    : position({ 0, 0 }),   // Posição inicial nas coordenadas (x,y) do mundo 2D
      speed({ 0, 0 }),      // Vetor de velocidade inicial (x,y) - movimento horizontal/vertical
      width(32),            // Largura do jogador em pixels (hitbox)
      height(32),           // Altura do jogador em pixels (hitbox)
      maxSpeed(5.0f),       // Velocidade máxima de movimento em pixels/frame
      gravity(0.5f),        // Força da gravidade aplicada a cada frame (queda/pulo)
      friction(1.0f),       // Força de atrito para desaceleração (contra movimento horizontal)
      grounded(true),       // Estado inicial: jogador está em contato com o chão
      playerSprite({ 0 }),  // Textura/Spritesheet do jogador (inicialmente vazia)
      currentState(IDLE),   // Estado inicial da animação (ocioso/parado)
      currentFrame(0),      // Quadro atual da animação (controle da spritesheet)
      frameTime(0.0f),      // Temporizador para troca de quadros de animação
      frameDuration(0.1f),  // Tempo em segundos que cada quadro da animação é exibido
      isFlipped(false)      // Controle de espelhamento do sprite (direção esquerda/direita)
{
    updateRectangles(); // Atualiza os retângulos de colisão e renderização
                        // com base nas propriedades definidas acima
}

// Atualiza os retângulos do jogador (limites)
void Player::updateRectangles() {
    playerRec = { position.x, position.y, (float)width, (float)height };
    belowRec = { position.x, position.y + height, (float)width, 1 };
}

// Inicializa a câmera
void Player::initializeCamera(Tilemap tilemap) {
    // Define o alvo da câmera como a posição inicial do jogador
    camera.target = { position.x, position.y };

    // Define o deslocamento da câmera para o centro da tela
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    // Define o zoom e a rotação da câmera
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    // Calcula o tamanho do mundo baseado no tamanho do tilemap
    Vector2 worldSize = { tilemap.getCols() * tilemap.getTileSize(), tilemap.getRows() * tilemap.getTileSize() };

    // Calcula a metade da largura e altura da tela
    float halfScreenWidth = GetScreenWidth() / 2.0f;
    float halfScreenHeight = GetScreenHeight() / 2.0f;

    // Ajusta a posição da câmera para não ultrapassar os limites do mundo
    if (camera.target.x < halfScreenWidth)
        camera.target.x = halfScreenWidth;
    if (camera.target.y < halfScreenHeight)
        camera.target.y = halfScreenHeight;
    if (camera.target.x > worldSize.x - halfScreenWidth)
        camera.target.x = worldSize.x - halfScreenWidth;
    if (camera.target.y > worldSize.y - halfScreenHeight)
        camera.target.y = worldSize.y - halfScreenHeight;
}

void Player::Update(const Tilemap& tilemap, float deltaTime) {
    // Movimento horizontal
    if (IsKeyDown(KEY_D)) {
        speed.x = fmin(speed.x + 1.0f, maxSpeed);
        currentState = RUNNING;  // Muda para o estado de corrida ao se mover
    } else if (IsKeyDown(KEY_A)) {
        speed.x = fmax(speed.x - 1.0f, -maxSpeed);
        currentState = RUNNING;  // Muda para o estado de corrida ao se mover
    } else {
        speed.x = (speed.x > 0) ? fmax(speed.x - friction, 0) : fmin(speed.x + friction, 0);
        if (grounded) {
            currentState = IDLE;  // Muda para o estado ocioso quando não está se movendo
        }
    }

    // Pulo
    if (IsKeyPressed(KEY_SPACE) && grounded) {
        speed.y = -10.0;  // Define a velocidade vertical negativa para pular
        grounded = false;  // Garante que o jogador não está no chão após pular
        currentState = JUMPING;  // Muda para o estado de pulo
    }

    // Aplica gravidade
    speed.y = fmin(speed.y + gravity, maxSpeed);  // Incrementa a velocidade vertical com a gravidade, limitada pela velocidade máxima

    // Movimento horizontal e subida de escadas
    position.x += speed.x;  // Atualiza a posição horizontal com base na velocidade
    updateRectangles();  // Atualiza os retângulos de colisão e renderização
    if (tilemap.checkCollision(playerRec)) {
        // Verifica se há uma "escada" na frente do jogador
        // Verifica se há um degrau acima do jogador
        Rectangle stepCheck = { playerRec.x, playerRec.y - playerRec.height, playerRec.width, playerRec.height };
        if (!tilemap.checkCollision(stepCheck)) {
            // Sobe o degrau
            position.y -= 32;  // Ajusta a altura para subir o degrau
            updateRectangles();
        } else {
            // Reseta a posição se não conseguir subir
            position.x -= speed.x;
            speed.x = 0;
            updateRectangles();
        }
        }

        // Movimento vertical e colisão
        position.y += speed.y;
        updateRectangles();
        if (tilemap.checkCollision(playerRec)) {
        position.y -= speed.y;
        speed.y = 0;
        grounded = true;
        if (speed.x == 0){
            currentState = IDLE;  // Reseta o estado atual quando no chão
        }
        updateRectangles(); // Atualiza os retângulos de colisão e renderização
        } else {
        grounded = false; // Define que o jogador não está no chão
        if (speed.y > 0) currentState = JUMPING;  // Atualiza o estado atual para pulo se estiver no ar
        }

        // Ajusta a duração do quadro para o estado ocioso para tornar a animação mais lenta
        if (currentState == IDLE) {
        frameDuration = 0.2f; // Duração do quadro mais lenta para o estado ocioso
        } else {
        frameDuration = 0.1f; // Duração padrão do quadro para outros estados
        }

        // Atualiza o quadro da animação
        frameTime += deltaTime; // Acumula o tempo decorrido desde o último quadro
        if (frameTime >= frameDuration) { // Verifica se é hora de mudar para o próximo quadro
        frameTime = 0.0f; // Reseta o temporizador do quadro
        if (currentState == JUMPING) { // Se o estado atual for pulo
            if (speed.y > 0){ // Se a velocidade vertical for positiva (caindo)
            currentFrame = 1; // Define o quadro atual para o segundo quadro de pulo
            }
            else if (speed.y < 0){ // Se a velocidade vertical for negativa (subindo)
            currentFrame = 0; // Define o quadro atual para o primeiro quadro de pulo
            }
        } else {
            // Cicla através dos quadros para outros estados
            currentFrame = (currentFrame + 1) % getcurrentStateFrameCount(currentState); // Avança para o próximo quadro e reseta se necessário
        }
        }

        if (speed.x > 0){ // Se a velocidade horizontal for positiva (movendo para a direita)
        isFlipped = false; // Não espelha o sprite
        }
        else if (speed.x < 0){ // Se a velocidade horizontal for negativa (movendo para a esquerda)
        isFlipped = true; // Espelha o sprite horizontalmente
        }

    // Atualiza suavemente a câmera para seguir o jogador
    float cameraSmoothness = 0.1f;  // Valores menores para movimento mais lento e suave
    Vector2 desiredTarget = { position.x + width / 2, position.y + height / 2 };
    camera.target.x += (desiredTarget.x - camera.target.x) * cameraSmoothness;
    camera.target.y += (desiredTarget.y - camera.target.y) * cameraSmoothness;

}

// Retorna a contagem de quadros para o estado atual do jogador
int Player::getcurrentStateFrameCount(PlayerState currentState) const {
    switch (currentState) {
        case IDLE: return 4;  // Contagem total de quadros
        case RUNNING: return 6;
        case JUMPING: return 2;  // Contagem fixa de quadros para pulo
        default: return 1;  // Padrão
    }
}


void Player::Draw() const {
    // Obtém a linha atual com base no estado do jogador
    int row = static_cast<int>(currentState); // Supondo que PlayerState corresponda ao índice da linha

    // Limita o quadro atual para garantir que não exceda a contagem de quadros para o estado atual
    int frameCount = getcurrentStateFrameCount(currentState);
    int validFrame = currentFrame % frameCount; // Garante que o quadro permaneça dentro dos limites

    // Define a porção da spritesheet a ser renderizada (retângulo de origem)
    Rectangle sourceRec = { validFrame * 32, row * 32, 32, 32 };

    // Define o retângulo de destino (escalado para 64x64) com um deslocamento de altura
    Rectangle destRec = { position.x - 16, position.y - 32.0f, 64.0f, 64.0f };

    // Verifica se o sprite deve ser espelhado
    if (isFlipped) {
        sourceRec.width = -32; // Espelha o player horizontalmente
    } else {
        sourceRec.width = 32; // Mantém o player não espelhado
    }

    // A origem é definida como (0, 0), sem rotação
    Vector2 origin = { 0, 0 };

    // Desenha o sprite (espelhamento tratado modificando o sourceRec)
    DrawTexturePro(playerSprite, sourceRec, destRec, origin, 0.0f, WHITE);
}


// Getters
Vector2 Player::getPosition() const { 
    return position; 
}

Vector2 Player::getSpeed() const { 
    return speed; 
}

bool Player::isGrounded() const { 
    return grounded; 
}

// Setters
void Player::setPosition(Vector2 newPosition) {
    position = newPosition;
    updateRectangles(); // Atualiza os retângulos de colisão e renderização após mudar a posição
}

void Player::setSpeed(Vector2 newSpeed) { 
    speed = newSpeed; 
}

void Player::setSprite(Texture2D sprite) { 
    playerSprite = sprite; 
}

// Retorna o retângulo de colisão do jogador
Rectangle Player::getRec() const {
    return playerRec;
}

// Retorna a câmera do jogador
Camera2D Player::getCamera() const {
    return camera;
}

// Retorna a altura do jogador
int Player::getHeight() {
    return height;
}
