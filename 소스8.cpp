#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <fstream>  // ���� ����¿� ���
#include <direct.h> 
#include <string>
#include <vector>
#include <iostream>

std::vector<float> points;                      // ����� �Է� ����
std::vector<std::vector<float>> meshVertices;   // ������ SOR �޽� ����
bool isMeshGenerated = false;                   // �޽� ���� ���� Ȯ��
bool wireframeMode = true;                      // ���̾������� ��� Ȱ��ȭ
bool pointsOnlyMode = false;                    // ���鸸 ǥ�� ��� Ȱ��ȭ

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

// �Էµ� ���鸸 ǥ��
void drawPointsOnly() {
    glPointSize(5.0f);
    glColor3f(1.0, 1.0, 1.0); // ���
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

// ���� ���� �߰�
bool animateRotation = false;        // �޽� ȸ�� ���� �÷���
float currentRotationAngle = 0; // ���� ȸ�� ���� (0�� �Ǵ� 30��)

// ȭ�� ����
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawYAxis(); // y�� �׸���

    if (pointsOnlyMode) {
        drawPointsOnly(); // ���鸸 ǥ��
    }
    else {
        drawPointsAndLines(); // �Է� ���� �� �׸���

        if (isMeshGenerated) {
            drawMesh(); // ������ SOR �޽� �׸���
        }
    }

    if (isMeshGenerated) {
        glPushMatrix();

        // �޽� ȸ�� ���� (ī�޶� ��ġ�� �������� ����)
        glRotatef(currentRotationAngle, 1.0f, 1.0f, 1.0f); // y�� �������� ȸ��

        //glScalef(0.5f, 0.5f, 0.5f); // �޽� ũ�� ����
        drawMesh();                 // �޽� �׸���

        glPopMatrix();
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

// �� ������ ���� ���� �Լ�
void saveModel() {
    if (!isMeshGenerated) {
        std::cerr << "Error: �޽��� �������� �ʾҽ��ϴ�. ������ �� �����ϴ�." << std::endl;
        return;
    }

    // �� ���͸� ���� ���� Ȯ�� �� ����
    std::string modelDir = "./model";
    if (_mkdir(modelDir.c_str()) != 0 && errno != EEXIST) {
        std::cerr << "���͸��� ������ �� �����ϴ�: " << modelDir << std::endl;
        return;
    }

    // ����ڷκ��� ���� �̸� �Է¹ޱ�
    std::string filename;
    std::cout << "������ ���� �̸��� �Է��ϼ��� (Ȯ���� ����): ";
    std::cin >> filename;

    // ���� ��� ����
    std::string filepath = modelDir + "/" + filename + ".dat";

    // ���� ����
    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "������ ������ �� �����ϴ�: " << filepath << std::endl;
        return;
    }

    // ���� ���� ����
    file << "VERTEX = " << meshVertices.size() << "\n";
    for (const auto& vertex : meshVertices) {
        file << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
    }

    // �� ���� ����
    file << "FACE = " << (meshVertices.size() / points.size() * (points.size() - 2)) << "\n";
    for (size_t i = 0; i < meshVertices.size() - 1; ++i) {
        if ((i + 1) % points.size() != 0) {
            file << i << " " << i + 1 << " " << (i + points.size()) % meshVertices.size() << "\n";
        }
    }

    file.close();
    std::cout << "���� ����Ǿ����ϴ�: " << filepath << std::endl;
}

// �� ������ ���� �ҷ����� �Լ�
void loadModel() {
    // ����ڷκ��� ���� �̸� �Է¹ޱ�
    std::string filename;
    std::cout << "�ҷ��� ���� �̸��� �Է��ϼ��� (Ȯ���� ����): ";
    std::cin >> filename;

    // ���� ��� ����
    std::string filepath = "./model/" + filename + ".dat";

    // ���� ����
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "������ �� �� �����ϴ�: " << filepath << std::endl;
        return;
    }

    meshVertices.clear();
    points.clear();  // ���鵵 �ʱ�ȭ�Ͽ� �浹�� �����մϴ�.

    // ���� ���� �б�
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

    // �޽��� ����� �ҷ��������� Ȯ�� �� ���� ����
    if (numVertices > 0) {
        isMeshGenerated = true;  // �޽��� �����Ǿ��ٰ� ǥ��
        std::cout << "���� �ҷ��������ϴ�: " << filepath << std::endl;
    }
    else {
        isMeshGenerated = false;  // �ҷ��� ������ ������ �޽� ���� ����
        std::cerr << "�� �ҷ����⿡ �����߽��ϴ�." << std::endl;
    }

    // ȭ�� ���� ��û
    glutPostRedisplay();
}

// Ÿ�̸� �ݹ� �Լ� ����
void timerFunction(int value) {
    if (animateRotation) {
        currentRotationAngle += 2.0f; // ȸ�� ������ �����ϰ� ����
        if (currentRotationAngle >= 360.0f) {
            currentRotationAngle -= 360.0f; // 360���� �ʰ��ϸ� 0���� ����
        }
        glutPostRedisplay(); // ȭ�� ���� ��û
        glutTimerFunc(16, timerFunction, 0); // �� 60FPS�� �ִϸ��̼� ����
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
    else if (key == 'b') {
        pointsOnlyMode = !pointsOnlyMode; // ���鸸 ǥ�� ��� ��ȯ
        glutPostRedisplay();
    }
    else if (key == 'p') {
        saveModel(); // �� ����
    }
    else if (key == 'l') {
        loadModel(); // �� �ҷ�����
    }
    else if (key == 'o') {
        animateRotation = !animateRotation; // ȸ�� �ִϸ��̼� ���� ����
        if (animateRotation) {
            glutTimerFunc(0, timerFunction, 0); // Ÿ�̸� �Լ� ����
        }
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
