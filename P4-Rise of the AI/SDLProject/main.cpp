#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
//=================================================================================================
#define PLATFORM_COUNT 29
#define ENEMY_COUNT 4

Mix_Music* music;

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
    Entity* status;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

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
    displayWindow = SDL_CreateWindow("Project 4!", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    //-------------------------------------------
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    //-------------------------------------------

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-7.5f, 7.5f, -5.625f, 5.625f, -1.0f, 1.0f);
    //projectionMatrix = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.4f, 0.8f, 0.5f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //--------------------------------------------------------------------------
    // Initialize Game Objects

    state.player = new Entity();
    state.platforms = new Entity[PLATFORM_COUNT];
    state.enemies = new Entity[ENEMY_COUNT];
    state.status = new Entity();


    // Initialize Player--------------------------------------------------------
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(-6, -3, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0); //will always be in a state of free fall   
    state.player->speed = 1.5f;

    state.player->textureID = LoadTexture("george_0.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.player->height = 0.8f;
    state.player->width = 0.7f;

    state.player->jumpPower = 8.5f;

    // Initialize Platform------------------------------------------------------

    //Bottom Platform..........................
    for (int i = 0; i < 15; i++) {
        state.platforms[i].position = glm::vec3(-7 + i, -5.2f, 0);
    }

    //2nd Platform..........................
    int add = 0;
    for (int i = 15; i < 21; i++) {
        state.platforms[i].position = glm::vec3(-7 + add, -2.0f, 0);
        add++;
    }

    //3rd Platform..........................
    int add1 = 0;
    for (int i = 21; i < 29; i++) {
        state.platforms[i].position = glm::vec3(7 - add1, 1.0f, 0);
        add1++;
    }

    //Need this loop here to update one time, so that their model matrix will update here 
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].textureID = LoadTexture("platformPack_tile001.png");;
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].Update(0, nullptr, nullptr, 0 );
    }


    // Initialize Enemies-------------------------------------------------------

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = LoadTexture("slime.png");
    }

    state.enemies[0].position = glm::vec3(4, -4.2, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

    state.enemies[1].position = glm::vec3(-7.25, -1 , 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = PATROLLER;

    state.enemies[2].position = glm::vec3(5, 2, 0);
    state.enemies[2].speed = 1;
    state.enemies[2].aiType = JUMPER;
    state.enemies[2].jumpPower = 3.0f;
    state.enemies[2].acceleration = glm::vec3(0, -4.9f, 0);

    state.enemies[3].position = glm::vec3(2, 2, 0);
    state.enemies[3].speed = 1;
    state.enemies[3].aiType = JUMPER;
    state.enemies[3].jumpPower = 3.0f;
    state.enemies[3].acceleration = glm::vec3(0, -4.9f, 0);

}

//=================================================================================================
void ProcessInput() {

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {

            case SDLK_SPACE:
                if (state.player->collided == true) {
                    state.player->jump = true;
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }
    //-------------------------------------------
    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    if (state.status->gameOver == false) {

        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->movement.x = -1.0f;
            state.player->animIndices = state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->movement.x = 1.0f;
            state.player->animIndices = state.player->animRight;
        }

    }
    else {

        state.player->speed = 0.0f;

        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].acceleration.x = 0.0f;
            state.enemies[i].acceleration.y = 0.0f;

            state.enemies[i].velocity.x = 0;
            state.enemies[i].velocity.y = 0;

            state.enemies[i].speed = 0.0f;
        }
      
    }

    //-------------------------------------------

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
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

        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);

        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
        }

        if (state.enemies[2].collided || state.enemies[3].collided) {
            state.enemies[2].jump = true;
            state.enemies[3].jump = true;
        }

        deltaTime -= FIXED_TIMESTEP; //Subtracting from deltaTime, how much time has gone by
    }

    accumulator = deltaTime;
}

//=================================================================================================
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    state.platforms->DrawText(&program, LoadTexture("font1.png"), "Press Space Bar to Jump!",
        0.65f, -0.25f, glm::vec3(-4.5f, 5.0f, 0));

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }

    state.player->Render(&program);

    //int x = 1;
    //int j = ENEMY_COUNT;
    //if (state.enemies[ENEMY_COUNT - x].isActive == false) {
    //    x++;
    //    if (x = j) {
    //        state.status->DrawText(&program, LoadTexture("font1.png"), "You Win!",
    //            1.0f, -0.25f, glm::vec3(-2.5f, 0.0f, 0));
    //        state.status->gameOver = true;

    //    }
    //    
    //} 

    if (state.enemies[0].isActive == false &&
        state.enemies[1].isActive == false &&
        state.enemies[2].isActive == false &&
        state.enemies[3].isActive == false
        ) {
        state.status->DrawText(&program, LoadTexture("font1.png"), "You Win!",
            1.0f, -0.25f, glm::vec3(-2.5f, 0.5f, 0));
        state.status->gameOver = true;
    }

    if (state.player->isActive == false) {
        state.status->DrawText(&program, LoadTexture("font1.png"), "You Failed!",
            1.0f, -0.25f, glm::vec3(-3.5f, 0.5f, 0));
        state.status->gameOver = true;
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



