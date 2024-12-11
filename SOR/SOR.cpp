#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <fstream>  // 파일 출력을 위한 헤더
#include <cstdlib> // system 함수 사용을 위한 헤더
using namespace std;

std::vector<float> points;                      // 사용자 입력 점들
std::vector<std::vector<float>> meshVertices;   // 생성된 SOR 메쉬 정점
bool isMeshGenerated = false;                   // 메쉬 생성 상태 확인
bool wireframeMode = true;                      // 와이어프레임 모드 활성화

int windowWidth = 1000, windowHeight = 900; // 화면 크기

// SOR 회전 분할 수 (사용자 입력에 따라 동적으로 결정)
int SOR_SEGMENTS = 36; // 기본값은 36

const float PI = 3.14159265358979323846f; // π 값 정의

char rotationAxis = 'y'; // 기본 회전 축 (y축)

// 360의 약수인지 확인하는 함수
bool isDivisorOf360(int num) {
    return (360 % num == 0);
}

// 마우스 좌표 정규화 함수
void normalizeMouseCoords(int x, int y, float& normX, float& normY) {
    normX = (2.0f * x / windowWidth) - 1.0f;
    normY = 1.0f - (2.0f * y / windowHeight);
}

// y축을 화면에 그리기
void drawYAxis() {
    glColor3f(0.0, 0.0, 0.0); // 검은색
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
}
// x축을 화면에 그리기
void drawXAxis() {
    glColor3f(0.0, 0.0, 0.0); // 빨간색
    glBegin(GL_LINES);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();
}

// z축을 화면에 그리기
void drawZAxis() {
    glColor3f(0.0, 0.0, 0.0); // 파란색
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}

// 입력된 점들만 표시
void drawPointsOnly() {
    glPointSize(3.0f);
    glColor3f(0.0, 0.0, 0.0); // 검은색
    glBegin(GL_POINTS);
    for (size_t i = 0; i < points.size(); i += 2) {
        glVertex3f(points[i], points[i + 1], 0.0f);
    }
    glEnd();
}

// 화면에 텍스트 출력 함수
void drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y); // 텍스트 위치 설정
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // 글꼴과 문자 출력
    }
}

// 키보드 인풋 도움말 출력
void displayKeyboardHelp() {
    glColor3f(0.0, 0.0, 0.0); // 텍스트 색상: 검정
    drawText(-0.95f, 0.9f, "Keyboard Inputs");
    drawText(-0.95f, 0.85f, "Enter : Generate SOR");
    drawText(-0.95f, 0.8f, "x / y / z : Set rotation axis");
    drawText(-0.95f, 0.75f, "w : Toggle wireframe");
    drawText(-0.95f, 0.7f, "s : Save SOR to OBJ");
    drawText(-0.95f, 0.65f, "q : Reset model");
    drawText(-0.95f, 0.6f, "g : Run Maze file");
    drawText(-0.95f, 0.55f, "ESC : Exit");
}

void generateSOR() {
    meshVertices.clear();
    if (points.size() < 4) {
        std::cerr << "Error: 최소한 두 개 이상의 점이 필요합니다.\n";
        isMeshGenerated = false;
        return;
    }

    // SOR 메쉬 정점 생성
    for (int i = 0; i < SOR_SEGMENTS; ++i) {
        float angle = (2.0f * PI * i) / SOR_SEGMENTS;
        float cosTheta = cos(angle);
        float sinTheta = sin(angle);

        for (size_t j = 0; j < points.size(); j += 2) {
            float x = points[j];
            float y = points[j + 1];
            float rotatedX = x, rotatedY = y, rotatedZ = 0.0f;

            // 선택한 축에 따라 회전 변환 적용
            if (rotationAxis == 'x') {
                // x축 기준 회전 (y, z 좌표만 변환)
                rotatedY = cosTheta * y - sinTheta * rotatedZ;
                rotatedZ = sinTheta * y + cosTheta * rotatedZ;
                rotatedX = x; // x 좌표는 그대로 유지
            }
            else if (rotationAxis == 'y') {
                // y축 기준 회전 (x, z 좌표만 변환)
                rotatedX = cosTheta * x + sinTheta * rotatedZ;
                rotatedZ = -sinTheta * x + cosTheta * rotatedZ;
                rotatedY = y; // y 좌표는 그대로 유지
            }
            else if (rotationAxis == 'z') {
                // z축 기준 회전 (x, y 좌표만 변환)
                rotatedX = cosTheta * x - sinTheta * y;
                rotatedY = sinTheta * x + cosTheta * y;
                rotatedZ = 0.0f; // z 좌표는 그대로 유지
            }

            // 생성된 정점을 메쉬에 추가
            meshVertices.push_back({ rotatedX, rotatedY, rotatedZ });
        }
    }

    isMeshGenerated = true;
    std::cout << "SOR 메쉬 생성 완료. 총 정점 수: " << meshVertices.size() << "\n";
}

