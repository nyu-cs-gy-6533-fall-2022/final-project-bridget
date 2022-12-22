// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

// OpenGL Mathematics Library
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

// Timer
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

float FOV = 35.0f;
float distance= 5.0;



// VertexBufferObject wrapper
BufferObject VBO;
BufferObject VBO_front;
BufferObject VBO_back;
BufferObject VBO_top;
BufferObject VBO_bottom;
BufferObject VBO_left;
BufferObject VBO_right;

// Contains the vertex positions
std::vector<glm::vec3> V(6);


//CAMERA
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);



//view matrix
glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);

//projection matrix
glm::mat4 projection = glm::perspective(glm::radians(FOV), 800.0f/600.0f, 0.1f, 100.0f);

//model matrix
glm::mat4 model(1.0f);



void update_camera(glm::vec3 newPos)
{
    cameraPos = newPos;
    cameraDirection = glm::normalize(cameraPos-cameraTarget);
    cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    cameraUp = glm::cross(cameraDirection, cameraRight);
    view = glm::lookAt(cameraPos, cameraTarget, up);
}







void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    // Update the position of the first vertex if the left button is pressed

    // Upload the change to the GPU
    VBO.update(V);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {

        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_A:
            update_camera(glm::normalize(cameraPos-0.5f*cameraRight) * distance);
            break;
        case GLFW_KEY_D:
            update_camera(glm::normalize(cameraPos+0.5f*cameraRight) * distance);
            break;
        case GLFW_KEY_S:
            update_camera(glm::normalize(cameraPos-0.5f*cameraUp) * distance);
            break;
        case GLFW_KEY_W:
            update_camera(glm::normalize(cameraPos+0.5f*cameraUp) * distance);
            break;
        case GLFW_KEY_UP:
            distance = distance+0.25;
            update_camera(cameraPos-glm::normalize(cameraDirection)*0.25f);
            break;
        case GLFW_KEY_DOWN:
            update_camera(cameraPos+glm::normalize(cameraDirection)*0.25f);
            distance = distance-0.25;
            break;
        case GLFW_KEY_R:
            update_camera(glm::vec3(0.0f, 0.0f, 5.0f));
            distance = distance+0.25;
            break;
        default:
            break;
    }

    //camera shift differences:
        //camera pos
        //direction assigned in wasd can leave in rest tho!!, distance assigned in up, down, r sooo distance = distance + val
        //param will be: camera pos(vec), val(double)

    // Upload the change to the GPU
    VBO.update(V);
}


