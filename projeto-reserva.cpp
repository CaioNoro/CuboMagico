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
int windowWidth = 1280;
int windowHeight = 720;

void resetGame();

bool buffer[256];

void keyboard(unsigned char key, int x, int y)
{
    buffer[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
    buffer[key] = false;
}

float randomFloat()
    {
        return -100.0f + (200.0f * rand()) / (RAND_MAX + 100.0f);
    }

GLfloat mouseX, mouseY;
void passiveMotion(int x, int y)
{
    // Atualiza as coordenadas do mouse
    mouseX = x;
    mouseY = y;
    std::cout << "Posicao do mouse: x = " << x << ", y = " << y << std::endl;
}

class Projectile
{
public:
    GLfloat posX;
    GLfloat posY;
    GLfloat dirX;
    GLfloat dirY;
    bool active;
    GLfloat speed;

    void moveProjectiles()
    {

        if (active)
        {
            // Atualiza a posi��o do proj�til na dire��o calculada
            posX += speed * dirX;
            posY += speed * dirY;

            // Verifica se o proj�til est� dentro da tela
            if (posX < -10.0f || posX > 10.0f || posY < -10.0f || posY > 10.0f)
            {
                // Excluir do vector
                active = false;
            }
        }
    }

    void drawProjectiles()
    {
        if (active)
        {
            glColor3f(1.0f, 1.0f, 0.0f); // Cor do proj�til (amarelo)
            glRectf(posX - 0.05f, posY - 0.05f, posX + 0.05f, posY + 0.05f);
        }
    }
};

class Projectiles
{
public:
    std::vector<Projectile> projectiles;

    void addProjectile(Projectile projectile)
    {
        projectiles.push_back(projectile);
    }

    void drawProjectiles()
    {
        for (int i = 0; i < projectiles.size(); i++)
        {
            projectiles[i].drawProjectiles();
        }
    }

    void moveProjectiles()
    {
        for (int i = 0; i < projectiles.size(); i++)
        {
            projectiles[i].moveProjectiles();
        }
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile &projectile)
                                         { return !projectile.active; }),
                          projectiles.end());
    }
};

Projectiles projectiles;

class Cube
{
public:
    GLfloat posX;
    GLfloat posY;
    GLfloat dirX;
    GLfloat dirY;
    GLfloat speed;

    Cube(float posX, float posY, float dirX, float dirY, float speed)
    {
        this->posX = posX;
        this->posY = posY;
        this->dirX = dirX;
        this->dirY = dirY;
        this->speed = speed;
    }

    void moveCubes()
    {
        posX += speed * dirX;
        posY += speed * dirY;
        GLfloat diffX = posX - posX;
        GLfloat diffY = posY - posY;
        GLfloat magnitude = sqrt(diffX * diffX + diffY * diffY);
        dirX = diffX / magnitude;
        dirY = diffY / magnitude;
    }

    void drawCubes()
    {

        glPushMatrix();
        glTranslatef(posX, posY, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidCube(0.2f);
        glPopMatrix();
    }

    void checkCollisions()
    {
        for (auto &projectile : projectiles.projectiles)
        {
            if (projectile.active)
            {

                GLfloat diffX = projectile.posX - posX;
                GLfloat diffY = projectile.posY - posY;
                GLfloat distance = sqrt(diffX * diffX + diffY * diffY);

                if (distance < 0.2f)
                {                              // Se a dist�ncia for menor que 0.2 (considerando o tamanho do cubo)
                    projectile.active = false; // Desativa o proj�til
                    posX = randomFloat();      // Reposiciona o cubo aleatoriamente
                    posY = randomFloat();
                    score++; // Incrementa a pontua��o
                }
            }
        }
    }


    void checkCollisionPlayerCube()
    {

        GLfloat diffX = posX - posX;
        GLfloat diffY = posY - posY;
        GLfloat distance = sqrt(diffX * diffX + diffY * diffY);

        if (distance < 0.2f)
        { // Se a distância for menor que 0.2 (considerando o tamanho do cubo)
            // Colisão detectada, o player cubo perde vida
            life--; // Diminui a vida do player cubo

            // Reposiciona o cubo inimigo aleatoriamente
            posX = randomFloat();
            posY = randomFloat();

            // Verifica se o player cubo perdeu todas as vidas
            if (life <= 0)
            {
                // O player cubo perdeu todas as vidas, reinicia o jogo
                resetGame();
            }
        }
    }
};

class Cubes
{
public:
    std::vector<Cube> cubes;

