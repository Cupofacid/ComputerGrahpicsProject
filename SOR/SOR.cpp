#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <fstream>  // ���� ����� ���� ���
#include <cstdlib> // system �Լ� ����� ���� ���
using namespace std;

std::vector<float> points;                      // ����� �Է� ����
std::vector<std::vector<float>> meshVertices;   // ������ SOR �޽� ����
bool isMeshGenerated = false;                   // �޽� ���� ���� Ȯ��
bool wireframeMode = true;                      // ���̾������� ��� Ȱ��ȭ

int windowWidth = 1000, windowHeight = 900; // ȭ�� ũ��

// SOR ȸ�� ���� �� (����� �Է¿� ���� �������� ����)
int SOR_SEGMENTS = 36; // �⺻���� 36

const float PI = 3.14159265358979323846f; // �� �� ����

char rotationAxis = 'y'; // �⺻ ȸ�� �� (y��)

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
// x���� ȭ�鿡 �׸���
void drawXAxis() {
    glColor3f(0.0, 0.0, 0.0); // ������
    glBegin(GL_LINES);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();
}

// z���� ȭ�鿡 �׸���
void drawZAxis() {
    glColor3f(0.0, 0.0, 0.0); // �Ķ���
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
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

// ȭ�鿡 �ؽ�Ʈ ��� �Լ�
void drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y); // �ؽ�Ʈ ��ġ ����
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // �۲ð� ���� ���
    }
}

// Ű���� ��ǲ ���� ���
void displayKeyboardHelp() {
    glColor3f(0.0, 0.0, 0.0); // �ؽ�Ʈ ����: ����
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
            float rotatedX = x, rotatedY = y, rotatedZ = 0.0f;

            // ������ �࿡ ���� ȸ�� ��ȯ ����
            if (rotationAxis == 'x') {
                // x�� ���� ȸ�� (y, z ��ǥ�� ��ȯ)
                rotatedY = cosTheta * y - sinTheta * rotatedZ;
                rotatedZ = sinTheta * y + cosTheta * rotatedZ;
                rotatedX = x; // x ��ǥ�� �״�� ����
            }
            else if (rotationAxis == 'y') {
                // y�� ���� ȸ�� (x, z ��ǥ�� ��ȯ)
                rotatedX = cosTheta * x + sinTheta * rotatedZ;
                rotatedZ = -sinTheta * x + cosTheta * rotatedZ;
                rotatedY = y; // y ��ǥ�� �״�� ����
            }
            else if (rotationAxis == 'z') {
                // z�� ���� ȸ�� (x, y ��ǥ�� ��ȯ)
                rotatedX = cosTheta * x - sinTheta * y;
                rotatedY = sinTheta * x + cosTheta * y;
                rotatedZ = 0.0f; // z ��ǥ�� �״�� ����
            }

            // ������ ������ �޽��� �߰�
            meshVertices.push_back({ rotatedX, rotatedY, rotatedZ });
        }
    }

    isMeshGenerated = true;
    std::cout << "SOR �޽� ���� �Ϸ�. �� ���� ��: " << meshVertices.size() << "\n";
}

// �־��� ���� �������� ������ ȸ��
void rotatePoints(char axis) {
    float angleRadians = (2.0f * PI) / SOR_SEGMENTS; // 360���� SOR_SEGMENTS�� ���� ����
    float cosTheta = cos(angleRadians);
    float sinTheta = sin(angleRadians);

    for (size_t i = 0; i < points.size(); i += 2) {
        float x = points[i];
        float y = points[i + 1];
        float rotatedX = x, rotatedY = y, rotatedZ = 0.0f;

        if (axis == 'x') {
            // x�� ���� ȸ�� (y, z ��ǥ�� ��ȯ)
            rotatedY = cosTheta * y - sinTheta * rotatedZ;
            rotatedZ = sinTheta * y + cosTheta * rotatedZ;
        }
        else if (axis == 'y') {
            // y�� ���� ȸ�� (x, z ��ǥ�� ��ȯ)
            rotatedX = cosTheta * x + sinTheta * rotatedZ;
            rotatedZ = -sinTheta * x + cosTheta * rotatedZ;
        }
        else if (axis == 'z') {
            // z�� ���� ȸ�� (x, y ��ǥ�� ��ȯ)
            rotatedX = cosTheta * x - sinTheta * y;
            rotatedY = sinTheta * x + cosTheta * y;
        }

        // �� ���� (���� �� ���� ����� x, y�� ���Ե� -> z�� ����)
        points[i] = rotatedX;
        points[i + 1] = rotatedY;
    }
}

