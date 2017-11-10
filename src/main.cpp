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
#include "Texture.hpp"
#include "Camera.hpp"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

const GLuint WIDTH = 800, HEIGHT = 600;

bool keys[1024]; 
bool firstMouse = true;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

ParticleSystem *particles;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);    
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }    

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    Shader *lightingShader = new Shader(SHADER_PATH "lighting.vert", SHADER_PATH "lighting.frag");
    Shader *lampShader = new Shader(SHADER_PATH "lamp.vert", SHADER_PATH "lamp.frag");

    particles = new ParticleSystem();

    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // First, set the container's VAO (and VBO)
    GLuint VBO, containerVAO;
    glGenVertexArrays(1, &containerVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(containerVAO);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // std::cout << deltaTime << std::endl;
        particles->update(deltaTime);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // // Use cooresponding shader when setting uniforms/drawing objects
        // lightingShader->use();
        // GLint lightPosLoc    = glGetUniformLocation(lightingShader->getProg(), "light.position");
        // GLint viewPosLoc     = glGetUniformLocation(lightingShader->getProg(), "viewPos");
        // glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
        // // glUniform3f(lightPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
        // glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
        // // Set lights properties
        // glm::vec3 lightColor = glm::vec3(1.0f);
        // // lightColor.x = sin(glfwGetTime() * 2.0f);
        // // lightColor.y = sin(glfwGetTime() * 0.7f);
        // // lightColor.z = sin(glfwGetTime() * 1.3f);
        // glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // Decrease the influence
        // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // Low influence
        // glUniform3f(glGetUniformLocation(lightingShader->getProg(), "light.ambient"),  ambientColor.x, ambientColor.y, ambientColor.z);
        // glUniform3f(glGetUniformLocation(lightingShader->getProg(), "light.diffuse"),  diffuseColor.x, diffuseColor.y, diffuseColor.z);
        // glUniform3f(glGetUniformLocation(lightingShader->getProg(), "light.specular"), 1.0f, 1.0f, 1.0f);
        // // Set material properties
        // glUniform3f(glGetUniformLocation(lightingShader->getProg(), "material.ambient"),   1.0f, 0.5f, 0.31f);
        // glUniform3f(glGetUniformLocation(lightingShader->getProg(), "material.diffuse"),   1.0f, 0.5f, 0.31f);
        // glUniform3f(glGetUniformLocation(lightingShader->getProg(), "material.specular"),  0.5f, 0.5f, 0.5f); // Specular doesn't have full effect on this object's material
        // glUniform1f(glGetUniformLocation(lightingShader->getProg(), "material.shininess"), 32.0f);

        // GLint lightDirPos = glGetUniformLocation(lightingShader->getProg(), "light.direction");
        // glUniform3f(lightDirPos, -0.2f, -1.0f, -0.3f);  

        // glUniform1f(glGetUniformLocation(lightingShader->getProg(), "light.constant"),  1.0f);
        // glUniform1f(glGetUniformLocation(lightingShader->getProg(), "light.linear"),    0.09);
        // glUniform1f(glGetUniformLocation(lightingShader->getProg(), "light.quadratic"), 0.032);

        // Create camera transformations
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom + 20.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 200.0f);
        // Get the uniform locations
        
        // GLint modelLoc = glGetUniformLocation(lightingShader->getProg(), "model");
        // GLint viewLoc  = glGetUniformLocation(lightingShader->getProg(),  "view");
        // GLint projLoc  = glGetUniformLocation(lightingShader->getProg(),  "projection");
        // // Pass the matrices to the shader
        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        // glm::mat4 model;
        // glBindVertexArray(containerVAO);
        // for (GLuint i = 0; i < 10; i++)
        // {
        //     model = glm::mat4();
        //     model = glm::translate(model, cubePositions[i]);
        //     GLfloat angle = 20.0f * i;
        //     model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
        //     glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }
        // glBindVertexArray(0);

            // // Draw the container (using container's vertex attributes)
            // glBindVertexArray(containerVAO);
            // glm::mat4 model;
            // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // glDrawArrays(GL_TRIANGLES, 0, 36);
            // glBindVertexArray(0);

        // // Also draw the lamp object, again binding the appropriate shader
        // lampShader->use();
        // // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        // modelLoc = glGetUniformLocation(lampShader->getProg(), "model");
        // viewLoc  = glGetUniformLocation(lampShader->getProg(), "view");
        // projLoc  = glGetUniformLocation(lampShader->getProg(), "projection");
        // // Set matrices
        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        // model = glm::mat4();
        // model = glm::translate(model, lightPos);
        // model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // // Draw the light object (using light's vertex attributes)
        // glBindVertexArray(lightVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);

        // std::cout << camera.getCamPos().x;
        // std::cout << " ";
        // std::cout << camera.getCamPos().y;
        // std::cout << " ";
        // std::cout << camera.getCamPos().z;
        // std::cout << std::endl;
        
        particles->render(projection, view);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
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