// 주어진 축을 기준으로 점들을 회전
void rotatePoints(char axis) {
    float angleRadians = (2.0f * PI) / SOR_SEGMENTS; // 360도를 SOR_SEGMENTS로 나눈 각도
    float cosTheta = cos(angleRadians);
    float sinTheta = sin(angleRadians);

    for (size_t i = 0; i < points.size(); i += 2) {
        float x = points[i];
        float y = points[i + 1];
        float rotatedX = x, rotatedY = y, rotatedZ = 0.0f;

        if (axis == 'x') {
            // x축 기준 회전 (y, z 좌표만 변환)
            rotatedY = cosTheta * y - sinTheta * rotatedZ;
            rotatedZ = sinTheta * y + cosTheta * rotatedZ;
        }
        else if (axis == 'y') {
            // y축 기준 회전 (x, z 좌표만 변환)
            rotatedX = cosTheta * x + sinTheta * rotatedZ;
            rotatedZ = -sinTheta * x + cosTheta * rotatedZ;
        }
        else if (axis == 'z') {
            // z축 기준 회전 (x, y 좌표만 변환)
            rotatedX = cosTheta * x - sinTheta * y;
            rotatedY = sinTheta * x + cosTheta * y;
        }

        // 점 갱신 (현재 점 저장 방식은 x, y만 포함됨 -> z는 무시)
        points[i] = rotatedX;
        points[i + 1] = rotatedY;
    }
}

// 모델과 점 초기화 함수
void resetModel() {
    points.clear();         // 입력된 점 초기화
    meshVertices.clear();   // 메쉬 정점 초기화
    isMeshGenerated = false; // 메쉬 생성 상태 초기화
    wireframeMode = true;   // 기본 와이어프레임 모드 활성화
    std::cout << "모델 초기화 완료. 새로운 점을 입력하세요.\n";
    glutPostRedisplay();    // 화면 갱신
}

// SOR 메쉬 렌더링
void drawMesh() {
    if (!isMeshGenerated || meshVertices.empty()) {
        return;
    }

    glPointSize(5.0f); // 점 크기 조정
    glColor3f(0.0, 0.0, 0.0); // 검은색
    glBegin(GL_POINTS);
    for (const auto& vertex : meshVertices) {
        glVertex3f(vertex[0], vertex[1], vertex[2]);
    }
    glEnd();

    // 와이어프레임 모드로 메쉬 그리기
    if (wireframeMode) {
        glColor3f(1.0, 1.0, 0.0); // 파란색
        glBegin(GL_LINES);        // 와이어프레임 모드

        size_t numPoints = points.size() / 2; // 점의 개수 (x, y 쌍으로 저장되어 있음)

        for (int i = 0; i < SOR_SEGMENTS; ++i) {
            int nextSegment = (i + 1) % SOR_SEGMENTS;

            for (size_t j = 0; j < numPoints - 1; ++j) {
                const auto& v1 = meshVertices[i * numPoints + j];
                if (numPoints == 0) {
                    continue;
                }

                size_t rightIndex = (j + 1) % numPoints;
                const auto& right = meshVertices[i * numPoints + rightIndex];

                size_t downIndex = (j + numPoints) % meshVertices.size();
                const auto& down = meshVertices[((i + 1) % SOR_SEGMENTS) * numPoints + j];

                size_t rightDownIndex = (j + 1) % numPoints;
                const auto& rightDown = meshVertices[((i + 1) % SOR_SEGMENTS) * numPoints + rightDownIndex];

                glVertex3f(v1[0], v1[1], v1[2]);
                glVertex3f(right[0], right[1], right[2]);

                glVertex3f(v1[0], v1[1], v1[2]);
                glVertex3f(down[0], down[1], down[2]);

                glVertex3f(v1[0], v1[1], v1[2]);
                glVertex3f(rightDown[0], rightDown[1], rightDown[2]);
            }
        }

        glEnd();
    }
}

