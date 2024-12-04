#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <fstream>  // 파일 입출력에 사용
#include <direct.h> 
#include <string>
#include <vector>
#include <iostream>

std::vector<float> points;                      // 사용자 입력 점들
std::vector<std::vector<float>> meshVertices;   // 생성된 SOR 메쉬 정점
bool isMeshGenerated = false;                   // 메쉬 생성 상태 확인
bool wireframeMode = true;                      // 와이어프레임 모드 활성화
bool pointsOnlyMode = false;                    // 점들만 표시 모드 활성화

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

// 입력된 점들만 표시
void drawPointsOnly() {
    glPointSize(5.0f);
    glColor3f(1.0, 1.0, 1.0); // 흰색
    glBegin(GL_POINTS);
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

// 전역 변수 추가
bool animateRotation = false;        // 메쉬 회전 상태 플래그
float currentRotationAngle = 0; // 현재 회전 각도 (0도 또는 30도)

// 화면 갱신
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawYAxis(); // y축 그리기

    if (pointsOnlyMode) {
        drawPointsOnly(); // 점들만 표시
    }
    else {
        drawPointsAndLines(); // 입력 점과 선 그리기

        if (isMeshGenerated) {
            drawMesh(); // 생성된 SOR 메쉬 그리기
        }
    }

    if (isMeshGenerated) {
        glPushMatrix();

        // 메쉬 회전 적용 (카메라 위치는 변경하지 않음)
        glRotatef(currentRotationAngle, 1.0f, 1.0f, 1.0f); // y축 기준으로 회전

        //glScalef(0.5f, 0.5f, 0.5f); // 메쉬 크기 조정
        drawMesh();                 // 메쉬 그리기

        glPopMatrix();
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

// 모델 데이터 파일 저장 함수
void saveModel() {
    if (!isMeshGenerated) {
        std::cerr << "Error: 메쉬가 생성되지 않았습니다. 저장할 수 없습니다." << std::endl;
        return;
    }

    // 모델 디렉터리 생성 여부 확인 및 생성
    std::string modelDir = "./model";
    if (_mkdir(modelDir.c_str()) != 0 && errno != EEXIST) {
        std::cerr << "디렉터리를 생성할 수 없습니다: " << modelDir << std::endl;
        return;
    }

    // 사용자로부터 파일 이름 입력받기
    std::string filename;
    std::cout << "저장할 파일 이름을 입력하세요 (확장자 제외): ";
    std::cin >> filename;

    // 파일 경로 구성
    std::string filepath = modelDir + "/" + filename + ".dat";

    // 파일 열기
    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "파일을 생성할 수 없습니다: " << filepath << std::endl;
        return;
    }

    // 정점 정보 저장
    file << "VERTEX = " << meshVertices.size() << "\n";
    for (const auto& vertex : meshVertices) {
        file << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
    }

    // 면 정보 저장
    file << "FACE = " << (meshVertices.size() / points.size() * (points.size() - 2)) << "\n";
    for (size_t i = 0; i < meshVertices.size() - 1; ++i) {
        if ((i + 1) % points.size() != 0) {
            file << i << " " << i + 1 << " " << (i + points.size()) % meshVertices.size() << "\n";
        }
    }

    file.close();
    std::cout << "모델이 저장되었습니다: " << filepath << std::endl;
}

// 모델 데이터 파일 불러오기 함수
void loadModel() {
    // 사용자로부터 파일 이름 입력받기
    std::string filename;
    std::cout << "불러올 파일 이름을 입력하세요 (확장자 제외): ";
    std::cin >> filename;

    // 파일 경로 구성
    std::string filepath = "./model/" + filename + ".dat";

    // 파일 열기
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "파일을 열 수 없습니다: " << filepath << std::endl;
        return;
    }

    meshVertices.clear();
    points.clear();  // 점들도 초기화하여 충돌을 방지합니다.

    // 정점 정보 읽기
    std::string line;
    int numVertices = 0;
    while (std::getline(file, line)) {
        if (line.rfind("VERTEX", 0) == 0) {
            sscanf_s(line.c_str(), "VERTEX = %d", &numVertices);
            for (int i = 0; i < numVertices; ++i) {
                float x, y, z;
                file >> x >> y >> z;
                meshVertices.push_back({ x, y, z });
            }
        }
    }

    // 메쉬가 제대로 불러와졌는지 확인 후 상태 설정
    if (numVertices > 0) {
        isMeshGenerated = true;  // 메쉬가 생성되었다고 표시
        std::cout << "모델이 불러와졌습니다: " << filepath << std::endl;
    }
    else {
        isMeshGenerated = false;  // 불러온 정점이 없으면 메쉬 생성 실패
        std::cerr << "모델 불러오기에 실패했습니다." << std::endl;
    }

    // 화면 갱신 요청
    glutPostRedisplay();
}

// 타이머 콜백 함수 정의
void timerFunction(int value) {
    if (animateRotation) {
        currentRotationAngle += 2.0f; // 회전 각도를 일정하게 증가
        if (currentRotationAngle >= 360.0f) {
            currentRotationAngle -= 360.0f; // 360도를 초과하면 0도로 리셋
        }
        glutPostRedisplay(); // 화면 갱신 요청
        glutTimerFunc(16, timerFunction, 0); // 약 60FPS로 애니메이션 실행
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
    else if (key == 'b') {
        pointsOnlyMode = !pointsOnlyMode; // 점들만 표시 모드 전환
        glutPostRedisplay();
    }
    else if (key == 'p') {
        saveModel(); // 모델 저장
    }
    else if (key == 'l') {
        loadModel(); // 모델 불러오기
    }
    else if (key == 'o') {
        animateRotation = !animateRotation; // 회전 애니메이션 상태 변경
        if (animateRotation) {
            glutTimerFunc(0, timerFunction, 0); // 타이머 함수 시작
        }
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
