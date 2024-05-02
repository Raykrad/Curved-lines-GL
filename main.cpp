#include <GLFW/glfw3.h>
#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <cmath>

struct Point {
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
};

std::vector<Point> controlPoints;

bool isDragging = false;
int draggedPointIndex = -1;

void drawPoint(float x, float y) {
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

int factorial(int n) {
    int result = 1;
    for(int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

float polynom(int i, int n, float t) {
    return (factorial(n)/(factorial(i) *
                          factorial(n - i)))* (float)pow(t, i) *
           (float)pow(1 - t, n - i);
}

void drawBezierCurve() {
    glLineWidth(2);
    glColor3f(0.5, 0.0, 0.5);

    int n = controlPoints.size() - 1;
    if (n < 1) return;
    glBegin(GL_LINE_STRIP);

    int j = 0;
    float step = 0.01;
    float t = 0;
    while (t < 1) {
        float ytmp = 0;
        float xtmp = 0;
        for (size_t i = 0; i < controlPoints.size(); i++) {
            float b = polynom(i, controlPoints.size() - 1, t);
            xtmp += controlPoints[i].x * b;
            ytmp += controlPoints[i].y * b;
        }
        glVertex2f(xtmp, ytmp);
        j++;
        t += step;
    }
    glEnd();
}

void mouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        if (!isDragging) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            controlPoints.push_back(Point((float) xpos, (float) (height - ypos)));
            glfwPostEmptyEvent();
        } else {
            isDragging = false;
        }
    }
}

void mouseMotion(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging && draggedPointIndex >= 0 && draggedPointIndex < controlPoints.size()) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        controlPoints[draggedPointIndex].x = xpos;
        controlPoints[draggedPointIndex].y = height - ypos;
        glfwPostEmptyEvent();
    }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        controlPoints.clear();
    }
}

void display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0); // Black color

    for (int i = 0; i < controlPoints.size(); ++i) {
        drawPoint(controlPoints[i].x, controlPoints[i].y);
    }
    if (controlPoints.size() >= 2) {
        drawBezierCurve();
    }

    glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouseDrag(GLFWwindow* window, double xpos, double ypos) {
    if (!isDragging) {
        for (int i = 0; i < controlPoints.size(); ++i) {
            if (abs(controlPoints[i].x - xpos) <= 5 && abs(controlPoints[i].y - ypos) <= 5) {
                isDragging = true;
                draggedPointIndex = i;
                break;
            }
        }
    }
}

int main() {

    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Curved Line", NULL, NULL);
    if (window == NULL) {
        std::cout << "Epic GLFW Failure" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // ImGui в коде не работает, оставляю только потому, что по некоей причине он улучшает работу программы
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glfwSetMouseButtonCallback(window, mouseClick);
    glfwSetCursorPosCallback(window, mouseMotion);
    glfwSetKeyCallback(window, keyboard);

    while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Interaction Panel");
    if (ImGui::Button("Clear Control Points")) {
        controlPoints.clear();
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    display(window);

    glfwPollEvents();
}
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
