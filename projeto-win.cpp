#include <GL/glut.h>
#include <time.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <cmath>

int score = 0;
int life = 3;

struct Projectile {
    GLfloat posX;
    GLfloat posY;
    GLfloat dirX;
    GLfloat dirY;
    bool active;
    GLfloat speed;
};

struct Cube {
    GLfloat posX;
    GLfloat posY;
    GLfloat dirX;
    GLfloat dirY;
    GLfloat speed;
};

std::vector<Projectile> projectiles;
std::vector<Cube> cubes;

// Vari�vel para controlar o tempo de atualiza��o
int previousTime = 0;

// Fun��o de temporiza��o para limitar o FPS
void timer(int value) {
    // Obt�m o tempo atual em milissegundos
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    // Calcula o tempo decorrido desde a �ltima atualiza��o
    float deltaTime = (currentTime - previousTime) / 1000.0f; // Converter para segundos

    // Se passou menos de 1/60 segundos (16.67 ms), aguarda o tempo restante
    if (deltaTime < 1.0f / 60.0f) {
        int sleepTime = ((1.0f / 60.0f) - deltaTime) * 1000;
        if (sleepTime > 0) {
            usleep(sleepTime * 1000);
        }
    }

    // Atualiza o tempo anterior para o tempo atual
    previousTime = currentTime;

    // Redesenha a tela
    glutPostRedisplay();

    // Agende a pr�xima chamada da fun��o de temporiza��o
    glutTimerFunc(0, timer, 0);
}

// �ngulo de rota��o do cubo
GLfloat angle = 0.0f;
// Posi��o do cubo
GLfloat posX = 0.0f, posY = 0.0f;

// Cores para cada face do cubole
GLfloat colors[][3] = {
    {1.0f, 0.0f, 0.0f},   // Vermelho
    {1.0f, 0.5f, 0.0f},   // Laranja
    {1.0f, 1.0f, 0.0f},   // Amarelo
    {0.0f, 1.0f, 0.0f},   // Verde
    {0.0f, 0.0f, 1.0f},   // Azul
    {0.5f, 0.0f, 0.5f}    // Roxo
};

int colorIndex = 0;  // Índice da cor atual
float colorChangeSpeed = 0.07f;  // Velocidade de mudança de cor
float rainbowEffectTime = 0.0f; // Tempo para controlar a transição de cores

void rainbowEffect() {
    // Atualiza o tempo para controlar a transição de cores
    rainbowEffectTime += colorChangeSpeed;

    // Calcula as componentes de cor R, G e B com base no tempo e nas funções trigonométricas
    GLfloat red = 0.5f + 0.5f * sin(rainbowEffectTime);
    GLfloat green = 0.5f + 0.5f * sin(rainbowEffectTime + 2.0f * M_PI / 3.0f);
    GLfloat blue = 0.5f + 0.5f * sin(rainbowEffectTime + 4.0f * M_PI / 3.0f);

    // Define as novas cores para o cubo
    for (int i = 0; i < 6; ++i) {
        colors[i][0] = red;
        colors[i][1] = green;
        colors[i][2] = blue;
    }

    // Chama glutPostRedisplay() para redesenhar o cubo com as novas cores
    glutPostRedisplay();
}




bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false;

float randomFloat() {
   return -100.0f + (200.0f * rand()) / (RAND_MAX + 100.0f);
}

// Trecho destino a permiter o use v�rias teclas ao mesmo tempo
bool buffer[256];