    void addCube(Cube cube)
    {
        cubes.push_back(cube);
    }

    void drawCubes()
    {
        for (int i = 0; i < cubes.size(); i++)
        {
            cubes[i].drawCubes();
        }
    }

    void moveCubes()
    {
        for (int i = 0; i < cubes.size(); i++)
        {
            cubes[i].moveCubes();
        }
    }

    void checkCollisions()
    {
        for (int i = 0; i < cubes.size(); i++)
        {
            cubes[i].checkCollisions();
        }
    }

    void initializeCubes()
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        int numCubes = 500 + score; // Número inicial de cubos + pontuação
        for (int i = 0; i < numCubes; ++i)
        {
            
            /* posX = randomFloat();
            posY = randomFloat();
            dirX = 0.0f;
            dirY = 0.0f;
            speed = 0.015f; */
            Cube cube(randomFloat(), randomFloat(), 0.0f, 0.0f, 0.015f);
            cubes.push_back(cube);
        }
    }

    void checkCollisionPlayerCube()
    {
        for (int i = 0; i < cubes.size(); i++)
        {
            cubes[i].checkCollisionPlayerCube();
        }
    }
};


Cubes cubes;

class Player
{
public:
    GLfloat posX;
    GLfloat posY;
    GLfloat speed;
    GLfloat size;
    GLfloat colors[6][3] = {
        {1.0f, 0.0f, 0.0f}, // Vermelho
        {1.0f, 0.5f, 0.0f}, // Laranja
        {1.0f, 1.0f, 0.0f}, // Amarelo
        {0.0f, 1.0f, 0.0f}, // Verde
        {0.0f, 0.0f, 1.0f}, // Azul
        {0.5f, 0.0f, 0.5f}  // Roxo
    };
    int life;
    int score;
    Player(GLfloat posX, GLfloat posY, GLfloat speed, GLfloat size, int life, int score)
    {
        this->posX = posX;
        this->posY = posY;
        this->speed = speed;
        this->size = size;
        this->life = life;
        this->score = score;
    }

    int colorIndex = 0;             // Índice da cor atual
    float colorChangeSpeed = 0.07f; // Velocidade de mudança de cor
    float rainbowEffectTime = 0.0f; // Tempo para controlar a transição de cores

    void rainbowEffect()
    {
        // Atualiza o tempo para controlar a transição de cores
        rainbowEffectTime += colorChangeSpeed;

        // Calcula as componentes de cor R, G e B com base no tempo e nas funções trigonométricas
        GLfloat red = 0.5f + 0.5f * sin(rainbowEffectTime);
        GLfloat green = 0.5f + 0.5f * sin(rainbowEffectTime + 2.0f * M_PI / 3.0f);
        GLfloat blue = 0.5f + 0.5f * sin(rainbowEffectTime + 4.0f * M_PI / 3.0f);

        // Define as novas cores para o cubo
        for (int i = 0; i < 6; ++i)
        {
            this->colors[i][0] = red;
            this->colors[i][1] = green;
            this->colors[i][2] = blue;
        }

        // Chama glutPostRedisplay() para redesenhar o cubo com as novas cores
        glutPostRedisplay();
    }

