#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include "stb_image.h"

// �̷� ������: 1�� ��, 0�� ���
std::vector<std::vector<int>> maze = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 3, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 3, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 1, 3, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 1, 4, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const float wallSize = 1.0f; // �̷� ũ��

// ȭ�� �߽� ��ǥ
int windowWidth = 2000, windowHeight = 1200;
int centerX = windowWidth / 2;
int centerY = windowHeight / 2;

float cameraX = 1.0f, cameraY = 0.5f, cameraZ = 1.0f; // ī�޶� ��ġ
float playerX = 1.0f, playerY = 0.5f, playerZ = 1.0f; // �÷��̾� ��ü ��ġ

float yaw = 0.0f, pitch = 0.0f; // ī�޶� ȸ�� (yaw: �¿�, pitch: ����)

GLuint wallTexture;
GLuint floorTexture;
GLuint skyTexture;

static float angleX = 0.0f;
static float angleY = 0.0f;
static int mousePrevX, mousePrevY;
static int mouseLeftDown = 0;

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v[3];
} Face;

static Vertex* gVertices = NULL;
static Vertex* gNormals = NULL;
static Face* gFaces = NULL;
static int gNumVertices = 0;
static int gNumFaces = 0;

// �߰� �𵨿� ���� ����
static Vertex* gVertices2 = NULL;
static Vertex* gNormals2 = NULL;
static Face* gFaces2 = NULL;
static int gNumVertices2 = 0;
static int gNumFaces2 = 0;

// ���� ���� �Լ�
void VectorAdd(Vertex* a, Vertex* b, Vertex* result) {
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}
void VectorNormalize(Vertex* v) {
    float length = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (length > 1e-8) {
        v->x /= length;
        v->y /= length;
        v->z /= length;
    }
}
void VectorCross(Vertex* a, Vertex* b, Vertex* c, Vertex* out) {
    // out = (b-a) x (c-a)
    Vertex ab, ac;
    ab.x = b->x - a->x;
    ab.y = b->y - a->y;
    ab.z = b->z - a->z;

    ac.x = c->x - a->x;
    ac.y = c->y - a->y;
    ac.z = c->z - a->z;

    out->x = ab.y * ac.z - ab.z * ac.y;
    out->y = ab.z * ac.x - ab.x * ac.z;
    out->z = ab.x * ac.y - ab.y * ac.x;
}

