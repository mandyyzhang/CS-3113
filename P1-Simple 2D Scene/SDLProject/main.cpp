#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//-------------------------------------------------------------------------------

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, GeorgeMatrix, SunMatrix, projectionMatrix;

float George_x = -5.3;
float George_y = -2;

float sun_rotate = 0.0f;

GLuint GeorgeTextureID;
GLuint SunTextureID;

//--------------------------------------------------------------------------------
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == nullptr) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}
//--------------------------------------------------------------------------------

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);

    displayWindow = SDL_CreateWindow("Project 1!", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    GeorgeMatrix = glm::mat4(1.0f);
    SunMatrix = glm::mat4(1.0f);

    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    //looking from -5 to 5, -3.75 to 3.75

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);//teling open Gl to use this shader program when drawing

    glClearColor(0.1f, 0.5f, 1.0f, 0.01f); //background color
    //telling OpenGl that whenever you clear the screen or window, to use this color 
    //and whenever to draw something use this color 

    //Blending 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GeorgeTextureID = LoadTexture("tile003.png");
    SunTextureID = LoadTexture("Sun.png");

}

//--------------------------------------------------------------------------------

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

//--------------------------------------------------------------------------------
float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    //George
    George_x += 1.0f * deltaTime;
    GeorgeMatrix = glm::mat4(1.0f);
    GeorgeMatrix = glm::translate(GeorgeMatrix, glm::vec3(George_x, George_y, 0.0f));
    while (George_x > 5.3) {
        George_x = -5.3;
    }

    //Tile
    sun_rotate += -180.0f * deltaTime;

    SunMatrix = glm::mat4(1.0f);
    SunMatrix = glm::rotate(SunMatrix, glm::radians(sun_rotate),
        glm::vec3(0.0f, 0.0f, 1.0f));
}

//--------------------------------------------------------------------------------

void DrawGeorge() {
    program.SetModelMatrix(GeorgeMatrix);
    glBindTexture(GL_TEXTURE_2D, GeorgeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawSun() {
    program.SetModelMatrix(SunMatrix);
    glBindTexture(GL_TEXTURE_2D, SunTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//--------------------------------------------------------------------------------

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    DrawGeorge();
    DrawSun();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow); //take from memory and display
}

//--------------------------------------------------------------------------------

void Shutdown() {
    SDL_Quit();
}

//--------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
