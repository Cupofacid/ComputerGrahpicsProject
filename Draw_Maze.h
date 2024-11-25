#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include "stb_image.h"

// �̷� ������: 1�� ��, 0�� ���
std::vector<std::vector<int>> maze = {
    // 32X32 ũ�� �̷�
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0},
    {1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1},
    {1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0},
    {1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0},
    {1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0},
    {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1},
    {1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
    {1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1},
    {1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1}

};

const float wallSize = 1.0f; // �̷� ũ��

// ȭ�� �߽� ��ǥ
int windowWidth = 1000, windowHeight = 800;
int centerX = windowWidth / 2;
int centerY = windowHeight / 2;

float cameraX = 1.0f, cameraY = 0.5f, cameraZ = 1.0f; // ī�޶� ��ġ
float playerX = 1.0f, playerY = 0.5f, playerZ = 1.0f; // �÷��̾� ��ü ��ġ

float yaw = 0.0f, pitch = 0.0f; // ī�޶� ȸ�� (yaw: �¿�, pitch: ����)

GLuint wallTexture; // �� �ؽ�ó ����
GLuint floorTexture; // �ٴ� �ؽ�ó ����

// �ؽ�ó�� �ε� �Լ�
GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

// �̷� �� �׸��� �Լ� (�ؽ�ó ����)
void drawWall() {
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glBegin(GL_QUADS);

    // �ո�
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

    // �޸�
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

    // ���� ��
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

    // ������ ��
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

    // ���� ��
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

    // �Ʒ��� ��
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    
    glEnd();
}

// �̷� �ٴ� �׸��� �Լ�
void drawFloor() {
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glBegin(GL_QUADS);

    // �ٴ� Ÿ�� �鿡 �ؽ�ó ����
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.0f, -0.5f);  // ���� �ϴ�
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, -0.5f);   // ������ �ϴ�
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.0f, 0.5f);    // ������ ���
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, 0.5f);   // ���� ���
   
    glEnd();
}

// ȭ�� �׸���
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.8f, 1.0f, 1);
    glLoadIdentity();

    // ī�޶� ���� ���
    float dirX = cos(pitch) * cos(yaw);
    float dirY = sin(pitch);
    float dirZ = cos(pitch) * sin(yaw);

    // ī�޶� ��ġ ����
    gluLookAt(cameraX, cameraY, cameraZ,           // ī�޶� ��ġ
        cameraX + dirX, cameraY + dirY, cameraZ + dirZ, // �ٶ󺸴� ��
        0.0f, 1.0f, 0.0f);                  // �� ����

    // �̷� �׸���
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].size(); ++j) {
            // Ÿ���� �׷����� ��ġ�� �̵�
            glPushMatrix();
            glTranslatef(j * 1.0f, 0.0f, i * 1.0f);  // 1.0f ũ�� Ÿ��
            drawFloor();
            glPopMatrix();
            if (maze[i][j] == 1) {
                glPushMatrix();
                glTranslatef(j * wallSize, wallSize / 2, i * wallSize);
                glScalef(wallSize, wallSize, wallSize);
                drawWall();
                glPopMatrix();
            }
        }
    }

    // �̷� ũ�⸸ŭ �ٴ� Ÿ���� �׸���
    //for (int i = 0; i < maze.size(); ++i) {
    //    for (int j = 0; j < maze[i].size(); ++j) {
    //        // Ÿ���� �׷����� ��ġ�� �̵�
    //        glPushMatrix();
    //        glTranslatef(j * 1.0f, 0.0f, i * 1.0f);  // 1.0f ũ�� Ÿ��
    //        drawFloor();
    //        glPopMatrix();
    //    }
    //}

    // �÷��̾� ��ü �׸���
    //glPushMatrix();
    //glTranslatef(playerX, playerY, playerZ);
    //glutSolidSphere(playerRadius, 20, 20); // ��ü�� ��ü�� �׸���
    //glPopMatrix();

    glutSwapBuffers();
}

// ���� ����
void setupLighting() {
    glEnable(GL_LIGHTING); // ���� Ȱ��ȭ
    glEnable(GL_LIGHT0);   // �⺻ ����(Light 0) Ȱ��ȭ
    glDisable(GL_LIGHTING);

    // ���� ���� ����
    GLfloat lightPos[] = { 2.0f, 4.0f, 2.0f, 1.0f };  // ���� ��ġ
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // �ֺ���
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // ���ݻ籤
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // �ݻ籤

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // ���� ����
    GLfloat matAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat matDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matShininess[] = { 50.0f }; // �ݻ� ����

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}