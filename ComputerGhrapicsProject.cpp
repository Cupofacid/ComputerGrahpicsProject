#include <GL/glut.h>
#include <vector>
#include <cmath>

// 미로 데이터: 1은 벽, 0은 통로
std::vector<std::vector<int>> maze = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1}
};

float cameraX = 1.5f, cameraY = 0.5f, cameraZ = 1.5f; // 카메라 위치
float yaw = 0.0f, pitch = 0.0f; // 카메라 회전 (yaw: 좌우, pitch: 상하)

bool keyStates[256] = { false }; // 키 상태 추적

// 화면 중심 좌표
int windowWidth = 1000, windowHeight = 800;
int centerX = windowWidth / 2;
int centerY = windowHeight / 2;

// 미로 크기
const float wallSize = 1.0f;

// 미로 충돌 검사 함수
bool isColliding(float newX, float newZ) {
    // 카메라가 위치할 새로운 그리드 좌표를 계산
    int gridX = static_cast<int>(newX);
    int gridZ = static_cast<int>(newZ);
    
    // 잘 안돼서 지금은 비워둠

    return false; // 벽이 없으면 충돌하지 않음
}


// 카메라 이동 처리
void moveCamera() {
    const float moveSpeed = 0.05f;

    // 카메라 방향 벡터 계산
    float forwardX = cos(pitch) * cos(yaw);
    float forwardZ = cos(pitch) * sin(yaw);
    float rightX = sin(yaw);
    float rightZ = -cos(yaw);

    // 이동을 위한 새로운 위치 계산
    float newCameraX = cameraX;
    float newCameraZ = cameraZ;

    if (keyStates['w']) { // 전진
        newCameraX = cameraX + moveSpeed * forwardX;
        newCameraZ = cameraZ + moveSpeed * forwardZ;
        if (!isColliding(newCameraX, newCameraZ)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
        }
    }
    if (keyStates['s']) { // 후진
        newCameraX = cameraX - moveSpeed * forwardX;
        newCameraZ = cameraZ - moveSpeed * forwardZ;
        if (!isColliding(newCameraX, newCameraZ)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
        }
    }
    if (keyStates['d']) { // 왼쪽 이동
        newCameraX = cameraX - moveSpeed * rightX;
        newCameraZ = cameraZ - moveSpeed * rightZ;
        if (!isColliding(newCameraX, newCameraZ)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
        }
    }
    if (keyStates['a']) { // 오른쪽 이동
        newCameraX = cameraX + moveSpeed * rightX;
        newCameraZ = cameraZ + moveSpeed * rightZ;
        if (!isColliding(newCameraX, newCameraZ)) {
            cameraX = newCameraX;
            cameraZ = newCameraZ;
        }
    }

    glutPostRedisplay(); // 화면 갱신
}

// 화면 그리기
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 카메라 방향 계산
    float dirX = cos(pitch) * cos(yaw);
    float dirY = sin(pitch);
    float dirZ = cos(pitch) * sin(yaw);

    // 카메라 위치 설정
    gluLookAt(cameraX, cameraY, cameraZ,           // 카메라 위치
        cameraX + dirX, cameraY + dirY, cameraZ + dirZ, // 바라보는 점
        0.0f, 1.0f, 0.0f);                  // 업 벡터

    // 미로 그리기
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].size(); ++j) {
            if (maze[i][j] == 1) { // 벽
                glPushMatrix();
                glTranslatef(i, wallSize / 2, j);
                glScalef(wallSize, wallSize, wallSize);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }

    glutSwapBuffers();
}

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
    if (pitch > 1.5f) pitch = 1.5f;  // 약 85도
    if (pitch < -1.5f) pitch = -1.5f; // 약 -85도

    // 마우스를 다시 화면 중앙으로 이동
    glutWarpPointer(centerX, centerY);

    glutPostRedisplay();
}


// 조명 설정
void setupLighting() {
    glEnable(GL_LIGHTING); // 조명 활성화
    glEnable(GL_LIGHT0);   // 기본 조명(Light 0) 활성화

    // 조명 색상 설정
    GLfloat lightPos[] = { 2.0f, 4.0f, 2.0f, 1.0f };  // 조명 위치
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // 주변광
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // 난반사광
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 반사광

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // 재질 설정
    GLfloat matAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat matDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matShininess[] = { 50.0f }; // 반사 강도

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

// OpenGL 초기화
void initOpenGL() {
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 배경색 설정
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    setupLighting(); // 조명 설정 호출

    // 마우스를 화면 중앙으로 이동
    glutWarpPointer(centerX, centerY);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("3D Maze with Collision Detection");

    initOpenGL();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion); // 마우스 움직임 처리 등록
    glutIdleFunc(moveCamera);           // 부드러운 이동 처리

    glutMainLoop();
    return 0;
}



