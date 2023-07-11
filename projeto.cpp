#include <GL/glut.h>
#include <time.h>

// Ângulo de rotação do cubo
GLfloat angle = 0.0f;
// Posição do cubo
GLfloat posX = 0.0f, posY = 0.0f;

// Cores para cada face do cubo
int color = 0;
GLfloat colors[][3] = {
    {1.0, 0.0, 0.0},  // Vermelho
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Posiciona a câmera
    gluLookAt(0.0, 0.0, 5.0,  // Posição da câmera
              0.0, 0.0, 0.0,  // Ponto para onde a câmera está olhando
              0.0, 1.0, 0.0); // Vetor de orientação da câmera (eixo Y)
    
    //glRotatef(angle, 1.0f, 1.0f, 1.0f); // Rotaciona o cubo
    
    // Desenha o cubo
    drawCube();
    
    glFlush();
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': // Move para cima
            posY += 0.1f;
            break;
        case 's': // Move para baixo
            posY -= 0.1f;
            break;
        case 'a': // Move para a esquerda
            posX -= 0.1f;
            break;
        case 'd': // Move para a direita
            posX += 0.1f;
            break;
        case 27: // Tecla Esc: fecha o programa
            exit(0);
            break;
    }
}

void MouseOptions(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON){

    }         
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Cubo com Cores");
    srand(time(NULL));
    // Cor do cubo
    glutIdleFunc(nextColor);
    // Movimento e tiro
    glutMouseFunc(MouseOptions);
    glutKeyboardFunc(keyboard);
    // Aparecer na tela
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);    
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    
    return 0;
}
