#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include "Collision_Detection.h"
#include "Mouse_And_Keyboard_Input.h"
#include "Draw_MazeAndSOR.h"

void initOpenGL() {
    glClearColor(0.407f, 0.705f, 0.941f, 1);
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glMatrixMode(GL_MODELVIEW);
    gluPerspective(45.0, 1.0, 0.1, 100.0);  // 초기 비율을 1.0으로 설정

    wallTexture = loadTexture("Imagedata\\Walltexture.bmp");  // 사용할 텍스처 파일 경로
    floorTexture = loadTexture("Imagedata\\Floortexture.bmp");
    skyTexture = loadTexture("Imagedata\\Skytexture.jpg");
    skyTopTexture = loadTexture("Imagedata\\Skytoptexture.jpg");
    // 모델(maze[i][j] == 2 용)
    LoadOBJ("SORdata\\mesh.obj", gVertices2, gNormals2, gFaces2, gNumVertices2, gNumFaces2);
    // 모델(maze[i][j] == 3 용)
    LoadOBJ("SORdata\\mesh2.obj", gVertices3, gNormals3, gFaces3, gNumVertices3, gNumFaces3);
    // 모델(maze[i][j] == 4 용)
    LoadOBJ("SORdata\\mesh3.obj", gVertices4, gNormals4, gFaces4, gNumVertices4, gNumFaces4);
    // 모델(maze[i][j] == 5 용)
    LoadOBJ("SORdata\\mesh4.obj", gVertices5, gNormals5, gFaces5, gNumVertices5, gNumFaces5);

    // 미로 벽 콜라이더 생성
    createColliders(maze, wallSize);

    // 마우스를 화면 중앙으로 이동
    glutWarpPointer(centerX, centerY);
}

// 윈도우 크기 변경 시 호출되는 함수
void reshape(int width, int height) {
    // 화면의 비율이 변하지 않도록 설정
    glViewport(0, 0, width, height);  // 윈도우 크기 설정

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 새로운 비율에 맞는 투영 설정
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
    glutPassiveMotionFunc(mouseMotion); // 마우스 움직임 처리 등록
    glutIdleFunc(moveCameraAndPlayer);  // 부드러운 이동 처리
    glutReshapeFunc(reshape);  // 윈도우 크기 조정 시 호출될 함수 등록

    glutMainLoop();
    return 0;
}
