#pragma once
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include "Draw_Maze.h"

bool keyStates[256] = { false }; // 키 상태 추적

// 키보드 눌림 처리
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC 키로 종료
        exit(0);
    }
    keyStates[key] = true; // 키 눌림 상태 저장
}

// 키보드 떼기 처리
void keyboardUp(unsigned char key, int x, int y) {
    keyStates[key] = false; // 키 떼기 상태 저장
}

// 마우스 움직임 처리
void mouseMotion(int x, int y) {
    // 중앙에서의 이동량 계산
    int dx = x - centerX;
    int dy = y - centerY;

    // 카메라 회전 업데이트
    const float sensitivity = 0.002f; // 민감도
    yaw += dx * sensitivity;
    pitch -= dy * sensitivity;

    // pitch 제한 (상하 회전 제한)
    if (pitch < -1.5f) pitch = -1.5f; // 약 -85도

    // 마우스를 다시 화면 중앙으로 이동
    glutWarpPointer(centerX, centerY);

    glutPostRedisplay();
}

