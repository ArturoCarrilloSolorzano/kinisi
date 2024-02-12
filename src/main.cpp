/*
Compilation on Linux
g++ -std=c++17 ./src/*.cpp -o prog  ./src/glad.c -I./include -lSDL2 -ldl
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/vec3.hpp>
#include <glm-master/glm/mat4x4.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>

// C++ standard template library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Our libraries

#include "./Camera.hpp"

// GLOBALS
int gScreenHeight = 480;
int gScreenWidth = 640;

SDL_Window *gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGlContext = nullptr;

bool gQuit = false;

// VAO
GLuint gVertexArrayObject = 0;
// VBO
GLuint gVertexBufferObject = 0;
// IBO
GLuint gIndexBufferObject = 0;

// Program Object (for our shaders)
GLuint gGraphicsPipelineShaderProgram = 0;

float g_uOffset = -2.0f;
float g_uRotate = 0.0f;
float g_uScale = 0.5f;

Camera gCamera;
// Error Handler

static void
GLClearAllErrors()
{
    while (glGetError() != GL_NO_ERROR)
    {
    }
}

static bool GLCheckErrorStatus(const char *function, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGL Error: " << error
                  << "\tLine: " << line
                  << "\tFunction: " << function << std::endl;
        return true;
    }
    return false;
}

#define GLCheck(x)      \
    GLClearAllErrors(); \
    x;                  \
    GLCheckErrorStatus(#x, __LINE__);

std::string LoadShaderAsString(const std::string &filename)
{
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if (myFile.is_open())
    {
        while (std::getline(myFile, line))
        {
            result += line + '\n';
            /* code */
        }
        myFile.close();
    }
    return result;
}

void GetOpenGlVersionInfo()
{
    std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void VertexSpecification()
{

    // Lives on the CPU
    const std::vector<GLfloat> vertexData{
        // Front Face
        -0.5f, -0.5f, 0.0f, // vertex 1 Left
        1.f, 1.f, 0.0f,     // color

        0.5f, -0.5f, 0.0f, // vertex 2 Right
        1.f, 1.f, 0.0f,    // color

        -0.5f, 0.5f, 0.0f, // vertex 3 Top Left
        1.f, 1.f, 0.0f,    // color

        // Second Triangle

        0.5f, 0.5f, 0.0f, // vertex 3 Top Right
        1.f, 1.f, 0.0f,   // color

        // Back Face
        -0.5f, -0.5f, -1.0f, // vertex 1 Left
        0.5f, 1.0f, 0.5f,    // color

        0.5f, -0.5f, -1.0f, // vertex 2 Right
        0.5f, 1.0f, 0.5f,   // color

        -0.5f, 0.5f, -1.0f, // vertex 3 Top Left
        0.5f, 1.0f, 0.5f,   // color

        // Second Triangle

        0.5f, 0.5f, -1.0f, // vertex 3 Top Right
        0.5f, 1.0f, 0.5f,  // color

        // Top Face
        -0.5f, 0.5f, 0.0f, // vertex 1 Left
        1.f, 0.5f, 0.0f,   // color

        0.5f, 0.5f, 0.0f, // vertex 2 Right
        1.f, 0.5f, 0.0f,  // color

        -0.5f, 0.5f, -1.0f, // vertex 3 Top Left
        1.f, 0.5f, 0.0f,    // color

        // Second Triangle

        0.5f, 0.5f, -1.0f, // vertex 3 Top Right
        1.f, 0.5f, 0.0f,   // color

    };

    const std::vector<GLuint> indexBufferData{
        // FrontFace
        2,
        0,
        1,
        3,
        2,
        1,
        // BackFace
        6,
        4,
        5,
        7,
        6,
        5,
        // TopFace
        10,
        8,
        9,
        11,
        10,
        9};

    // Setting things up on the gpu
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // Start generating our VBO
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    glBufferData(GL_ARRAY_BUFFER,
                 vertexData.size() * sizeof(GL_FLOAT),
                 vertexData.data(),
                 GL_STATIC_DRAW);

    // Setup Index Buffer Object (IBO)

    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferData.size() * sizeof(GLuint),
                 indexBufferData.data(),
                 GL_STATIC_DRAW);

    // Position information
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT, // X, Y, Z, R, G, B
                          GL_FALSE,
                          sizeof(GL_FLOAT) * 6,
                          (GLvoid *)0);

    // Color information
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT, // X, Y, Z, R, G, B
                          GL_FALSE,
                          sizeof(GL_FLOAT) * 6,
                          (GLvoid *)(sizeof(GL_FLOAT) * 3));
    // Cleanup
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
}

