#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imconfig.h"
#include <iostream>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
// ����ȫ�ֱ���
// ���ڴ�С
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ���λ��
float mouse_x = SCR_WIDTH / 2.0f;
float mouse_y = SCR_HEIGHT / 2.0f;

// �ṹ���
struct Point {
	float x;
	float y;
	Point(float _x, float _y) {
		x = _x;
		y = _y;
	}
};

// bonus
bool bonus = false;

// curve���
// ���Ƶ㡢���ߵ㡢bonus������
vector<Point> ctrlPoints;
vector<Point> curvePoints;
vector<Point> animationPoints;
// t��Ƶ��
float frequency = 1500.0f;
float t_count = 0.0f;
// �Ż���Ⱦ�㣬�����μ���
bool change = false;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		bonus = !bonus;
	}
}

void mouse_pos_callback(GLFWwindow* window, double x, double y) {
	mouse_x = x;
	mouse_y = y;
}

void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		t_count = 0.0f;
		change = true;
		Point p(mouse_x - SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f - mouse_y);
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			ctrlPoints.push_back(p);
			break;
		case GLFW_MOUSE_BUTTON_RIGHT: 
			if (!ctrlPoints.empty()) {
				ctrlPoints.pop_back();
			}
			break;
		default: break;
		}

	}
}

float* createVertices(vector<Point> points, float R, float G, float B) {
	float *vertices = new float[points.size() * 6];
	for (int i = 0; i < points.size(); i++) {
		// λ��
		vertices[i * 6 + 0] = points[i].x / (float)SCR_WIDTH * 2;
		vertices[i * 6 + 1] = points[i].y / (float)SCR_HEIGHT * 2;
		vertices[i * 6 + 2] = 0.0f;
		// ��ɫ
		vertices[i * 6 + 3] = R;
		vertices[i * 6 + 4] = G;
		vertices[i * 6 + 5] = B;
	}
	return vertices;
}

void getCurvePoints(vector<Point> ctrlPoints, int size) {
	curvePoints.clear();
	for (float i = 0; i <= frequency; ++i) {
		float t = i / frequency;
		vector<Point> temp;
		Point p(0, 0);
		for (int i = 0; i < size; i++) {
			p.x = ctrlPoints[i].x * (1 - t) + ctrlPoints[i + 1].x * t;
			p.y = ctrlPoints[i].y * (1 - t) + ctrlPoints[i + 1].y * t;
			temp.push_back(p);
		}
		while (temp.size() != 1) {
			vector<Point> next;
			for (int i = 0; i < temp.size() - 1; i++) {
				p.x = temp[i].x * (1 - t) + temp[i + 1].x * t;
				p.y = temp[i].y * (1 - t) + temp[i + 1].y * t;
				next.push_back(p);
			}
			temp = next;
		}
		curvePoints.push_back(temp[0]);
	}
}

void getAnimationPoints(vector<Point> ctrlPoints, int size) {
	animationPoints.clear();
	if (t_count < frequency) {
		t_count++;
	}
	float t = t_count / frequency;
	vector<Point> temp;
	Point p(0, 0);
	for (int i = 0; i < size; i++) {
		p.x = ctrlPoints[i].x * (1 - t) + ctrlPoints[i + 1].x * t;
		p.y = ctrlPoints[i].y * (1 - t) + ctrlPoints[i + 1].y * t;
		animationPoints.push_back(p);
		temp.push_back(p);
	}
	while (temp.size() != 1) {
		vector<Point> next;
		for (int i = 0; i < temp.size() - 1; i++) {
			p.x = temp[i].x * (1 - t) + temp[i + 1].x * t;
			p.y = temp[i].y * (1 - t) + temp[i + 1].y * t;
			animationPoints.push_back(p);
			next.push_back(p);
		}
		temp = next;
	}
	animationPoints.push_back(temp[0]);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ����ģʽʹ���������
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//	�������ص�
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_btn_callback);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


	// ��ɫ��
	Shader shader("vertexShader.vs", "fragmentShader.fs");

	// ������ʼ��
	// VAO, VBO
	GLuint curveVAO, curveVBO, ctrlVAO, ctrlVBO, animaVAO, animaVBO;


	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// ��Ⱦ
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();

		// ���ƿ��Ƶ�
		float *ctrlPointVertices = createVertices(ctrlPoints, 0.0f, 0.0f, 1.0f);
		// VAO, VBO
		glGenVertexArrays(1, &ctrlVAO);
		glGenBuffers(1, &ctrlVBO);
		glBindVertexArray(ctrlVAO);
		glBindBuffer(GL_ARRAY_BUFFER, ctrlVBO);
		glBufferData(GL_ARRAY_BUFFER, ctrlPoints.size() * 6 * sizeof(GLfloat), ctrlPointVertices, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// ����
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, ctrlPoints.size());
		glPointSize(1.0f);
		glDrawArrays(GL_LINE_STRIP, 0, ctrlPoints.size());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		if (ctrlPointVertices != NULL) {
			delete ctrlPointVertices;
			ctrlPointVertices = NULL;
		}

		if (ctrlPoints.size() > 1) {
			// ��������
			if (change) {
				// ��ȡ BezierCurve ���ߵ�
				getCurvePoints(ctrlPoints, ctrlPoints.size() - 1);
				change = false;
			}

			// �������ߵ�
			float *curveVertices = createVertices(curvePoints, 1.0f, 0.f, 0.0f);
			// VAO, VBO
			glGenVertexArrays(1, &curveVAO);
			glGenBuffers(1, &curveVBO);
			glBindVertexArray(curveVAO);
			glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
			glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * 6 * sizeof(GLfloat), curveVertices, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			// ����
			glPointSize(2.0f);
			glDrawArrays(GL_POINTS, 0, curvePoints.size());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			if (curveVertices != NULL) {
				delete curveVertices;
				curveVertices = NULL;
			}
			
			// ���ƶ���
			if (bonus) {
				getAnimationPoints(ctrlPoints, ctrlPoints.size() - 1);
				float *animationVertices = createVertices(animationPoints, 1.0f, 1.0f, 0.0f);
				// VAO, VBO
				glGenVertexArrays(1, &animaVAO);
				glGenBuffers(1, &animaVBO);
				glBindVertexArray(animaVAO);
				glBindBuffer(GL_ARRAY_BUFFER, animaVBO);
				glBufferData(GL_ARRAY_BUFFER, animationPoints.size() * 6 * sizeof(GLfloat), animationVertices, GL_STREAM_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);
				// ����
				glPointSize(5.0f);
				glDrawArrays(GL_POINTS, 0, animationPoints.size());
				glPointSize(1.0f);
				int current = 0;
				int count = ctrlPoints.size() - 1;
				while (count > 1) {
					glDrawArrays(GL_LINE_STRIP, current, count);
					current += count;
					count--;
				}
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				if (animationVertices != NULL) {
					delete animationVertices;
					animationVertices = NULL;
				}
			}
			
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &curveVAO);
	glDeleteVertexArrays(1, &curveVBO);
	glDeleteBuffers(1, &curveVBO);
	glDeleteVertexArrays(1, &ctrlVAO);
	glDeleteVertexArrays(1, &ctrlVBO);
	glDeleteBuffers(1, &ctrlVBO);
	glDeleteVertexArrays(1, &animaVAO);
	glDeleteVertexArrays(1, &animaVBO);
	glDeleteBuffers(1, &animaVBO);

	glfwTerminate();
	return 0;
}

