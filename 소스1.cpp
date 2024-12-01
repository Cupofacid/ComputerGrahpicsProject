#include <GL/glut.h>
#include <vector>
#include <cmath>

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Parameters
std::vector<std::pair<float, float>> points; // 2D points (x, y)
std::vector<std::vector<float>> vertices;   // 3D vertices
std::vector<std::vector<int>> indices;      // Mesh indices
bool isRotated = false;                     // Rotation status
const int numSegments = 100;                // Number of rotation segments

// Draw the Y-axis
void drawYAxis() {
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color for Y-axis
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
}

// Function to draw 2D points
void drawPoints() {
    glPointSize(5.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for points
    glBegin(GL_POINTS);
    for (const auto& p : points) {
        glVertex3f(p.first, p.second, 0.0f);
    }
    glEnd();
}

// Function to generate SOR
void generateSOR() {
    vertices.clear();
    indices.clear();
    int numProfilePoints = points.size();

    // Generate vertices
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float cosTheta = cos(angle);
        float sinTheta = sin(angle);

        for (const auto& p : points) {
            float x = p.first * cosTheta;
            float y = p.second;
            float z = p.first * sinTheta;
            vertices.push_back({ x, y, z });
        }
    }

    // Generate indices
    for (int i = 0; i < numSegments; i++) {
        for (int j = 0; j < numProfilePoints - 1; j++) {
            int p0 = i * numProfilePoints + j;
            int p1 = ((i + 1) % numSegments) * numProfilePoints + j;
            int p2 = p0 + 1;
            int p3 = p1 + 1;

            indices.push_back({ p0, p2, p1 });
            indices.push_back({ p1, p2, p3 });
        }
    }
}

// Function to draw the 3D object
void draw3DObject() {
    glBegin(GL_TRIANGLES);
    glColor3f(0.8f, 0.4f, 0.2f); // Brown color
    for (const auto& face : indices) {
        for (int idx : face) {
            glVertex3f(vertices[idx][0], vertices[idx][1], vertices[idx][2]);
        }
    }
    glEnd();
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0f, 2.0f, 10.0f,  // Camera position
        0.0f, 2.0f, 0.0f,   // Look-at position
        0.0f, 1.0f, 0.0f);  // Up vector

    if (isRotated) {
        draw3DObject();
    }
    else {
        drawYAxis();
        drawPoints();
    }

    glutSwapBuffers();
}

// Mouse callback for adding points
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !isRotated) {
        // 현재 창 크기 가져오기
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // 화면 좌표를 OpenGL 좌표로 변환
        float normX = (2.0f * x / windowWidth) - 1.0f;
        float normY = -(2.0f * y / windowHeight) + 1.0f;

        points.emplace_back(fabs(normX), normY); // 절대값 x로 대칭성 유지
        glutPostRedisplay();
    }
}

// Keyboard callback
void keyboard(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') {
        if (!isRotated && points.size() > 1) {
            generateSOR();
            isRotated = true;
        }
        glutPostRedisplay();
    }
}

// Initialization function
void initGL() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Background color
    glEnable(GL_DEPTH_TEST);             // Enable depth testing

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 800.0f / 600.0f, 1.0f, 50.0f); // Perspective view for 3D
    glMatrixMode(GL_MODELVIEW);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Interactive SOR - Draw Points and Rotate");

    initGL();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