GLuint CompileShader(GLuint type, const std::string &source)
{
    GLuint shaderObject;

    if (type == GL_VERTEX_SHADER)
    {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (type == GL_FRAGMENT_SHADER)
    {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }
    const char *src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    int result;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        char *errorMessages = new char[length];
        glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

        if (type == GL_VERTEX_SHADER)
        {
            std::cout << "ERROR: GL_VERTEX_SHADER compilation failed! \n"
                      << errorMessages << std::endl;
        }
        else if (type == GL_FRAGMENT_SHADER)
        {
            std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed! \n"
                      << errorMessages << std::endl;
        }

        delete[] errorMessages;
        glDeleteShader(shaderObject);
        return 0;
    }

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string &vertexshadersource, const std::string &fragmentshadersource)
{
    GLuint programObject = glCreateProgram();

    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);

    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    // Validate program

    glValidateProgram(programObject);

    // glDetachShader, deleteShader

    return programObject;
}
void CreateGraphicsPipeline()
{
    std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");
    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void InitializeProgram()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL2 could not initilize video subsystem"
                  << std::endl;
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    gGraphicsApplicationWindow = SDL_CreateWindow("My engine",
                                                  0, 0,
                                                  gScreenWidth, gScreenHeight,
                                                  SDL_WINDOW_OPENGL);

    if (gGraphicsApplicationWindow == nullptr)
    {
        std::cout << "SDL2_Window was not able to be created"
                  << std::endl;
        exit(1);
    }

    gOpenGlContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);

    if (gOpenGlContext == nullptr)
    {
        std::cout << "Opengl context not available"
                  << std::endl;
        exit(1);
    }

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // initialize the Glad Library
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        std::cout << "Glad was not initialized"
                  << std::endl;
        exit(1);
    }

    GetOpenGlVersionInfo();
}

void Input()
{
    SDL_Event e;
    float speed = 0.1f;
    int mouseX, mouseY;

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            std::cout << "Adios" << std::endl;
            gQuit = true;
        }
        else if (e.type == SDL_MOUSEMOTION)
        {
            mouseX += e.motion.xrel;
            mouseY += e.motion.yrel;
            gCamera.MouseLook(mouseX, mouseY);
        }
    }

    // g_uOffset -= 0.01f;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W])
    {
        gCamera.MoveForward(speed);
    }
    if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S])
    {
        gCamera.MoveBackward(speed);
    }
    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A])
    {
        gCamera.MoveLeft(speed);
    }
    if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D])
    {
        gCamera.MoveRight(speed);
    }
}

void PreDraw()
{
    // setting gl

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor(0.49412f, 0.50588f, 0.45490f, 1.f);

    // Clear color buffer and depth buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // use our shader
    glUseProgram(gGraphicsPipelineShaderProgram);

    g_uRotate -= 0.1f;
    // std::cout << "g_uRotate: " << g_uRotate << std::endl;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, g_uOffset));

    // Model transformation by translating our object
    model = glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    // Rotation matrix

    model = glm::scale(model, glm::vec3(g_uScale, g_uScale, g_uScale));

    GLint u_ModelMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix");

    if (u_ModelMatrixLocation >= 0)
    {
        glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
    }
    else
    {
        std::cout << "Could not find u_ModelMatrix, maybe a misspelling?" << std::endl;
        exit(EXIT_FAILURE);
    }

    glm::mat4 view = gCamera.GetViewMatrix();

    GLint u_ViewLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ViewMatrix");

    if (u_ViewLocation >= 0)
    {
        glUniformMatrix4fv(u_ViewLocation, 1, GL_FALSE, &view[0][0]);
    }
    else
    {
        std::cout << "Could not find u_ViewMatrix, maybe a misspelling?" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Projection matrix in perspective
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)gScreenWidth / (float)gScreenHeight,
                                             0.1f,
                                             10.0f);
    // Retrieve our location of our perspective matrix uniform
    GLint u_ProjectionLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Perspective");

    if (u_ProjectionLocation >= 0)
    {
        glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);
    }
    else
    {
        std::cout << "Could not find u_Perspective, maybe a misspelling?" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Draw()
{

    // Enable our attributes
    glBindVertexArray(gVertexArrayObject);

    // Render data
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    /*
        glDrawElements(GL_TRIANGLES,
                   6,
                   GL_UNSIGNED_INT,
                   0);
    */
    GLCheck(glDrawElements(GL_TRIANGLES,
                           18,
                           GL_UNSIGNED_INT,
                           0));
    // Stop using our current graphics pipeline
    glUseProgram(0);
}

void MainLoop()
{
    SDL_WarpMouseInWindow(gGraphicsApplicationWindow, gScreenWidth / 2, gScreenHeight / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (!gQuit)
    {
        Input();

        PreDraw();

        Draw();

        SDL_GL_SwapWindow(gGraphicsApplicationWindow);
    }
}

void Cleanup()
{
    std::cout << "triangle" << gVertexArrayObject << std::endl;
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}

int main()
{
    // setup graphics program
    InitializeProgram();

    // setup our geometry
    VertexSpecification();

    // Create graphics pipeline
    CreateGraphicsPipeline();

    // Call the main application loop
    MainLoop();

    // Call de cleanup function
    Cleanup();

    return 0;
}