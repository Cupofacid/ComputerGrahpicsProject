#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include "stb_image.h"

// 미로 데이터: 1은 벽, 0은 통로
std::vector<std::vector<int>> maze = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 3, 0, 0, 1, 4, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 1, 0, 1, 5, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const float wallSize = 1.0f; // 미로 크기

// 화면 중심 좌표
int windowWidth = 1200, windowHeight = 800;
int centerX = windowWidth / 2;
int centerY = windowHeight / 2;

float cameraX = 1.0f, cameraY = 0.5f, cameraZ = 1.0f; // 카메라 위치
float playerX = 1.0f, playerY = 0.5f, playerZ = 1.0f; // 플레이어 물체 위치

float yaw = 0.0f, pitch = 0.0f; // 카메라 회전 (yaw: 좌우, pitch: 상하)

GLuint wallTexture;
GLuint floorTexture;
GLuint skyTexture;
GLuint skyTopTexture;

static float angleX = 0.0f;
static float angleY = 0.0f;
static int mousePrevX, mousePrevY;
static int mouseLeftDown = 0;

enum GameState { GAME_DESCRIPTION, GAME_RUNNING };
GameState currentGameState = GAME_DESCRIPTION;

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v[3];
} Face;

// 추가 모델용 전역 변수
static Vertex* gVertices2 = NULL;
static Vertex* gNormals2 = NULL;
static Face* gFaces2 = NULL;
static int gNumVertices2 = 0;
static int gNumFaces2 = 0;
// 추가 모델용 전역 변수
static Vertex* gVertices3 = NULL;
static Vertex* gNormals3 = NULL;
static Face* gFaces3 = NULL;
static int gNumVertices3 = 0;
static int gNumFaces3 = 0;
// 추가 모델용 전역 변수
static Vertex* gVertices4 = NULL;
static Vertex* gNormals4 = NULL;
static Face* gFaces4 = NULL;
static int gNumVertices4 = 0;
static int gNumFaces4 = 0;
// 추가 모델용 전역 변수
static Vertex* gVertices5 = NULL;
static Vertex* gNormals5 = NULL;
static Face* gFaces5 = NULL;
static int gNumVertices5 = 0;
static int gNumFaces5 = 0;

// 벡터 연산 함수
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
        fprintf(stderr, "파일 열기 실패: %s\n", filename);
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
        fprintf(stderr, "OBJ 파일에 정점 또는 면 정보가 없습니다.\n");
        return 0;
    }

    fseek(fp, 0, SEEK_SET);

    vertices = (Vertex*)malloc(sizeof(Vertex) * totalVertices);
    normals = (Vertex*)malloc(sizeof(Vertex) * totalVertices);
    faces = (Face*)malloc(sizeof(Face) * totalFaces);
    int* vertexFaceCount = (int*)calloc(totalVertices, sizeof(int));

    if (!vertices || !normals || !faces || !vertexFaceCount) {
        fclose(fp);
        fprintf(stderr, "메모리 할당 실패\n");
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

    fprintf(stdout, "OBJ 로드 완료: %s 정점 %d개, 면 %d개\n", filename, numVertices, numFaces);
    return 1;
}

void DrawOBJShaded(Vertex* vertices, Vertex* normals, Face* faces, int numFaces) {
    glDisable(GL_TEXTURE_2D); // 텍스처 비활성화
    if (currentGameState == GAME_RUNNING) {
        // 게임 실행 중일 때만 조명 및 재질 설정
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        // 금색 재질 (단순 색상)
        GLfloat matAmb[] = { 0.8f, 0.6f, 0.2f, 1.0f }; // 금색 주변광 (밝은 황금빛)
        GLfloat matDif[] = { 0.8f, 0.6f, 0.2f, 1.0f }; // 금색 확산광 (밝은 황금빛)
        GLfloat matSpe[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 반사광 제거
        GLfloat matShn[] = { 0.0f }; // 반짝임 제거
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpe);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShn);

        // 단순 금색 조명
        GLfloat lightPos[] = { 5.0f, -10.0f, 5.0f, 1.0f }; // 조명 위치
        GLfloat lightAmb[] = { 0.8f, 0.6f, 0.2f, 1.0f };  // 금빛 주변광
        GLfloat lightDif[] = { 0.8f, 0.6f, 0.2f, 1.0f };  // 금빛 확산광
        GLfloat lightSpe[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // 반사광 제거
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpe);
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
    if (currentGameState == GAME_RUNNING) {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
    }
    glEnable(GL_TEXTURE_2D); // 텍스처 다시 활성화
}


