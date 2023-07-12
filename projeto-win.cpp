#include <GL/glut.h>
#include <time.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <algorithm>

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

void passiveMotion(int x, int y) {
    // Normaliza as coordenadas do mouse para o intervalo entre -1 e 1
    float normalizedX = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2.0f - 1.0f;
    float normalizedY = 1.0f - (float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;

    //std::cout << "Posi��o do mouse (normalizada): x = " << normalizedX << ", y = " << normalizedY << std::endl;
}

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
int color = 0;
GLfloat colors[][3] = {
    {0.0, 1.0, 0.0},  // Verde
    {0.0, 0.0, 1.0},  // Azul
    {1.0, 1.0, 0.0},  // Amarelo
    {1.0, 0.0, 1.0},  // Magenta
    {0.0, 1.0, 1.0}   // Ciano
};

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
        posY += 0.05f;
    }
    if (buffer['s']) { // Move para baixo
        posY -= 0.05f;
    }
    if (buffer['a']) { // Move para a esquerda
        posX -= 0.05f;
    }
    if (buffer['d']) { // Move para a direita
        posX += 0.05f;
    }
}

void drawCube() {
    glBegin(GL_QUADS);
        glColor3fv(colors[color]);
        glVertex2f(posX, posY);
        glVertex2f(posX + 0.2f, posY);
        glVertex2f(posX + 0.2f, posY + 0.2f);
        glVertex2f(posX, posY + 0.2f);
    glEnd();
}

void nextColor(){
    if(randomFloat() > 98.5){
        color++;
        if(color >= 6)
            color = 0;
    }
    glutPostRedisplay();
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
                }
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Posiciona a c�mera - para rodar no Windows
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f); // Rota��o em torno do eixo X
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // Rota��o em torno do eixo Y
    glTranslatef(0.0f, 0.0f, -5.0f);  // Transla��o ao longo do eixo Z negativo

    /*gluLookAt(0.0, 0.0, 5.0,  // Posi��o da c�mera
              0.0, 0.0, 0.0,  // Ponto para onde a c�mera est� olhando
              0.0, 1.0, 0.0); // Vetor de orienta��o da c�mera (eixo Y)
    */

    //glRotatef(angle, 1.0f, 1.0f, 1.0f); // Rotaciona o cubo

    // Verifica colis�es
    checkCollisions();

    // Desenha os cubos inimigos
    drawCubes();
    moveCubes();

    // Desenha o cubo
    drawCube();
    moveCube();

    // Desenha o tiro
    drawProjectiles();
    moveProjectiles();

    glFlush();
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

void initializeCubes() {
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < 1000; ++i) {
        Cube cube;
        cube.posX = randomFloat();
        cube.posY = randomFloat();
        cube.dirX = 0.0f;
        cube.dirY = 0.0f;
        cube.speed = 0.02f;
        cubes.push_back(cube);
    }
}

void MouseOptions(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float dx = (float)x / (float)glutGet(GLUT_WINDOW_WIDTH) * 2.0f - 1.0f;
        float dy = 1.0f - (float)y / (float)glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;

        // Cria um novo proj�til e define a posi��o e dire��o inicial
        Projectile newProjectile;
        newProjectile.posX = posX;
        newProjectile.posY = posY;
        newProjectile.dirX = dx - (posX/2);
        newProjectile.dirY = dy - (posY/2);
        newProjectile.active = true;
        newProjectile.speed = 0.02f;

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
        }
        // Mouse position
        std::cout << "Mouse position: x = " << dx << ", y = " << dy << std::endl;
        // Cubo position
        std::cout << "Cube position: x = " << posX << ", y = " << posY << std::endl;*/
    }

    glutPostRedisplay();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Cubo M�gico");
    srand(time(NULL));
    // Cor do cubo
    glutIdleFunc(nextColor);
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

