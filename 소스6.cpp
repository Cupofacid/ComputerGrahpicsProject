#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>

std::vector<float> points;                      // 사용자 입력 점들
std::vector<std::vector<float>> meshVertices;   // 생성된 SOR 메쉬 정점
bool isMeshGenerated = false;                   // 메쉬 생성 상태 확인
bool wireframeMode = true;                      // 와이어프레임 모드 활성화

// 화면 크기
int windowWidth = 800, windowHeight = 600;

// SOR 회전 분할 수
const int SOR_SEGMENTS = 36;

// π 값 정의
const float PI = 3.14159265358979323846f;

// 마우스 좌표 정규화 함수
void normalizeMouseCoords(int x, int y, float& normX, float& normY) {
    normX = (2.0f * x / windowWidth) - 1.0f;
    normY = 1.0f - (2.0f * y / windowHeight);
}

// y축을 화면에 그리기
void drawYAxis() {
    glColor3f(1.0, 0.0, 0.0); // 빨간색
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
}

// 입력 점과 선 그리기
void drawPointsAndLines() {
    glColor3f(1.0, 1.0, 1.0); // 흰색
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < points.size(); i += 2) {
        glVertex3f(points[i], points[i + 1], 0.0f);
    }
    glEnd();
}

// SOR 메쉬 생성
void generateSOR() {
    meshVertices.clear();
    if (points.size() < 4) {
        std::cerr << "Error: 최소한 두 개 이상의 점이 필요합니다.\n";
        isMeshGenerated = false;
        return;
    }

    for (int i = 0; i < SOR_SEGMENTS; ++i) {
        float angle = (2.0f * PI * i) / SOR_SEGMENTS;
        float cosTheta = cos(angle);
        float sinTheta = sin(angle);

        for (size_t j = 0; j < points.size(); j += 2) {
            float x = points[j];
            float y = points[j + 1];

            float rotatedX = cosTheta * x;
            float rotatedZ = sinTheta * x;

            meshVertices.push_back({ rotatedX, y, rotatedZ });
        }
    }

    isMeshGenerated = true;
    std::cout << "SOR 메쉬 생성 완료. 총 정점 수: " << meshVertices.size() << "\n";
}

// SOR 메쉬 렌더링
void drawMesh() {
    if (!isMeshGenerated || meshVertices.empty()) {
        std::cerr << "Error: 메쉬가 생성되지 않았습니다.\n";
        return;
    }

    if (wireframeMode) {
        glColor3f(0.0, 0.5, 1.0); // 파란색
        glBegin(GL_LINES);        // 와이어프레임 모드
        for (int i = 0; i < SOR_SEGMENTS; ++i) {
            int nextSegment = (i + 1) % SOR_SEGMENTS;
            for (size_t j = 0; j < points.size() / 2; ++j) {
                const auto& v1 = meshVertices[i * (points.size() / 2) + j];
                const auto& v2 = meshVertices[nextSegment * (points.size() / 2) + j];

                glVertex3f(v1[0], v1[1], v1[2]);
                glVertex3f(v2[0], v2[1], v2[2]);
            }
        }
        glEnd();
    }
    else {
        glColor3f(0.0, 1.0, 0.0); // 녹색
        glBegin(GL_TRIANGLES);   // 표면 모드
        for (int i = 0; i < SOR_SEGMENTS; ++i) {
            int nextSegment = (i + 1) % SOR_SEGMENTS;
            for (size_t j = 0; j < points.size() / 2 - 1; ++j) {
                const auto& v1 = meshVertices[i * (points.size() / 2) + j];
                const auto& v2 = meshVertices[nextSegment * (points.size() / 2) + j];
                const auto& v3 = meshVertices[i * (points.size() / 2) + j + 1];
                const auto& v4 = meshVertices[nextSegment * (points.size() / 2) + j + 1];

                glVertex3f(v1[0], v1[1], v1[2]);
                glVertex3f(v2[0], v2[1], v2[2]);
                glVertex3f(v3[0], v3[1], v3[2]);

                glVertex3f(v2[0], v2[1], v2[2]);
                glVertex3f(v4[0], v4[1], v4[2]);
                glVertex3f(v3[0], v3[1], v3[2]);
            }
        }
        glEnd();
    }
}

// 화면 갱신
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawYAxis();          // y축 그리기
    drawPointsAndLines(); // 입력 점과 선 그리기

    if (isMeshGenerated) {
        drawMesh(); // 생성된 SOR 메쉬 그리기
    }

    glutSwapBuffers();
}

// 마우스 클릭 이벤트
void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float normX, normY;
        normalizeMouseCoords(x, y, normX, normY);

        points.push_back(normX); // X 좌표
        points.push_back(normY); // Y 좌표

        std::cout << "점 추가됨: (" << normX << ", " << normY << ")\n";
        glutPostRedisplay();
    }
}

// 키보드 이벤트
void keyboardFunc(unsigned char key, int x, int y) {
    if (key == 'a') {
        generateSOR(); // SOR 메쉬 생성
        glutPostRedisplay();
    }
    else if (key == 'w') {
        wireframeMode = !wireframeMode; // 와이어프레임 모드 전환
        glutPostRedisplay();
    }
}

// OpenGL 초기화
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
}

// 메인 함수
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("SOR Mesh Creator");

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouseFunc);
    glutKeyboardFunc(keyboardFunc);

    glutMainLoop();
    return 0;
}
