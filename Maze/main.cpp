#include <GL/glut.h>
#include "Collision_Detection.h"
#include "Mouse_And_Keyboard_Input.h"
#include "Draw_Maze.h"

void initOpenGL() {
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);  // 백페이스 컬링 비활성화

    wallTexture = loadTexture("WallTexture01.jpg");  // 사용할 텍스처 파일 경로
    floorTexture = loadTexture("WallTexture01.jpg");  // 사용할 텍스처 파일 경로
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.0, 0.1, 100.0);  // 초기 비율을 1.0으로 설정
    glMatrixMode(GL_MODELVIEW);

    setupLighting(); // 조명 설정 호출

    // 미로 벽 콜라이더 생성
    createColliders(maze, wallSize);

    // 마우스를 화면 중앙으로 이동
    glutWarpPointer(centerX, centerY);
}

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
    glutCreateWindow("3D Maze with Collision Detection");

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