// �𵨰� �� �ʱ�ȭ �Լ�
void resetModel() {
    points.clear();         // �Էµ� �� �ʱ�ȭ
    meshVertices.clear();   // �޽� ���� �ʱ�ȭ
    isMeshGenerated = false; // �޽� ���� ���� �ʱ�ȭ
    wireframeMode = true;   // �⺻ ���̾������� ��� Ȱ��ȭ
    std::cout << "�� �ʱ�ȭ �Ϸ�. ���ο� ���� �Է��ϼ���.\n";
    glutPostRedisplay();    // ȭ�� ����
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
        glColor3f(1.0, 1.0, 0.0); // �Ķ���
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

// �޽��� OBJ ���Ϸ� �����ϴ� �Լ�
void saveMeshToOBJ(const std::string& filename) {
    std::string Path = "SORdata/" + filename; // ���� ��θ� D:/Meshes/�� ����
    std::ofstream outFile(Path);
    if (!outFile) {
        std::cerr << "������ �� �� �����ϴ�: " << Path << std::endl;
        return;
    }

    // OBJ ���
    outFile << "# SOR Mesh exported as OBJ format\n";

    // ���� ���� ����
    for (const auto& vertex : meshVertices) {
        outFile << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
    }

    // ������(face) ���� ����
    size_t numPoints = points.size() / 2; // �Է� �� ����
    for (int i = 0; i < SOR_SEGMENTS; ++i) {
        int nextSegment = (i + 1) % SOR_SEGMENTS;

        for (size_t j = 0; j < numPoints - 1; ++j) {
            int idx1 = i * numPoints + j + 1; // OBJ �ε����� 1���� ����
            int idx2 = idx1 + 1;
            int idx3 = nextSegment * numPoints + j + 1;
            int idx4 = idx3 + 1;

            outFile << "f " << idx1 << " " << idx2 << " " << idx3 << "\n";
            outFile << "f " << idx2 << " " << idx4 << " " << idx3 << "\n";
        }
    }

    outFile.close();
    std::cout << "�޽��� " << Path << "�� ����Ǿ����ϴ�.\n";
}

// ȭ�� ����
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawXAxis(); // x�� �׸���
    drawYAxis(); // y�� �׸���
    drawZAxis(); // z�� �׸���
    drawPointsOnly(); // ���鸸 ǥ��
    drawMesh();
    // ȭ�� ���� ���� Ű���� �Է� ���� ǥ��
    glPushMatrix();
    glLoadIdentity();    // �𵨺� ��� �ʱ�ȭ
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // 2D ��� ����
    displayKeyboardHelp();           // Ű���� �Է� ���� ���
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
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

// Ű���� �̺�Ʈ ó��
void keyboardFunc(unsigned char key, int x, int y) {
    if (key == '\r' || key == '\n') {  // Enter Ű�� ������
        generateSOR(); // SOR �޽� ����
        glutPostRedisplay();
    }
    else if (key == 'x' || key == 'y' || key == 'z') {
        rotationAxis = key; // ȸ�� �� ����
        std::cout << "���� ȸ�� ��: " << rotationAxis << "��\n";
    }
    else if (key == 'w') {
        wireframeMode = !wireframeMode; // ���̾������� <-> �� ��� ��ȯ
        glutPostRedisplay();
    }
    else if (key == 's') {  // 's' Ű�� ������ �޽��� ����
        if (isMeshGenerated) {
            std::cout << "������ �̸��� �Է��Ͻÿ�: ";
            std::string filename;
            std::cin >> filename;
            filename += ".obj";
            saveMeshToOBJ(filename);
        }
    }
    else if (key == 'q') { // 'q' Ű�� ���� �ʱ�ȭ
        resetModel(); // �� �ʱ�ȭ
    }
    else if (key == 'g') { // 'g' Ű�� ���� .exe ���� ����
        std::cout << "Maze.exe";
        system("Maze.exe");
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
    glClearColor(0.5, 0.5, 0.5, 1.0);
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