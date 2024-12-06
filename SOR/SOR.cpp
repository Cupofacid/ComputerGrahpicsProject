#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <fstream>  // ���� ����� ���� ���
using namespace std;

std::vector<float> points;                      // ����� �Է� ����
std::vector<std::vector<float>> meshVertices;   // ������ SOR �޽� ����
bool isMeshGenerated = false;                   // �޽� ���� ���� Ȯ��
bool wireframeMode = true;                      // ���̾������� ��� Ȱ��ȭ

// ȭ�� ũ��
int windowWidth = 1000, windowHeight = 900;

// SOR ȸ�� ���� �� (����� �Է¿� ���� �������� ����)
int SOR_SEGMENTS = 36; // �⺻���� 36
const float angle = 0;

// �� �� ����
const float PI = 3.14159265358979323846f;

// 360�� ������� Ȯ���ϴ� �Լ�
bool isDivisorOf360(int num) {
    return (360 % num == 0);
}

// ���콺 ��ǥ ����ȭ �Լ�
void normalizeMouseCoords(int x, int y, float& normX, float& normY) {
    normX = (2.0f * x / windowWidth) - 1.0f;
    normY = 1.0f - (2.0f * y / windowHeight);
}

// y���� ȭ�鿡 �׸���
void drawYAxis() {
    glColor3f(0.0, 0.0, 0.0); // ������
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
}

// �Էµ� ���鸸 ǥ��
void drawPointsOnly() {
    glPointSize(3.0f);
    glColor3f(0.0, 0.0, 0.0); // ������
    glBegin(GL_POINTS);
    for (size_t i = 0; i < points.size(); i += 2) {
        glVertex3f(points[i], points[i + 1], 0.0f);
    }
    glEnd();
}

// SOR �޽� ����
void generateSOR() {
    meshVertices.clear();
    if (points.size() < 4) {
        std::cerr << "Error: �ּ��� �� �� �̻��� ���� �ʿ��մϴ�.\n";
        isMeshGenerated = false;
        return;
    }

    // SOR �޽� ���� ����
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
    std::cout << "SOR �޽� ���� �Ϸ�. �� ���� ��: " << meshVertices.size() << "\n";
}

// SOR �޽� ������
void drawMesh() {
    if (!isMeshGenerated || meshVertices.empty()) {
        return;
    }

    glPointSize(5.0f); // �� ũ�� ����
    glColor3f(0.0, 0.0, 0.0); // ������
    glBegin(GL_POINTS);
    for (const auto& vertex : meshVertices) {
        glVertex3f(vertex[0], vertex[1], vertex[2]);
    }
    glEnd();

    // ���̾������� ���� �޽� �׸���
    if (wireframeMode) {
        glColor3f(0.0, 0.5, 1.0); // �Ķ���
        glBegin(GL_LINES);        // ���̾������� ���

        size_t numPoints = points.size() / 2; // ���� ���� (x, y ������ ����Ǿ� ����)

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

// �޽��� ���Ϸ� �����ϴ� �Լ�
void saveMeshToFile(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "������ �� �� �����ϴ�: " << filename << std::endl;
        return;
    }

    // ���� ���� ����
    outFile << "VERTEX_COUNT = " << meshVertices.size() << "\n";  // ���� ���� ����
    for (const auto& vertex : meshVertices) {
        outFile << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
    }

    // ������ ���� ��� (�ﰢ�� ����)
    int polygonCount = 0;
    for (int i = 0; i < SOR_SEGMENTS; ++i) {
        for (size_t j = 0; j < points.size() / 2 - 1; ++j) {
            polygonCount += 2;  // �� ���׸�Ʈ���� �� ���� �������� ������
        }
    }

    // ������ ���� ���� (������ ���� ��, ������ ������ ����� �� ����)
    outFile << "POLYGON_COUNT = " << polygonCount << "\n";

    // ������ ���� ���� (�ﰢ�� ����)
    for (int i = 0; i < SOR_SEGMENTS; ++i) {
        for (size_t j = 0; j < points.size() / 2 - 1; ++j) {
            int idx1 = i * (points.size() / 2) + j;
            int idx2 = idx1 + 1;
            int idx3 = ((i + 1) % SOR_SEGMENTS) * (points.size() / 2) + j;
            int idx4 = idx3 + 1;

            // �� ���� �ﰢ���� ���������� ����
            outFile << idx1 << " " << idx2 << " " << idx3 << "\n";
            outFile << idx2 << " " << idx4 << " " << idx3 << "\n";
        }
    }

    outFile.close();
}

// ȭ�� ����
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawYAxis(); // y�� �׸���
    drawPointsOnly(); // ���鸸 ǥ��

    drawMesh();

    glutSwapBuffers();
}

// ���콺 Ŭ�� �̺�Ʈ
void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float normX, normY;
        normalizeMouseCoords(x, y, normX, normY);

        points.push_back(normX); // X ��ǥ
        points.push_back(normY); // Y ��ǥ

        std::cout << "�� �߰���: (" << normX << ", " << normY << ")\n";
        glutPostRedisplay();
    }
}

// Ű���� �̺�Ʈ
void keyboardFunc(unsigned char key, int x, int y) {
    if (key == '\r' || key == '\n') {  // ����Ű�� ������
        generateSOR(); // SOR �޽� ����
        glutPostRedisplay();
    }
    else if (key == 'w') {
        wireframeMode = !wireframeMode; // ���̾������� <-> �� ��� ��ȯ
        glutPostRedisplay();
    }
    else if (key == 's') {  // 's' Ű�� ������ �޽��� ����
        if (isMeshGenerated) {
            std::cout << "�޽��� �����ϴ� ��...\n";
            saveMeshToFile("mesh.dat");
            std::cout << "�޽��� mesh.dat�� ����Ǿ����ϴ�.\n";
        }
    }
    else if (key == 27) { // ESC Ű�� ����
        exit(0);
    }
}

// ����� �Է��� ���� SOR ȸ�� ���� ���� ����
void setSORSegmentsFromUserInput() {
    int userAngle;
    bool validInput = false;

    while (!validInput) {
        std::cout << "ȸ�� ���� ������ �Է��ϼ��� (360�� ��� �� �ϳ�): ";
        std::cin >> userAngle;

        if (isDivisorOf360(userAngle)) {
            SOR_SEGMENTS = 360 / userAngle;
            std::cout << "SOR ȸ�� ���� ��: " << SOR_SEGMENTS << std::endl;
            validInput = true;
        }
        else {
            std::cerr << "����: �Էµ� ���� 360�� ����� �ƴմϴ�. �ٽ� �õ��ϼ���.\n";
        }
    }
}

// OpenGL �ʱ�ȭ
void init() {
    setSORSegmentsFromUserInput(); // ����� �Է����� SOR ���� �� ����
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
}

// ���� �Լ�
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