// 메쉬를 OBJ 파일로 저장하는 함수
void saveMeshToOBJ(const std::string& filename) {
    std::string Path = "SORdata/" + filename; // 저장 경로를 D:/Meshes/로 설정
    std::ofstream outFile(Path);
    if (!outFile) {
        std::cerr << "파일을 열 수 없습니다: " << Path << std::endl;
        return;
    }

    // OBJ 헤더
    outFile << "# SOR Mesh exported as OBJ format\n";

    // 정점 정보 저장
    for (const auto& vertex : meshVertices) {
        outFile << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
    }

    // 폴리곤(face) 정보 저장
    size_t numPoints = points.size() / 2; // 입력 점 개수
    for (int i = 0; i < SOR_SEGMENTS; ++i) {
        int nextSegment = (i + 1) % SOR_SEGMENTS;

        for (size_t j = 0; j < numPoints - 1; ++j) {
            int idx1 = i * numPoints + j + 1; // OBJ 인덱스는 1부터 시작
            int idx2 = idx1 + 1;
            int idx3 = nextSegment * numPoints + j + 1;
            int idx4 = idx3 + 1;

            outFile << "f " << idx1 << " " << idx2 << " " << idx3 << "\n";
            outFile << "f " << idx2 << " " << idx4 << " " << idx3 << "\n";
        }
    }

    outFile.close();
    std::cout << "메쉬가 " << Path << "에 저장되었습니다.\n";
}

// 화면 갱신
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawXAxis(); // x축 그리기
    drawYAxis(); // y축 그리기
    drawZAxis(); // z축 그리기
    drawPointsOnly(); // 점들만 표시
    drawMesh();
    // 화면 왼쪽 위에 키보드 입력 도움말 표시
    glPushMatrix();
    glLoadIdentity();    // 모델뷰 행렬 초기화
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // 2D 평면 설정
    displayKeyboardHelp();           // 키보드 입력 도움말 출력
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
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

// 키보드 이벤트 처리
void keyboardFunc(unsigned char key, int x, int y) {
    if (key == '\r' || key == '\n') {  // Enter 키가 눌리면
        generateSOR(); // SOR 메쉬 생성
        glutPostRedisplay();
    }
    else if (key == 'x' || key == 'y' || key == 'z') {
        rotationAxis = key; // 회전 축 변경
        std::cout << "현재 회전 축: " << rotationAxis << "축\n";
    }
    else if (key == 'w') {
        wireframeMode = !wireframeMode; // 와이어프레임 <-> 점 모드 전환
        glutPostRedisplay();
    }
    else if (key == 's') {  // 's' 키를 눌러서 메쉬를 저장
        if (isMeshGenerated) {
            std::cout << "저장할 이름을 입력하시오: ";
            std::string filename;
            std::cin >> filename;
            filename += ".obj";
            saveMeshToOBJ(filename);
        }
    }
    else if (key == 'q') { // 'q' 키를 눌러 초기화
        resetModel(); // 모델 초기화
    }
    else if (key == 'g') { // 'g' 키를 눌러 .exe 파일 실행
        std::cout << "Maze.exe";
        system("Maze.exe");
    }
    else if (key == 27) { // ESC 키로 종료
        exit(0);
    }
}


// 사용자 입력을 통해 SOR 회전 분할 각도 설정
void setSORSegmentsFromUserInput() {
    int userAngle;
    bool validInput = false;

    while (!validInput) {
        std::cout << "회전 분할 각도를 입력하세요 (360의 약수 중 하나): ";
        std::cin >> userAngle;

        if (isDivisorOf360(userAngle)) {
            SOR_SEGMENTS = 360 / userAngle;
            std::cout << "SOR 회전 분할 수: " << SOR_SEGMENTS << std::endl;
            validInput = true;
        }
        else {
            std::cerr << "오류: 입력된 값은 360의 약수가 아닙니다. 다시 시도하세요.\n";
        }
    }
}

// OpenGL 초기화
void init() {
    setSORSegmentsFromUserInput(); // 사용자 입력으로 SOR 분할 수 설정
    glClearColor(0.5, 0.5, 0.5, 1.0);
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