    void drawCube()
    {
        glPushMatrix();
        glTranslatef(posX, posY, 0.0f);
        glColor3fv(colors[colorIndex]);
        glutSolidCube(0.2f);

        // Desenha o texto de vida acima do cubo
        glPushMatrix();
        glTranslatef(-0.17f, 0.25f, 0.0f); // Posição do texto acima do cubo
        glPushAttrib(GL_CURRENT_BIT);
        glColor3f(0.0f, 1.0f, 0.0f); // Cor do texto (branco)

        std::string lifeText = "Life: " + std::to_string(life); // Exemplo de texto de vida (substitua pelo valor real)
        glRasterPos3f(0.0f, 0.0f, 0.0f);
        for (char c : lifeText)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c); // Fonte do texto
        }

        glPopAttrib();
        glPopMatrix();

        glPopMatrix();
    }

    void moveCube()
    {
        if (buffer['w'])
        { // Move para cima
            posY += 0.02f;
        }
        if (buffer['s'])
        { // Move para baixo
            posY -= 0.02f;
        }
        if (buffer['a'])
        { // Move para a esquerda
            posX -= 0.02f;
        }
        if (buffer['d'])
        { // Move para a direita
            posX += 0.02f;
        }
    }

    float gunPosX, gunPosY;
    void drawGun()
    {
        // Define as coordenadas da pistola em relação ao centro do cubo
        float gunOffsetX = 0.15f;
        float gunOffsetY = 0.0f;

        // Obtém as coordenadas do mouse normalizadas
        float mousePosX = static_cast<float>(mouseX) / static_cast<float>(windowWidth) * 2.0f - 1.0f;
        float mousePosY = 1.0f - static_cast<float>(mouseY) / static_cast<float>(windowHeight) * 2.0f;

        // Calcula o ângulo entre a pistola e a posição do mouse
        GLfloat normPosX = (posX / 6.25);
        GLfloat normPosY = (posY / 3.10);

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

    void shoot(int x, int y)
    {
        float dx = (float)x / (float)glutGet(GLUT_WINDOW_WIDTH) * 2.0f - 1.0f;
        float dy = 1.0f - (float)y / (float)glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;

        // Cria um novo projetil e define a posicao e direcao inicial
        Projectile newProjectile;
        newProjectile.posX = gunPosX;
        newProjectile.posY = gunPosY;
        newProjectile.dirX = dx - (posX / 6.25);
        newProjectile.dirY = dy - (posY / 3.10);
        newProjectile.active = true;
        newProjectile.speed = 0.03f;

        // Normaliza o vetor direcao
        GLfloat magnitude = sqrt(newProjectile.dirX * newProjectile.dirX + newProjectile.dirY * newProjectile.dirY);
        newProjectile.dirX /= magnitude;
        newProjectile.dirY /= magnitude;

        // Adiciona o projetil na lista de projeteis
        projectiles.addProjectile(newProjectile);
    }
};

Player player(0.0f, 0.0f, 0.05f, 0.1f, 3, 0);

void resetGame()
{
    // Redefina todas as variáveis e estados do jogo para seus valores iniciais
    player.posX = 0.0f;
    player.posY = 0.0f;
    // Outras variáveis do jogador e do jogo

    // Reinicialize os cubos inimigos
    // initializeCubes();

    // Reinicialize as variáveis de pontuação, vida, etc.
    score = 0;
    life = 3;

    cubes.cubes.clear();
    cubes.initializeCubes();
    // Outras ações de reinicialização, se necessário
}

// Vari�vel para controlar o tempo de atualiza��o
int previousTime = 0;

