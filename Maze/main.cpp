#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include "Collision_Detection.h"
#include "Mouse_And_Keyboard_Input.h"
#include "Draw_MazeAndSOR.h"

void initOpenGL() {
    glClearColor(0.407f, 0.705f, 0.941f, 1);
    glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glMatrixMode(GL_MODELVIEW);
    gluPerspective(45.0, 1.0, 0.1, 100.0);  // �ʱ� ������ 1.0���� ����

    wallTexture = loadTexture("Imagedata\\Walltexture.bmp");  // ����� �ؽ�ó ���� ���
    floorTexture = loadTexture("Imagedata\\Floortexture.bmp");
    skyTexture = loadTexture("Imagedata\\Skytexture.jpg");
    skyTopTexture = loadTexture("Imagedata\\Skytoptexture.jpg");
    // ��(maze[i][j] == 2 ��)
    LoadOBJ("SORdata\\mesh.obj", gVertices2, gNormals2, gFaces2, gNumVertices2, gNumFaces2);
    // ��(maze[i][j] == 3 ��)
    LoadOBJ("SORdata\\mesh2.obj", gVertices3, gNormals3, gFaces3, gNumVertices3, gNumFaces3);
    // ��(maze[i][j] == 4 ��)
    LoadOBJ("SORdata\\mesh3.obj", gVertices4, gNormals4, gFaces4, gNumVertices4, gNumFaces4);
    // ��(maze[i][j] == 5 ��)
    LoadOBJ("SORdata\\mesh4.obj", gVertices5, gNormals5, gFaces5, gNumVertices5, gNumFaces5);

    // �̷� �� �ݶ��̴� ����
    createColliders(maze, wallSize);

    // ���콺�� ȭ�� �߾����� �̵�
    glutWarpPointer(centerX, centerY);
}

// ������ ũ�� ���� �� ȣ��Ǵ� �Լ�
void reshape(int width, int height) {
    // ȭ���� ������ ������ �ʵ��� ����
    glViewport(0, 0, width, height);  // ������ ũ�� ����

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // ���ο� ������ �´� ���� ����
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    gluPerspective(45.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("3D Maze Experiment");

    initOpenGL();

    glDisable(GL_CULL_FACE);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion); // ���콺 ������ ó�� ���
    glutIdleFunc(moveCameraAndPlayer);  // �ε巯�� �̵� ó��
    glutReshapeFunc(reshape);  // ������ ũ�� ���� �� ȣ��� �Լ� ���

    glutMainLoop();
    return 0;
}