void keyboard(unsigned char key, int x, int y) {
    buffer[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    buffer[key] = false;
}

void moveCube() {
    if (buffer['w']) { // Move para cima
        posY += 0.02f;
    }
    if (buffer['s']) { // Move para baixo
        posY -= 0.02f;
    }
    if (buffer['a']) { // Move para a esquerda
        posX -= 0.02f;
    }
    if (buffer['d']) { // Move para a direita
        posX += 0.02f;
    }
}

GLfloat mouseX, mouseY;
int windowWidth = 1280;
int windowHeight = 720;

void passiveMotion(int x, int y) {
    // Atualiza as coordenadas do mouse
    mouseX = x;
    mouseY = y;
    std::cout << "Posi��o do mouse: x = " << x << ", y = " << y << std::endl;
    
    // Normaliza as coordenadas do mouse para o intervalo entre -1 e 1
    float normalizedX = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2.0f - 1.0f;
    float normalizedY = 1.0f - (float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;

    //std::cout << "Posi��o do mouse (normalizada): x = " << normalizedX << ", y = " << normalizedY << std::endl;
}

float gunPosX, gunPosY;
void drawGun() {
    // Define as coordenadas da pistola em relação ao centro do cubo
    float gunOffsetX = 0.15f;
    float gunOffsetY = 0.0f;

    // Obtém as coordenadas do mouse normalizadas
    float mousePosX = static_cast<float>(mouseX) / static_cast<float>(windowWidth) * 2.0f - 1.0f;
    float mousePosY = 1.0f - static_cast<float>(mouseY) / static_cast<float>(windowHeight) * 2.0f;

    // Calcula o ângulo entre a pistola e a posição do mouse
    GLfloat normPosX =(posX/6.25);
    GLfloat normPosY =(posY/3.10);


    float angle = atan2(mousePosY - normPosY, mousePosX - normPosX);

    // Calcula as coordenadas da pistola girada em relação ao centro do cubo
    gunPosX = posX + gunOffsetX * cos(angle) - gunOffsetY * sin(angle);
    gunPosY = posY + gunOffsetX * sin(angle) + gunOffsetY * cos(angle);

    // Desenha a pistola
    glPushMatrix();
    glTranslatef(gunPosX, gunPosY, 0.0f);
    glRotatef(angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // Cor da pistola (branco)
    glRectf(-0.05f, -0.02f, 0.15f, 0.02f);
    glPopMatrix();
}


void drawCube() {
    glPushMatrix();
    glTranslatef(posX, posY, 0.0f);
    glColor3fv(colors[colorIndex]);
    glutSolidCube(0.2f);

    // Desenha o texto de vida acima do cubo
    glPushMatrix();
    glTranslatef(-0.17f, 0.25f, 0.0f);  // Posição do texto acima do cubo
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.0f, 1.0f, 0.0f);  // Cor do texto (branco)

    std::string lifeText = "Life: " + std::to_string(life) ;  // Exemplo de texto de vida (substitua pelo valor real)
    glRasterPos3f(0.0f, 0.0f, 0.0f);
    for (char c : lifeText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);  // Fonte do texto
    }

    glPopAttrib();
    glPopMatrix();

    glPopMatrix();
}


void moveProjectiles() {
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& projectile) {
        return !projectile.active;
    }), projectiles.end());

    for (auto& projectile : projectiles) {
        if (projectile.active) {
            // Atualiza a posi��o do proj�til na dire��o calculada
            projectile.posX += projectile.speed * projectile.dirX;
            projectile.posY += projectile.speed * projectile.dirY;

             // Verifica se o proj�til est� dentro da tela
            if (projectile.posX < -10.0f || projectile.posX > 10.0f || projectile.posY < -10.0f || projectile.posY > 10.0f) {
                // Excluir do vector
                projectile.active = false;
            }
        }
    }

}

void drawProjectiles() {
    for (const auto& projectile : projectiles) {
        if (projectile.active) {
            glColor3f(1.0f, 1.0f, 0.0f); // Cor do proj�til (amarelo)
            glRectf(projectile.posX - 0.05f, projectile.posY - 0.05f, projectile.posX + 0.05f, projectile.posY + 0.05f);
        }
    }
}

void moveCubes() {
    for (auto& cube : cubes) {
        cube.posX += cube.speed * cube.dirX;
        cube.posY += cube.speed * cube.dirY;
        GLfloat diffX = posX - cube.posX;
        GLfloat diffY = posY - cube.posY;
        GLfloat magnitude = sqrt(diffX * diffX + diffY * diffY);
        cube.dirX = diffX / magnitude;
        cube.dirY = diffY / magnitude;
    }
}