// Fun��o de temporiza��o para limitar o FPS
void timer(int value)
{
    // Obt�m o tempo atual em milissegundos
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    // Calcula o tempo decorrido desde a �ltima atualiza��o
    float deltaTime = (currentTime - previousTime) / 1000.0f; // Converter para segundos

    // Se passou menos de 1/60 segundos (16.67 ms), aguarda o tempo restante
    if (deltaTime < 1.0f / 60.0f)
    {
        int sleepTime = ((1.0f / 60.0f) - deltaTime) * 1000;
        if (sleepTime > 0)
        {
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

// Posi��o do cubo
GLfloat posX = 0.0f, posY = 0.0f;

/* float randomFloat()
{
    return -100.0f + (200.0f * rand()) / (RAND_MAX + 100.0f);
}

void moveCubes()
{
    for (auto &cube : cubes)
    {
        cube.posX += cube.speed * cube.dirX;
        cube.posY += cube.speed * cube.dirY;
        GLfloat diffX = posX - cube.posX;
        GLfloat diffY = posY - cube.posY;
        GLfloat magnitude = sqrt(diffX * diffX + diffY * diffY);
        cube.dirX = diffX / magnitude;
        cube.dirY = diffY / magnitude;
    }
}

void drawCubes()
{
    for (const auto &cube : cubes)
    {
        glPushMatrix();
        glTranslatef(cube.posX, cube.posY, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidCube(0.2f);
        glPopMatrix();
    }
}

void checkCollisions()
{
    for (auto &projectile : projectiles.projectiles)
    {
        if (projectile.active)
        {
            for (auto &cube : cubes)
            {
                GLfloat diffX = projectile.posX - cube.posX;
                GLfloat diffY = projectile.posY - cube.posY;
                GLfloat distance = sqrt(diffX * diffX + diffY * diffY);

                if (distance < 0.2f)
                {                              // Se a dist�ncia for menor que 0.2 (considerando o tamanho do cubo)
                    projectile.active = false; // Desativa o proj�til
                    cube.posX = randomFloat(); // Reposiciona o cubo aleatoriamente
                    cube.posY = randomFloat();
                    score++; // Incrementa a pontua��o
                }
            }
        }
    }
}

void initializeCubes()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    int numCubes = 500 + score; // Número inicial de cubos + pontuação
    for (int i = 0; i < numCubes; ++i)
    {
        Cube cube;
        cube.posX = randomFloat();
        cube.posY = randomFloat();
        cube.dirX = 0.0f;
        cube.dirY = 0.0f;
        cube.speed = 0.015f;
        cubes.push_back(cube);
    }
}

void checkCollisionPlayerCube()
{
    for (auto &cube : cubes)
    {
        GLfloat diffX = cube.posX - posX;
        GLfloat diffY = cube.posY - posY;
        GLfloat distance = sqrt(diffX * diffX + diffY * diffY);

        if (distance < 0.2f)
        { // Se a distância for menor que 0.2 (considerando o tamanho do cubo)
            // Colisão detectada, o player cubo perde vida
            life--; // Diminui a vida do player cubo

            // Reposiciona o cubo inimigo aleatoriamente
            cube.posX = randomFloat();
            cube.posY = randomFloat();

            // Verifica se o player cubo perdeu todas as vidas
            if (life <= 0)
            {
                // O player cubo perdeu todas as vidas, reinicia o jogo
                resetGame();
            }
        }
    }
} */



void drawText(const std::string &text, float x, float y)
{
    glRasterPos2f(x, y); // Define a posição inicial do texto

    // Desenha cada caractere do texto
    for (char c : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Use uma fonte bitmap (HELVETICA_12 neste exemplo)
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Posiciona a c�mera - para rodar no Windows
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f); // Rota��o em torno do eixo X
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // Rota��o em torno do eixo Y
    glTranslatef(0.0f, 0.0f, -6.0f);   // Transla��o ao longo do eixo Z negativo

    // Verifica colis�es
    /* checkCollisions();
    checkCollisionPlayerCube();

    // Desenha os cubos inimigos
    drawCubes();
    moveCubes(); */
    cubes.checkCollisionPlayerCube();
    cubes.checkCollisions();
    cubes.drawCubes();
    cubes.moveCubes();

    // Desenha o cubo
    player.drawCube();
    player.moveCube();
    // drawCube();
    // moveCube();

    player.drawGun();
    // drawGun();

    // Desenha o tiro
    /* drawProjectiles();
    moveProjectiles(); */

    glFlush();

    // Define a posição e o conteúdo do texto da pontuação
    std::string scoreText = "POINTS: " + std::to_string(score);
    float textPosX = -4.75f;
    float textPosY = 2.5f;
    // Desenha o texto da pontuação
    glColor3f(1.0f, 1.0f, 1.0f); // Cor do texto (branco)
    drawText(scoreText, textPosX, textPosY);

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1f, 0.1f, -0.1f * height / width, 0.1f * height / width, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void MouseOptions(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        player.shoot(x, y);
    }

    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Cubo Magico");
    srand(time(NULL));
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(MouseOptions);
    glutTimerFunc(0, timer, 0);
    glutPassiveMotionFunc(passiveMotion);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    cubes.initializeCubes();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}