int LoadOBJ(const char* filename,
    Vertex*& vertices, Vertex*& normals, Face*& faces,
    int& numVertices, int& numFaces) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "���� ���� ����: %s\n", filename);
        return 0;
    }

    int vCount = 0;
    int fCount = 0;

    char line[1024];
    int totalVertices = 0, totalFaces = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == 'v' && line[1] == ' ') {
            totalVertices++;
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            totalFaces++;
        }
    }

    if (totalVertices == 0 || totalFaces == 0) {
        fclose(fp);
        fprintf(stderr, "OBJ ���Ͽ� ���� �Ǵ� �� ������ �����ϴ�.\n");
        return 0;
    }

    fseek(fp, 0, SEEK_SET);

    vertices = (Vertex*)malloc(sizeof(Vertex) * totalVertices);
    normals = (Vertex*)malloc(sizeof(Vertex) * totalVertices);
    faces = (Face*)malloc(sizeof(Face) * totalFaces);
    int* vertexFaceCount = (int*)calloc(totalVertices, sizeof(int));

    if (!vertices || !normals || !faces || !vertexFaceCount) {
        fclose(fp);
        fprintf(stderr, "�޸� �Ҵ� ����\n");
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == 'v' && line[1] == ' ') {
            Vertex vert;
            if (sscanf(line, "v %f %f %f", &vert.x, &vert.y, &vert.z) == 3) {
                vertices[vCount++] = vert;
            }
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            int v1, v2, v3;
            if (sscanf(line, "f %d %d %d", &v1, &v2, &v3) == 3) {
                if (v1 >= 1 && v1 <= totalVertices &&
                    v2 >= 1 && v2 <= totalVertices &&
                    v3 >= 1 && v3 <= totalVertices) {
                    faces[fCount].v[0] = v1;
                    faces[fCount].v[1] = v2;
                    faces[fCount].v[2] = v3;
                    fCount++;
                }
            }
        }
    }

    fclose(fp);

    numVertices = totalVertices;
    numFaces = fCount;
    for (int i = 0; i < numVertices; i++) {
        normals[i].x = normals[i].y = normals[i].z = 0.0f;
    }

    for (int i = 0; i < numFaces; i++) {
        Face f = faces[i];
        int v1 = f.v[0] - 1;
        int v2 = f.v[1] - 1;
        int v3 = f.v[2] - 1;
        Vertex* A = &vertices[v1];
        Vertex* B = &vertices[v2];
        Vertex* C = &vertices[v3];

        Vertex faceNormal;
        VectorCross(A, B, C, &faceNormal);
        VectorNormalize(&faceNormal);

        normals[v1].x += faceNormal.x; normals[v1].y += faceNormal.y; normals[v1].z += faceNormal.z; vertexFaceCount[v1]++;
        normals[v2].x += faceNormal.x; normals[v2].y += faceNormal.y; normals[v2].z += faceNormal.z; vertexFaceCount[v2]++;
        normals[v3].x += faceNormal.x; normals[v3].y += faceNormal.y; normals[v3].z += faceNormal.z; vertexFaceCount[v3]++;
    }

    for (int i = 0; i < numVertices; i++) {
        if (vertexFaceCount[i] > 0) {
            normals[i].x /= (float)vertexFaceCount[i];
            normals[i].y /= (float)vertexFaceCount[i];
            normals[i].z /= (float)vertexFaceCount[i];
            VectorNormalize(&normals[i]);
        }
    }

    free(vertexFaceCount);

    fprintf(stdout, "OBJ �ε� �Ϸ�: %s ���� %d��, �� %d��\n", filename, numVertices, numFaces);
    return 1;
}

void DrawOBJShaded(Vertex* vertices, Vertex* normals, Face* faces, int numFaces) {
    // ���� �� ���� ������ �����ϰ� ���
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat matAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matSpe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matShn[] = { 50.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpe);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShn);

    GLfloat lightPos[] = { 5.0f, 10.0f, 5.0f, 1.0f };
    GLfloat lightAmb[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
    GLfloat lightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };  
    GLfloat lightSpe[] = { 1.0f, 1.0f, 1.0f, 1.0f };  
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpe);

    if (!vertices || !normals || !faces || numFaces <= 0) {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
        return;
    }

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numFaces; i++) {
        Face f = faces[i];
        for (int j = 0; j < 3; j++) {
            int v_idx = f.v[j] - 1;
            glNormal3f(normals[v_idx].x, normals[v_idx].y, normals[v_idx].z);
            glVertex3f(vertices[v_idx].x, vertices[v_idx].y, vertices[v_idx].z);
        }
    }
    glEnd();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

void DrawOBJWireframe(Vertex* vertices, Face* faces, int numFaces) {
    // ���̾��������� ���� �����̳� ������ ���� �ܼ� �����θ� ǥ��
    // �ʿ�� glDisable(GL_LIGHTING) ���� ���� ���� ��Ȱ��ȭ
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    // ������ ���� (�ʿ��� ���)
    glColor3f(1.0f, 1.0f, 1.0f);

    if (!vertices || !faces || numFaces <= 0) {
        return;
    }

    // �� ���� GL_LINE_LOOP�� �׷��� ���̾������� ȿ���� ����.
    for (int i = 0; i < numFaces; i++) {
        Face f = faces[i];
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 3; j++) {
            int v_idx = f.v[j] - 1;
            glVertex3f(vertices[v_idx].x, vertices[v_idx].y, vertices[v_idx].z);
        }
        glEnd();
    }

    // �ʿ信 ���� �ٽ� ���� ���� Ȱ��ȭ�� �� ������, 
    // ���⼭�� ���̾������� �뵵�̹Ƿ� ��Ȱ�� ���� ����
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
}

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