void DrawOBJWireframe(Vertex* vertices, Face* faces, int numFaces, bool useLighting) {
    // 현재 OpenGL 상태 저장
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);

    if (useLighting) {
        // 조명 활성화 및 설정
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    else {
        glDisable(GL_LIGHTING); // 조명 비활성화
    }

    glDisable(GL_TEXTURE_2D); // 텍스처 비활성화

    // 와이어프레임 색상 설정 (흰색)
    glColor3f(0.83f, 0.69f, 0.22f); // 금색
    glLineWidth(2.0f);

    // 와이어프레임 그리기
    glBegin(GL_LINES);
    for (int i = 0; i < numFaces; i++) {
        Face f = faces[i];
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 3; j++) {
            int v_idx = f.v[j] - 1;
            glVertex3f(vertices[v_idx].x, vertices[v_idx].y, vertices[v_idx].z);
        }
        glEnd();
    }
    // 이전 OpenGL 상태 복구
    glPopAttrib();
}


// 텍스처를 로드 함수
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

// 미로 벽 그리기 함수 (텍스처 적용)
void drawWall() {
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glBegin(GL_QUADS);

    // 앞면
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

    // 뒷면
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

    // 왼쪽 면
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

    // 오른쪽 면
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

    // 위쪽 면
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

    // 아래쪽 면
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

    glEnd();
}

// 미로 바닥 그리기 함수
void drawFloor() {
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glBegin(GL_QUADS);

    // 바닥 타일 면에 텍스처 매핑
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.0f, -0.5f);  // 왼쪽 하단
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, -0.5f);   // 오른쪽 하단
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.0f, 0.5f);    // 오른쪽 상단
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, 0.5f);   // 왼쪽 상단

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
void drawSkyTop() {
    glBindTexture(GL_TEXTURE_2D, skyTopTexture);
    glBegin(GL_QUADS);
    // Top face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, 25.0f, -50.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, 25.0f, -50.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, 25.0f, 50.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f, 25.0f, 50.0f);
    glEnd();
}

// 전역 변수로 모델 상태를 저장할 2차원 배열을 선언(미로 크기에 맞게)
static bool modelTriggered[10][10] = { false }; // 미로가 10x10일 경우

// 모든 모델과 충돌했는지 확인하는 함수
bool checkGameClear() {
    for (int i = 0; i < (int)maze.size(); ++i) {
        for (int j = 0; j < (int)maze[i].size(); ++j) {
            if ((maze[i][j] == 2 || maze[i][j] == 3 || maze[i][j] == 4 || maze[i][j] == 5) && !modelTriggered[i][j]) {
                return false; // 아직 충돌하지 않은 모델이 있음
            }
        }
    }
    return true; // 모든 모델과 충돌 완료
}

void drawText(const char* text, float x, float y, void* font) {
    // 현재 OpenGL 상태 저장
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D); // 텍스처 비활성화

    glDisable(GL_LIGHTING); // 조명 비활성화
    glDisable(GL_DEPTH_TEST); // 깊이 테스트 비활성화 (텍스트는 화면 위에 그려지므로)
    glClearColor(1.0f, 1.0f, 1.0f, 1);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight); // 2D 좌표계 설정

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 텍스트 출력
    glRasterPos2f(x, y); // 텍스트 위치 설정
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(font, *c);
    }

    // 행렬 상태 복원
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D); // 텍스처 다시 활성화
    // 이전 OpenGL 상태 복구
    glPopAttrib();
    glClearColor(0.407f, 0.705f, 0.941f, 1);
}

void configureDimLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // 어두운 주변광
    GLfloat lightDiffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // 약간의 확산광
    GLfloat lightSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 반사광 없음
    GLfloat lightPosition[] = { 0.0f, 10.0f, 0.0f, 1.0f }; // 상단에서 비추는 조명

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float dirX = cos(pitch) * cos(yaw);
    float dirY = sin(pitch);
    float dirZ = cos(pitch) * sin(yaw);

    gluLookAt(cameraX, cameraY, cameraZ,
        cameraX + dirX, cameraY + dirY, cameraZ + dirZ,
        0.0f, 1.0f, 0.0f);

    bool gameCleared = checkGameClear(); // 게임 클리어 여부 확인

    // 플레이어가 현재 위치한 셀 인덱스 계산
    int playerCellX = (int)floor(playerX);
    int playerCellZ = (int)floor(playerZ);

    // 모델 총 개수와 플레이어가 찾은 개수를 계산
    int totalModels = 0;
    int foundModels = 0;
    for (int i = 0; i < (int)maze.size(); ++i) {
        for (int j = 0; j < (int)maze[i].size(); ++j) {
            if (maze[i][j] == 2 || maze[i][j] == 3 || maze[i][j] == 4 || maze[i][j] == 5) {
                totalModels++;
                if (modelTriggered[i][j]) {
                    foundModels++;
                }
            }
        }
    }

    for (int i = 0; i < (int)maze.size(); ++i) {
        for (int j = 0; j < (int)maze[i].size(); ++j) {
            // 바닥 그리기
            glPushMatrix();
            glTranslatef(j * 1.0f, 0.0f, i * 1.0f);
            drawFloor();
            glPopMatrix();

            if (maze[i][j] == 1) {
                // 벽
                glPushMatrix();
                glTranslatef(j * wallSize, wallSize / 2, i * wallSize);
                glScalef(wallSize, wallSize, wallSize);
                drawWall();
                glPopMatrix();
            }
            else if (maze[i][j] == 2 || maze[i][j] == 3 || maze[i][j] == 4 || maze[i][j] == 5) {
                float cellCenterX = j - 0.2f;
                float cellCenterZ = i + 0.1f;
                float dx = playerX - cellCenterX;
                float dz = playerZ - cellCenterZ;
                float dist = sqrtf(dx * dx + dz * dz);
                bool playerHere = (dist <= 0.9f); // 0.9f 반경 내

                // 플레이어가 접촉했다면 modelTriggered를 true로 변경
                if (playerHere && !modelTriggered[i][j]) {
                    modelTriggered[i][j] = true;
                }

                glPushMatrix();
                glTranslatef(j * wallSize, 0.4f, i * wallSize);
                glScalef(0.5f, 0.5f, 0.5f);

                if (modelTriggered[i][j]) {
                    if (maze[i][j] == 2) {
                        DrawOBJWireframe(gVertices2, gFaces2, gNumFaces2, gameCleared); // 게임 클리어 여부에 따라 조명 설정
                    }
                    else if (maze[i][j] == 3) {
                        DrawOBJWireframe(gVertices3, gFaces3, gNumFaces3, gameCleared);
                    }
                    else if (maze[i][j] == 4) {
                        DrawOBJWireframe(gVertices4, gFaces4, gNumFaces4, gameCleared);
                    }
                    else if (maze[i][j] == 5) {
                        DrawOBJWireframe(gVertices5, gFaces5, gNumFaces5, gameCleared);
                    }
                }
                else {
                    if (maze[i][j] == 2) {
                        DrawOBJShaded(gVertices2, gNormals2, gFaces2, gNumFaces2);
                    }
                    else if (maze[i][j] == 3) {
                        DrawOBJShaded(gVertices3, gNormals3, gFaces3, gNumFaces3);
                    }
                    else if (maze[i][j] == 4) {
                        DrawOBJShaded(gVertices4, gNormals4, gFaces4, gNumFaces4);
                    }
                    else if (maze[i][j] == 5) {
                        DrawOBJShaded(gVertices5, gNormals5, gFaces5, gNumFaces5);
                    }
                }

                glPopMatrix();
            }
        }
    }
    glPushMatrix();
    glTranslatef(0.0f, 25.0f, 0.0f);
    drawSky();
    drawSkyTop();
    glPopMatrix();

    // 게임 설명 화면 상태일 때만 설명창 오버레이 추가
    if (currentGameState == GAME_DESCRIPTION) {
        configureDimLighting();
        drawText("Welcome to the Maze Game!", windowWidth / 2 - 100, windowHeight / 2 + 100, GLUT_BITMAP_HELVETICA_18);
        drawText("Objective: Find all models in the maze.", windowWidth / 2 - 150, windowHeight / 2 + 50, GLUT_BITMAP_HELVETICA_18);
        drawText("Press ENTER to start the game.", windowWidth / 2 - 100, windowHeight / 2 - 50, GLUT_BITMAP_HELVETICA_18);
    }

    // 게임 클리어 상태 확인
    if (checkGameClear()) {
        configureDimLighting();
        drawText("Game Clear!", windowWidth / 2 - 100, windowHeight / 2, GLUT_BITMAP_HELVETICA_18); // 게임 클리어 이미지 출력
    }

    // 모델 상태 표시
    char modelStatus[50];
    sprintf(modelStatus, "Models Found: %d / %d", foundModels, totalModels);
    drawText(modelStatus, windowWidth - 200, windowHeight - 30, GLUT_BITMAP_HELVETICA_18);

    glutSwapBuffers();
}
