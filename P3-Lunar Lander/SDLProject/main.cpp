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

#include "Entity.h"
//=================================================================================================
#define PLATFORM_COUNT 27

struct GameState {
    Entity* ship;
    Entity* platforms;
    Entity* status;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameOver = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
//=================================================================================================
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
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
//=================================================================================================

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander!", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.2f, 0.3f, 0.5f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //--------------------------------------------------------------------------
    // Initialize Game Objects


    state.ship = new Entity();
    state.status = new Entity();;
    state.platforms = new Entity[PLATFORM_COUNT];

    // Initialize Ship----------------------------------------------------------

    state.ship->textureID = LoadTexture("playerShip2_blue.png");
    state.ship->entityType = PLAYER;

    state.ship->position = glm::vec3(0, 3.75, 0);
    state.ship->movement = glm::vec3(0);
    state.ship->acceleration = glm::vec3(0, -0.45f, 0); //will always be in a state of free fall   
    state.ship->speed = 2.0f;

    //Initialize Platform-------------------------------------------------------- 

    for (int i = 0; i <= 25; i++) {
        state.platforms[i].textureID = LoadTexture("platformPack_tile040.png");;
        state.platforms[i].entityType = PLATFORM;
    }
 
    //Bottom Platform..........................
    for (int i = 0; i <= 5; i++) {
        state.platforms[i].position = glm::vec3(-4.5 + i, -3.25f, 0);
    }

    int add = 0;
    for (int i = 6; i <= 7; i++) {
        state.platforms[i].position = glm::vec3(3.5 + add, -3.25f, 0);
        add++;
    }

    //Left Side................................
    int add1 = 0;
    for (int i = 8; i <= 14; i++) {
        state.platforms[i].position = glm::vec3(-4.5, -2.25f + add1, 0);
        add1++;
    }

    //Right Side...............................
    int add2 = 0;
    for (int i = 15; i <= 21; i++) {
        state.platforms[i].position = glm::vec3(4.5, -2.25f + add2, 0);
        add2++;
    }
    
    //Floating................................
    state.platforms[22].position = glm::vec3(-2.5, 2.0f, 0);
    state.platforms[23].position = glm::vec3(-1.5, 2.0f, 0);
    state.platforms[24].position = glm::vec3(1.5, 0.5f, 0);
    state.platforms[25].position = glm::vec3(2.5, 0.5f, 0);

    //Initialize Landing Place----------------------------------------------------

    state.platforms[26].textureID = LoadTexture("platformPack_tile034.png");
    state.platforms[26].entityType = LANDING;
    state.platforms[26].position = glm::vec3(2.0, -3.25f, 0);

   
    //Need this loop here to update one time, so that their model matrix will update here 
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, nullptr, 0);
    }




}

//=================================================================================================
void ProcessInput() {

    state.ship->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        }
    }
    //-------------------------------------------
    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    if (gameOver == false) {

        if (keys[SDL_SCANCODE_LEFT]) {
            state.ship->acceleration.x -= 0.07f;
        }

        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.ship->acceleration.x += 0.07f;
        }
    }
    else {

        state.ship->acceleration.x = 0.0f;
        state.ship->acceleration.y = 0.0f;

        state.ship->velocity.x = 0;
        state.ship->velocity.y = 0;
    }
    //-------------------------------------------

    if (glm::length(state.ship->movement) > 1.0f) {
        state.ship->movement = glm::normalize(state.ship->movement);
    }

}

//=================================================================================================
#define FIXED_TIMESTEP 0.0166666f //60 times a second

float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks; //Get how much time has pasted
    lastTicks = ticks;

    deltaTime += accumulator;

    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {

        state.ship->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);

        if (state.ship->collided) {
            gameOver = true;
        }

        deltaTime -= FIXED_TIMESTEP; 
    }

    accumulator = deltaTime;

}

//=================================================================================================
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    state.ship->Render(&program);

    if (gameOver == true) {

        if (state.ship->lastCollision == LANDING) {
            state.status->DrawText(&program, LoadTexture("font1.png"), "Mission Successful!",
                0.5f, -0.25f, glm::vec3(-2.2f, 0.5f, 0));;
        }

        else if (state.ship->lastCollision == PLATFORM) {
            state.status->DrawText(&program, LoadTexture("font1.png"), "Mission Failed!",
                0.5f, -0.25f, glm::vec3(-1.8f, 0.5f, 0));;

        }
           
    }

    SDL_GL_SwapWindow(displayWindow);

}

//=================================================================================================
void Shutdown() {
    SDL_Quit();
}

//=================================================================================================
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