void drawSky() {
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    glBegin(GL_QUADS);

    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, 25.0f, -50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, 25.0f, -50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, -25.0f, -50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, -25.0f, -50.0f);

    // Back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, 25.0f, 50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, 25.0f, 50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, -25.0f, 50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, -25.0f, 50.0f);

    // Right face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(50.0f, 25.0f, -50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, 25.0f, 50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, -25.0f, 50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(50.0f, -25.0f, -50.0f);

    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, 25.0f, -50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-50.0f, 25.0f, 50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-50.0f, -25.0f, 50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, -25.0f, -50.0f);

    glEnd();
}

// ���� ������ �� ���¸� ������ 2���� �迭�� ����(�̷� ũ�⿡ �°�)
static bool modelTriggered[10][10]; // �̷ΰ� 10x10�� ���
// ��� ��Ұ� false�� �ʱ�ȭ�ȴٰ� ����(���� ���� �ʱ�ȭ ��Ģ�� ����)

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.407f, 0.705f, 0.941f, 1);
    glLoadIdentity();

    float dirX = cos(pitch) * cos(yaw);
    float dirY = sin(pitch);
    float dirZ = cos(pitch) * sin(yaw);

    gluLookAt(cameraX, cameraY, cameraZ,
        cameraX + dirX, cameraY + dirY, cameraZ + dirZ,
        0.0f, 1.0f, 0.0f);

    // �÷��̾ ���� ��ġ�� �� �ε��� ���
    int playerCellX = (int)floor(playerX);
    int playerCellZ = (int)floor(playerZ);

    for (int i = 0; i < (int)maze.size(); ++i) {
        for (int j = 0; j < (int)maze[i].size(); ++j) {
            // �ٴ� �׸���
            glPushMatrix();
            glTranslatef(j * 1.0f, 0.0f, i * 1.0f);
            drawFloor();
            glPopMatrix();

            if (maze[i][j] == 1) {
                // ��
                glPushMatrix();
                glTranslatef(j * wallSize, wallSize / 2, i * wallSize);
                glScalef(wallSize, wallSize, wallSize);
                drawWall();
                glPopMatrix();
            }
            else if (maze[i][j] == 3) {
                float cellCenterX = j - 0.2f;
                float cellCenterZ = i + 0.1f;
                float dx = playerX - cellCenterX;
                float dz = playerZ - cellCenterZ;
                float dist = sqrtf(dx * dx + dz * dz);
                bool playerHere = (dist <= 1.0f); // 1.0f �ݰ� ��

                // �÷��̾ �����ߴٸ� modelTriggered�� true�� ����
                if (playerHere && !modelTriggered[i][j]) {
                    modelTriggered[i][j] = true;
                }

                glPushMatrix();
                glTranslatef(j * wallSize, 0.4f, i * wallSize);
                glScalef(0.5f, 0.5f, 0.5f);

                // modelTriggered ���¿� ���� �ٸ� ������
                if (modelTriggered[i][j]) {
                    DrawOBJWireframe(gVertices, gFaces, gNumFaces);
                }
                else {
                    DrawOBJShaded(gVertices, gNormals, gFaces, gNumFaces);
                }

                glPopMatrix();
            }
            else if (maze[i][j] == 4) {
                float cellCenterX = j - 0.2f;
                float cellCenterZ = i + 0.1f;
                float dx = playerX - cellCenterX;
                float dz = playerZ - cellCenterZ;
                float dist = sqrtf(dx * dx + dz * dz);
                bool playerHere = (dist <= 1.0f); // 1.0f �ݰ� ��

                if (playerHere && !modelTriggered[i][j]) {
                    modelTriggered[i][j] = true;
                }

                glPushMatrix();
                glTranslatef(j * wallSize, 0.4f, i * wallSize);
                glScalef(0.5f, 0.5f, 0.5f);

                if (modelTriggered[i][j]) {
                    DrawOBJWireframe(gVertices2, gFaces2, gNumFaces2);
                }
                else {
                    DrawOBJShaded(gVertices2, gNormals2, gFaces2, gNumFaces2);
                }

                glPopMatrix();
            }
        }
    }

    glPushMatrix();
    glTranslatef(0.0f, 25.0f, 0.0f);
    drawSky();
    glPopMatrix();

    glutSwapBuffers();
}