void drawCubes() {
    for (const auto& cube : cubes) {
        glPushMatrix();
        glTranslatef(cube.posX, cube.posY, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidCube(0.2f);
        glPopMatrix();
    }
}

void checkCollisions() {
    for (auto& projectile : projectiles) {
        if (projectile.active) {
            for (auto& cube : cubes) {
                GLfloat diffX = projectile.posX - cube.posX;
                GLfloat diffY = projectile.posY - cube.posY;
                GLfloat distance = sqrt(diffX * diffX + diffY * diffY);

                if (distance < 0.2f) { // Se a dist�ncia for menor que 0.2 (considerando o tamanho do cubo)
                    projectile.active = false; // Desativa o proj�til
                    cube.posX = randomFloat(); // Reposiciona o cubo aleatoriamente
                    cube.posY = randomFloat();
                    score++; // Incrementa a pontua��o
                }
            }
        }
    }
}

void initializeCubes() {
    srand(static_cast<unsigned int>(time(nullptr)));
    int numCubes = 500 + score; // Número inicial de cubos + pontuação
    for (int i = 0; i < numCubes; ++i) {
        Cube cube;
        cube.posX = randomFloat();
        cube.posY = randomFloat();
        cube.dirX = 0.0f;
        cube.dirY = 0.0f;
        cube.speed = 0.015f;
        cubes.push_back(cube);
    }
}

void resetGame() {
    // Redefina todas as variáveis e estados do jogo para seus valores iniciais
    posX = 0.0f;
    posY = 0.0f;
    // Outras variáveis do jogador e do jogo

    // Reinicialize os cubos inimigos
    //initializeCubes();

    // Reinicialize as variáveis de pontuação, vida, etc.
    score = 0;
    life = 3;

    cubes.clear();
    initializeCubes();
    // Outras ações de reinicialização, se necessário
}


void checkCollisionPlayerCube() {
    for (auto& cube : cubes) {
        GLfloat diffX = cube.posX - posX;
        GLfloat diffY = cube.posY - posY;
        GLfloat distance = sqrt(diffX * diffX + diffY * diffY);

        if (distance < 0.2f) { // Se a distância for menor que 0.2 (considerando o tamanho do cubo)
            // Colisão detectada, o player cubo perde vida
            life--; // Diminui a vida do player cubo

            // Reposiciona o cubo inimigo aleatoriamente
            cube.posX = randomFloat();
            cube.posY = randomFloat();

            // Verifica se o player cubo perdeu todas as vidas
            if (life <= 0) {
                // O player cubo perdeu todas as vidas, reinicia o jogo
                resetGame();
            }
        }
    }
}



void drawText(const std::string& text, float x, float y) {
    glRasterPos2f(x, y); // Define a posição inicial do texto

    // Desenha cada caractere do texto
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Use uma fonte bitmap (HELVETICA_12 neste exemplo)
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Posiciona a c�mera - para rodar no Windows
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f); // Rota��o em torno do eixo X
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // Rota��o em torno do eixo Y
    glTranslatef(0.0f, 0.0f, -6.0f);  // Transla��o ao longo do eixo Z negativo

    /* gluLookAt(0.0, 0.0, 5.0,  // Posi��o da c�mera
              0.0, 0.0, 0.0,  // Ponto para onde a c�mera est� olhando
              0.0, 1.0, 0.0); // Vetor de orienta��o da c�mera (eixo Y) */
   

    //glRotatef(angle, 1.0f, 1.0f, 1.0f); // Rotaciona o cubo

    // Verifica colis�es
    checkCollisions();
    checkCollisionPlayerCube();

    // Desenha os cubos inimigos
    drawCubes();
    moveCubes();

    // Desenha o cubo
    drawCube();
    moveCube();

    drawGun();

    // Desenha o tiro
    drawProjectiles();
    moveProjectiles();

    glFlush();

    // Define a posição e o conteúdo do texto da pontuação
    std::string scoreText = "POINTS: " + std::to_string(score); // Exemplo, substitua pela sua pontuação real
    float textPosX = -4.75f; // Posição X do texto no canto superior esquerdo
    float textPosY = 2.5f;  // Posição Y do texto no canto superior esquerdo

    // Desenha o texto da pontuação
    glColor3f(1.0f, 1.0f, 1.0f); // Cor do texto (branco)
    drawText(scoreText, textPosX, textPosY);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1f, 0.1f, -0.1f * height / width, 0.1f * height / width, 0.1f, 100.0f); // para rodar no Windows
    // gluPerspective(45, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}



void MouseOptions(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float dx = (float)x / (float)glutGet(GLUT_WINDOW_WIDTH) * 2.0f - 1.0f;
        float dy = 1.0f - (float)y / (float)glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;

        // Cria um novo proj�til e define a posi��o e dire��o inicial
        Projectile newProjectile;
        newProjectile.posX = gunPosX;
        newProjectile.posY = gunPosY;
        newProjectile.dirX = dx - (posX/6.25);
        newProjectile.dirY = dy - (posY/3.10);
        newProjectile.active = true;
        newProjectile.speed = 0.03f;

        // Normaliza o vetor dire��o
        GLfloat magnitude = sqrt(newProjectile.dirX * newProjectile.dirX + newProjectile.dirY * newProjectile.dirY);
        newProjectile.dirX /= magnitude;
        newProjectile.dirY /= magnitude;

        // Adiciona o proj�til � lista de proj�teis
        projectiles.push_back(newProjectile);

        /*std::cout << "Cube position: x = " << posX << ", y = " << posY << std::endl;
        std::cout << "Projectile created: posX = " << newProjectile.posX << ", posY = " << newProjectile.posY << std::endl;

        //Imprime todos os proj�teis na lista e todas as suas informa��es
        int i = 0;
        for (const auto& projectile : projectiles) {
            std::cout << "Projectile " << i++ << std::endl << "posX = " << projectile.posX << std::endl << "posY = " << projectile.posY << std::endl << "dirX = " << projectile.dirX << std::endl << "dirY = " << projectile.dirY << std::endl << "speed = " << projectile.speed << std::endl << "active = " << projectile.active << std::endl << std::endl;
        }*/
        // Mouse position
        std::cout << "Mouse position: x = " << dx << ", y = " << dy << std::endl;
        // Cubo position
        std::cout << "Cube position: x = " << posX << ", y = " << posY << std::endl;
    }

    glutPostRedisplay();
}





int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Cubo M�gico");
    srand(time(NULL));
    // Cor do cubo
    glutIdleFunc(rainbowEffect);
    // Movimento e tiro
    // Registra as fun��es de callback para as teclas pressionadas e liberadas
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(MouseOptions);
    // Chame a fun��o de temporiza��o para iniciar o controle de FPS
    glutTimerFunc(0, timer, 0);
    // Pegar as coordenadas do mouse passivamente
    glutPassiveMotionFunc(passiveMotion);
    // Aparecer na tela
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    initializeCubes();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    return 0;
}