#pragma once
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include "Mouse_And_Keyboard_Input.h"
#include "Draw_Maze.h"

const float playerRadius = 0.1f;  // 플레이어의 반지름 (조정 가능)

// Collider 구조체
struct Collider {
    float x, y, z;  // 벽의 중심 좌표
    float size;     // 벽의 크기 (정육면체)

    // 충돌 검사 함수
    bool checkCollision(float px, float py, float pz, float radius) {
        // 벽의 크기와 플레이어 반지름을 고려한 AABB 충돌 검사
        bool collisionX = px + radius > x - size / 2 && px - radius < x + size / 2;
        bool collisionY = py + radius > y - size / 2 && py - radius < y + size / 2;
        bool collisionZ = pz + radius > z - size / 2 && pz - radius < z + size / 2;

        return collisionX && collisionY && collisionZ;
    }
};

// 벽들의 콜라이더 리스트
std::vector<Collider> colliders;

bool isPlayerColliding(float newX, float newZ, float playerRadius) {
    // 플레이어의 y좌표는 고정되어 있으므로 x, z만 비교
    for (const auto& collider : colliders) {
        // 벽의 크기와 플레이어의 반지름을 고려한 AABB 충돌 검사
        bool collisionX = newX + playerRadius > collider.x - collider.size / 2 && newX - playerRadius < collider.x + collider.size / 2;
        bool collisionZ = newZ + playerRadius > collider.z - collider.size / 2 && newZ - playerRadius < collider.z + collider.size / 2;

        // x와 z 좌표가 모두 충돌 범위 안에 있으면 충돌
        if (collisionX && collisionZ) {
            return true; // 충돌이 발생하면 true 반환
        }
    }
    return false; // 충돌이 없으면 false 반환
}

// 미로에 벽의 콜라이더를 추가
void createColliders(std::vector<std::vector<int>> maze, float wallSize) {
    colliders.clear();
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].size(); ++j) {
            if (maze[i][j] == 1) { // 벽이면
                // 벽의 중앙 좌표와 크기를 설정
                float x = j * wallSize;
                float z = i * wallSize;
                colliders.push_back(Collider{ x, wallSize / 2, z, wallSize });
            }
        }
    }
}

void moveCameraAndPlayer() {
    const float moveSpeed = 0.05f; // 이동 속도 설정
    const float rotationSpeed = 0.05f;  // 회전 속도 설정

    // 카메라 방향 벡터 계산
    float forwardX = cos(pitch) * cos(yaw);
    float forwardZ = cos(pitch) * sin(yaw);
    float rightX = sin(yaw);
    float rightZ = -cos(yaw);

    // 예상 위치 계산
    float newCameraX = cameraX;
    float newCameraZ = cameraZ;
    float newPlayerX = playerX;
    float newPlayerZ = playerZ;

    // 전진
    if (keyStates['w']) {
        newCameraX = cameraX + moveSpeed * forwardX;
        newCameraZ = cameraZ + moveSpeed * forwardZ;
        newPlayerX = playerX + moveSpeed * forwardX;
        newPlayerZ = playerZ + moveSpeed * forwardZ;

        // 충돌이 없으면 이동
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // 후진
    if (keyStates['s']) {
        newCameraX = cameraX - moveSpeed * forwardX;
        newCameraZ = cameraZ - moveSpeed * forwardZ;
        newPlayerX = playerX - moveSpeed * forwardX;
        newPlayerZ = playerZ - moveSpeed * forwardZ;

        // 충돌이 없으면 이동
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // 오른쪽 이동
    if (keyStates['a']) {
        newCameraX = cameraX + moveSpeed * rightX;
        newCameraZ = cameraZ + moveSpeed * rightZ;
        newPlayerX = playerX + moveSpeed * rightX;
        newPlayerZ = playerZ + moveSpeed * rightZ;

        // 충돌이 없으면 이동
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // 왼쪽 이동
    if (keyStates['d']) {
        newCameraX = cameraX - moveSpeed * rightX;
        newCameraZ = cameraZ - moveSpeed * rightZ;
        newPlayerX = playerX - moveSpeed * rightX;
        newPlayerZ = playerZ - moveSpeed * rightZ;

        // 충돌이 없으면 이동
        if (!isPlayerColliding(newPlayerX, newPlayerZ, playerRadius)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
            playerX = newPlayerX;
            playerZ = newPlayerZ;
        }
    }

    // 'q' 키를 눌렀을 때 왼쪽 회전
    if (keyStates['q']) {
        yaw -= rotationSpeed;  // 왼쪽으로 회전
    }

    // 'e' 키를 눌렀을 때 오른쪽 회전
    if (keyStates['e']) {
        yaw += rotationSpeed;  // 오른쪽으로 회전
    }

    glutPostRedisplay(); // 화면 갱신
}




