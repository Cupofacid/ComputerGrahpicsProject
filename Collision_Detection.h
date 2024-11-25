#pragma once
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include "Mouse_And_Keyboard_Input.h"
#include "Draw_Maze.h"

const float playerRadius = 0.1f;  // �÷��̾��� ������ (���� ����)

// Collider ����ü
struct Collider {
    float x, y, z;  // ���� �߽� ��ǥ
    float size;     // ���� ũ�� (������ü)

    // �浹 �˻� �Լ�
    bool checkCollision(float px, float py, float pz, float radius) {
        // ���� ũ��� �÷��̾� �������� ����� AABB �浹 �˻�
        bool collisionX = px + radius > x - size / 2 && px - radius < x + size / 2;
        bool collisionY = py + radius > y - size / 2 && py - radius < y + size / 2;
        bool collisionZ = pz + radius > z - size / 2 && pz - radius < z + size / 2;

        return collisionX && collisionY && collisionZ;
    }
};

// ������ �ݶ��̴� ����Ʈ
std::vector<Collider> colliders;

bool isPlayerColliding(float newX, float newZ, float playerRadius) {
    // �÷��̾��� y��ǥ�� �����Ǿ� �����Ƿ� x, z�� ��
    for (const auto& collider : colliders) {
        // ���� ũ��� �÷��̾��� �������� ����� AABB �浹 �˻�
        bool collisionX = newX + playerRadius > collider.x - collider.size / 2 && newX - playerRadius < collider.x + collider.size / 2;
        bool collisionZ = newZ + playerRadius > collider.z - collider.size / 2 && newZ - playerRadius < collider.z + collider.size / 2;

        // x�� z ��ǥ�� ��� �浹 ���� �ȿ� ������ �浹
        if (collisionX && collisionZ) {
            return true; // �浹�� �߻��ϸ� true ��ȯ
        }
    }
    return false; // �浹�� ������ false ��ȯ
}

// �̷ο� ���� �ݶ��̴��� �߰�
void createColliders(std::vector<std::vector<int>> maze, float wallSize) {
    colliders.clear();
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].size(); ++j) {
            if (maze[i][j] == 1) { // ���̸�
                // ���� �߾� ��ǥ�� ũ�⸦ ����
                float x = j * wallSize;
                float z = i * wallSize;
                colliders.push_back(Collider{ x, wallSize / 2, z, wallSize });
            }
        }
    }
}

void moveCameraAndPlayer() {
    const float moveSpeed = 0.05f; // �̵� �ӵ� ����
    const float rotationSpeed = 0.05f;  // ȸ�� �ӵ� ����

    // ī�޶� ���� ���� ���
    float forwardX = cos(pitch) * cos(yaw);
    float forwardZ = cos(pitch) * sin(yaw);
    float rightX = sin(yaw);
    float rightZ = -cos(yaw);

    // ���� ��ġ ���
    float newCameraX = cameraX;
    float newCameraZ = cameraZ;
    float newPlayerX = playerX;
    float newPlayerZ = playerZ;

    // ����
    if (keyStates['w']) {
        newCameraX = cameraX + moveSpeed * forwardX;
        newCameraZ = cameraZ + moveSpeed * forwardZ;
        newPlayerX = playerX + moveSpeed * forwardX;
        newPlayerZ = playerZ + moveSpeed * forwardZ;

        // �浹�� ������ �̵�
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // ����
    if (keyStates['s']) {
        newCameraX = cameraX - moveSpeed * forwardX;
        newCameraZ = cameraZ - moveSpeed * forwardZ;
        newPlayerX = playerX - moveSpeed * forwardX;
        newPlayerZ = playerZ - moveSpeed * forwardZ;

        // �浹�� ������ �̵�
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // ������ �̵�
    if (keyStates['a']) {
        newCameraX = cameraX + moveSpeed * rightX;
        newCameraZ = cameraZ + moveSpeed * rightZ;
        newPlayerX = playerX + moveSpeed * rightX;
        newPlayerZ = playerZ + moveSpeed * rightZ;

        // �浹�� ������ �̵�
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // ���� �̵�
    if (keyStates['d']) {
        newCameraX = cameraX - moveSpeed * rightX;
        newCameraZ = cameraZ - moveSpeed * rightZ;
        newPlayerX = playerX - moveSpeed * rightX;
        newPlayerZ = playerZ - moveSpeed * rightZ;

        // �浹�� ������ �̵�
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // 'q' Ű�� ������ �� ���� ȸ��
    if (keyStates['q']) {
        yaw -= rotationSpeed;  // �������� ȸ��
    }

    // 'e' Ű�� ������ �� ������ ȸ��
    if (keyStates['e']) {
        yaw += rotationSpeed;  // ���������� ȸ��
    }

    glutPostRedisplay(); // ȭ�� ����
}




