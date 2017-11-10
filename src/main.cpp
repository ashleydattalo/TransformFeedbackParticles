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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

void glfwSetUp();

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
        "in float inValue;\n"
        "out float outValue;\n"

        "void main()\n"
        "{\n"
            "outValue = sqrt(inValue);\n"
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

    GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    GLint inputAttrib = glGetAttribLocation(program, "inValue");
    glEnableVertexAttribArray(inputAttrib);
    glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);


    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);






    int i = 0;
    // while (!glfwWindowShouldClose(window))
    while (i++ < 100)
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
            glDrawArrays(GL_POINTS, 0, 5);
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);
        glFlush();


        GLfloat feedback[5];
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

        printf("%f %f %f %f %f\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4]);

        for (int i = 0; i < 5; i ++) {
            data[i] = feedback[i];
        }
        printf("%f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 5*sizeof(float), &data[0]);



        // glfwSwapBuffers(window);


    }
    glDeleteProgram(program);
    glDeleteShader(shader);

    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);
    return 0;
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