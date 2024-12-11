#pragma once
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include "Draw_MazeAndSOR.h"

bool keyStates[256] = { false }; // Ű ���� ����

// Ű���� ���� ó��
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC Ű�� ����
        exit(0);
    }
    else if (currentGameState == GAME_DESCRIPTION) {
        if (key == 13) { // Enter key (ASCII 13)
            currentGameState = GAME_RUNNING; // ���� ��ȯ
        }
    }
    keyStates[key] = true; // Ű ���� ���� ����

}

// Ű���� ���� ó��
void keyboardUp(unsigned char key, int x, int y) {
    keyStates[key] = false; // Ű ���� ���� ����
}

// ���콺 ������ ó��
void mouseMotion(int x, int y) {
    // �߾ӿ����� �̵��� ���
    int dx = x - centerX;
    int dy = y - centerY;

    // ī�޶� ȸ�� ������Ʈ
    const float sensitivity = 0.002f; // �ΰ���
    yaw += dx * sensitivity;
    pitch -= dy * sensitivity;

    // pitch ���� (���� ȸ�� ����)
    if (pitch < -1.5f) pitch = -1.5f; // �� -85��

    // ���콺�� �ٽ� ȭ�� �߾����� �̵�
    glutWarpPointer(centerX, centerY);

    glutPostRedisplay();
}

