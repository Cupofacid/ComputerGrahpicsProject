#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>

std::vector<float> points;                      // ����� �Է� ����
std::vector<std::vector<float>> meshVertices;   // ������ SOR �޽� ����
bool isMeshGenerated = false;                   // �޽� ���� ���� Ȯ��
bool wireframeMode = true;                      // ���̾������� ��� Ȱ��ȭ

// ȭ�� ũ��
int windowWidth = 800, windowHeight = 600;

// SOR ȸ�� ���� ��
const int SOR_SEGMENTS = 36;

// �� �� ����
const float PI = 3.14159265358979323846f;

// ���콺 ��ǥ ����ȭ �Լ�
void normalizeMouseCoords(int x, int y, float& normX, float& normY) {
    normX = (2.0f * x / windowWidth) - 1.0f;
    normY = 1.0f - (2.0f * y / windowHeight);
}

// y���� ȭ�鿡 �׸���
void drawYAxis() {
    glColor3f(1.0, 0.0, 0.0); // ������
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
}

// �Է� ���� �� �׸���
void drawPointsAndLines() {
    glColor3f(1.0, 1.0, 1.0); // ���
    glBegin(GL_LINE_STRIP);
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
        std::cerr << "Error: �޽��� �������� �ʾҽ��ϴ�.\n";
        return;
    }

    if (wireframeMode) {
        glColor3f(0.0, 0.5, 1.0); // �Ķ���
        glBegin(GL_LINES);        // ���̾������� ���
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
        glColor3f(0.0, 1.0, 0.0); // ���
        glBegin(GL_TRIANGLES);   // ǥ�� ���
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

// ȭ�� ����
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawYAxis();          // y�� �׸���
    drawPointsAndLines(); // �Է� ���� �� �׸���

    if (isMeshGenerated) {
        drawMesh(); // ������ SOR �޽� �׸���
    }

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
    if (key == 'a') {
        generateSOR(); // SOR �޽� ����
        glutPostRedisplay();
    }
    else if (key == 'w') {
        wireframeMode = !wireframeMode; // ���̾������� ��� ��ȯ
        glutPostRedisplay();
    }
}

// OpenGL �ʱ�ȭ
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
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