int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    std::cout << "OpenGL version recieved: " << major << "." << minor << "." << rev << std::endl;
    std::cout << "Supported OpenGL is " << (const char*)glGetString(GL_VERSION) << std::endl;
    std::cout << "Supported GLSL is " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    VBO.init();


    //Cube vertices
    V.resize(36);
    //front
    V[0] = glm::vec3(-0.5, -0.5, 0.5);
    V[1] = glm::vec3(0.5, -0.5, 0.5);
    V[2] = glm::vec3(0.5, 0.5, 0.5);
    V[3] = glm::vec3(0.5, 0.5, 0.5);
    V[4] = glm::vec3(-0.5, -0.5, 0.5);
    V[5] = glm::vec3(-0.5, 0.5, 0.5);
    //right
    V[6] = glm::vec3(0.5, -0.5, 0.5);
    V[7] = glm::vec3(0.5, -0.5, -0.5);
    V[8] = glm::vec3(0.5, 0.5, 0.5);
    V[9] = glm::vec3(0.5, 0.5, 0.5);
    V[10] = glm::vec3(0.5, -0.5, -0.5);
    V[11] = glm::vec3(0.5, 0.5, -0.5);
     //left
    V[12] = glm::vec3(-0.5, -0.5, 0.5);
    V[13] = glm::vec3(-0.5, 0.5, 0.5);
    V[14] = glm::vec3(-0.5, -0.5, -0.5);
    V[15] = glm::vec3(-0.5, 0.5, -0.5);
    V[16] = glm::vec3(-0.5, -0.5, -0.5);
    V[17] = glm::vec3(-0.5, 0.5, 0.5);
     //top
    V[18] = glm::vec3(0.5, 0.5, 0.5);
    V[19] = glm::vec3(-0.5, 0.5, 0.5);
    V[20] = glm::vec3(-0.5, 0.5, -0.5);
    V[21] = glm::vec3(0.5, 0.5, 0.5);
    V[22] = glm::vec3(0.5, 0.5, -0.5);
    V[23] = glm::vec3(-0.5, 0.5, -0.5);
     //bottom
    V[24] = glm::vec3(-0.5, -0.5, 0.5);
    V[25] = glm::vec3(-0.5, -0.5, -0.5);
    V[26] = glm::vec3(0.5, -0.5, 0.5);
    V[27] = glm::vec3(0.5, -0.5, 0.5);
    V[28] = glm::vec3(-0.5, -0.5, -0.5);
    V[29] = glm::vec3(0.5, -0.5, -0.5);
     //back
    V[30] = glm::vec3(-0.5, -0.5, -0.5);
    V[31] = glm::vec3(0.5, -0.5, -0.5);
    V[32] = glm::vec3(-0.5, 0.5, -0.5);
    V[33] = glm::vec3(-0.5, 0.5, -0.5);
    V[34] = glm::vec3(0.5, -0.5, -0.5);
    V[35] = glm::vec3(0.5, 0.5, -0.5);

    VBO.update(V);

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    // load fragment shader file
    std::ifstream fragShaderFile("../shader/fragment.glsl");
    std::stringstream fragCode;
    fragCode << fragShaderFile.rdbuf();
    // load vertex shader file
    std::ifstream vertShaderFile("../shader/vertex.glsl");
    std::stringstream vertCode;
    vertCode << vertShaderFile.rdbuf();
    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertCode.str(), fragCode.str(), "outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader
    program.bindVertexAttribArray("position",VBO);

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    GLfloat halfScreenWidth = 800.0f / 2;
    GLfloat halfScreenHeight = 600.0f / 2;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program
        program.bind();

        glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, glm::value_ptr(model));

        // Set the uniform value depending on the time difference
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        

        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_TEST);


        //Cube
        glUniform3f(program.uniform("triangleColor"), 1.0f,0.0f,0.0f);
        for(GLint i=0; i < 6;){
            glDrawArrays(GL_LINE_LOOP, i, V.size());
            i = i+3;
        }
        glUniform3f(program.uniform("triangleColor"), 0.0f,1.0f,0.0f);
        for(GLint i=6; i < 12;){
            glDrawArrays(GL_LINE_LOOP, i, V.size());
            i = i+3;
        }
        glUniform3f(program.uniform("triangleColor"), 0.0f,0.0f,1.0f);
        for(GLint i=12; i < 18;){
            glDrawArrays(GL_LINE_LOOP, i, V.size());
            i = i+3;
        }
        glUniform3f(program.uniform("triangleColor"), 1.0f,1.0f,0.0f);
        for(GLint i=18; i < 24;){
            glDrawArrays(GL_LINE_LOOP, i, V.size());
            i = i+3;
        }
        glUniform3f(program.uniform("triangleColor"), 0.0f,1.0f,1.0f);
        for(GLint i=24; i < 30;){
            glDrawArrays(GL_LINE_LOOP, i, V.size());
            i = i+3;
        }
        glUniform3f(program.uniform("triangleColor"), 1.0f,0.0f,1.0f);
        for(GLint i=30; i < 36;){
            glDrawArrays(GL_LINE_LOOP, i, V.size());
            i = i+3;
        }
    
        //glDrawArrays(GL_TRIANGLES, 0, V.size());
       //DrawCube( halfScreenWidth, halfScreenHeight, -500, 200 );


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
