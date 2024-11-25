#include <GL/glut.h>
#include "Collision_Detection.h"
#include "Mouse_And_Keyboard_Input.h"
#include "Draw_Maze.h"

void initOpenGL() {
    glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);  // �����̽� �ø� ��Ȱ��ȭ

    wallTexture = loadTexture("WallTexture01.jpg");  // ����� �ؽ�ó ���� ���
    floorTexture = loadTexture("WallTexture01.jpg");  // ����� �ؽ�ó ���� ���
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    setupLighting(); // ���� ���� ȣ��

    // �̷� �� �ݶ��̴� ����
    createColliders(maze, wallSize);

    // ���콺�� ȭ�� �߾����� �̵�
    glutWarpPointer(centerX, centerY);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("3D Maze with Collision Detection");

    initOpenGL();

    glDisable(GL_CULL_FACE);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion); // ���콺 ������ ó�� ���
    glutIdleFunc(moveCameraAndPlayer);  // �ε巯�� �̵� ó��

    glutMainLoop();
    return 0;
}