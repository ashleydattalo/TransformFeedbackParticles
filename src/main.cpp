// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SHADER_PATH "/Users/ashleydattalo/graphics/LearnOpenGL/src/Shaders/"

#include "Shader.hpp"
#include "Camera.hpp"

void glfwSetUp();

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();
void printVec(glm::vec3 vec);

GLFWwindow* window;
const GLuint WIDTH = 800, HEIGHT = 600;

bool keys[1024]; 
bool firstMouse = true;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

const GLchar* vertexShaderSrc = 
        "#version 330 core\n"
        "in vec3 pos;\n"
        "in vec3 vel;\n"
        "in vec3 col;\n"
        "out vec3 outValue;\n"

        "void main()\n"
        "{\n"
            "outValue = sqrt(pos);\n"
        "}\n";

//Shader *lightingShader = new Shader(SHADER_PATH "lighting.vert", SHADER_PATH "lighting.frag");
int main()
{
    glfwSetUp();

    GLuint shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);

    const GLchar* feedbackVaryings[] = { "outValue" };
    glTransformFeedbackVaryings(program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(program);
    glUseProgram(program);

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    int numParticles = 10;
    std::vector<glm::vec3> data;
    std::vector<glm::vec3> feedback;

    for (int i = 0; i < numParticles; i++) {
        glm::vec3 pos = glm::vec3(16.0f, 16.0f, 16.0f);
        glm::vec3 vel = glm::vec3(16.0f, 16.0f, 16.0f);
        glm::vec3 col = glm::vec3(16.0f, 16.0f, 16.0f);
        glm::vec3 emptyVec = glm::vec3(0.0f, 0.0f, 0.0f);
        data.push_back(pos);
        data.push_back(vel);
        data.push_back(col);

        feedback.push_back(emptyVec);
    }

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);
    
    GLint posAttrib = glGetAttribLocation(program, "pos");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);

    GLint colAttrib = glGetAttribLocation(program, "vel");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    GLint velAttrib = glGetAttribLocation(program, "col");
    glEnableVertexAttribArray(velAttrib);
    glVertexAttribPointer(velAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));






    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), nullptr, GL_STATIC_READ);


    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    int i = 0;
    // while (!glfwWindowShouldClose(window))
    while (i++ < 10)
    {
        std::cout << "i : " << i << std::endl;
        // Calculate deltatime of current frame
        // GLfloat currentFrame = glfwGetTime();
        // deltaTime = currentFrame - lastFrame;
        // lastFrame = currentFrame;
        // //glfwSetWindowTitle(window, );

        // // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        // glfwPollEvents();
        // do_movement();

        // // Clear the colorbuffer
        // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // // Create camera transformations
        // glm::mat4 view;
        // view = camera.GetViewMatrix();
        // glm::mat4 projection = glm::perspective(camera.Zoom + 20.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 200.0f);


        // Perform transform feedback
        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, numParticles);
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);
        glFlush();


        // GLfloat feedback[5];
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedback.size() * sizeof(glm::vec3), &feedback[0]);

        // printf("%f %f %f %f %f\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4]);

        for (int i = 0; i < numParticles; i ++) {
            data[3*i] = feedback[i];
            printVec(data[3*i]);
        }
        // printf("%f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data.size()*sizeof(glm::vec3), &data[0]);



        // glfwSwapBuffers(window);


    }
    glDeleteProgram(program);
    glDeleteShader(shader);

    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);
    return 0;
}

void printVec(glm::vec3 vec) {
    std::cout << vec.x << " ";
    std::cout << vec.y << " ";
    std::cout << vec.z << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }    
    if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false; 
}

// Moves/alters the camera positions based on user input
void do_movement() {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(keys[GLFW_KEY_J])
        camera.ProcessKeyboard(SPEEDUP, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void glfwSetUp() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "TransformFeedback", nullptr, nullptr);    
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
    }    

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}