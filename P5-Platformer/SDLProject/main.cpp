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
#include <SDL_mixer.h>

#include "Entity.h"
#include "Map.h"
#include "Util.h"

#include "Scene.h"
#include "Menu.h" 
#include "Level1.h"
#include "Level2.h" 
#include "Level3.h"

using namespace std;

//=================================================================================================

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
//--------------------------------------------------------
Mix_Music* music;
Mix_Chunk* bounce;
Mix_Chunk* success;


Scene* currentScene;
Scene* sceneList[4];

void SwitchToScene(Scene* scene) {
	currentScene = scene;
	currentScene->Initialize();
}


int lives = 3;
glm::vec3 livesFont = glm::vec3(0);

//=================================================================================================
void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Project 5!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480, SDL_WINDOW_OPENGL);
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

	bounce = Mix_LoadWAV("bounce.wav");
	Mix_VolumeChunk(bounce, MIX_MAX_VOLUME / 2);

	success = Mix_LoadWAV("success.wav");
	Mix_VolumeChunk(success, MIX_MAX_VOLUME / 4);


	//-------------------------------------------

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);

	glClearColor(0.0f, 0.4f, 0.8f, 0.5f);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	sceneList[0] = new Menu();
	sceneList[1] = new Level1();
	sceneList[2] = new Level2();
	sceneList[3] = new Level3();
	SwitchToScene(sceneList[0]);

}

//=================================================================================================

void ProcessInput() {

	currentScene->state.player->movement = glm::vec3(0);

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
				if (currentScene->state.player->collidedBottom) {
					currentScene->state.player->jump = true;
					Mix_PlayChannel(-1, bounce, 0);
				}
				break;

			} 
			break; 
		}  
	} 

	//-------------------------------------------
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (currentScene->gameOver == false) {

		if (keys[SDL_SCANCODE_LEFT]) {
			currentScene->state.player->movement.x = -1.0f;
			currentScene->state.player->animIndices = currentScene->state.player->animLeft;
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			currentScene->state.player->movement.x = 1.0f;
			currentScene->state.player->animIndices = currentScene->state.player->animRight;
		}

		if ((currentScene == sceneList[0]) && keys[SDL_SCANCODE_RETURN]) {
			SwitchToScene(sceneList[1]);
		}
	}
	else {
		currentScene->state.player->speed = 0.0f;
	}

	//-------------------------------------------
	if (glm::length(currentScene->state.player->movement) > 1.0f) {
		currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
	}

}

//=================================================================================================

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;


void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	deltaTime += accumulator;

	if (deltaTime < FIXED_TIMESTEP) {

		accumulator = deltaTime;

		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {

		currentScene->Update(FIXED_TIMESTEP);

		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;
	//-------------------------------------------

	viewMatrix = glm::mat4(1.0f);

	//if our player x postion is greater than 5, have the screen move 
	if (currentScene == sceneList[0]) {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
		livesFont = glm::vec3(8, -0.5f, 0);
	}
	else {
		if (currentScene->state.player->position.x > 5) {
			viewMatrix = glm::translate(viewMatrix,
				glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
			livesFont = glm::vec3(currentScene->state.player->position.x + 3, -0.5f, 0);

		}

		else { //or else keep it in a fixed position
			viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
			livesFont = glm::vec3(8, -0.5f, 0);
		}
	
	}
	//-------------------------------------------

	if (currentScene->state.player->lastCollision == ENEMY) {

		 
		if (currentScene->state.player->collidedTop ||
			currentScene->state.player->collidedRight ||
			currentScene->state.player->collidedLeft) {

			currentScene->state.player->position = glm::vec3(2, 0, 0);
			lives -= 1;

		}
		else if (currentScene->state.player->collidedBottom) {
			Mix_PlayChannel(-1, bounce, 0);

			currentScene->state.player->lastCollidedObject->isActive = false;
			currentScene->enemyNum -= 1;
		}

		currentScene->state.player->lastCollision = PLATFORM;
	}
	
	 //-------------------------------------------

	if (lives == 0) {
		currentScene->winGame = false;
		currentScene->state.player->isActive = false;
		currentScene->gameOver = true;
	}

}

//=================================================================================================

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	program.SetViewMatrix(viewMatrix);

	currentScene->Render(&program);


	Util::DrawText(&program, Util::LoadTexture("font1.png"),
	"Lives: " + to_string(lives), 0.45f, -0.25f,
	livesFont);


	if (currentScene->gameOver == true) {

		if (currentScene->winGame == true) {

			Util::DrawText(&program, Util::LoadTexture("font1.png"), "You Win!",
				0.75f, -0.25f, glm::vec3((currentScene->state.player->position.x) - 1.8f, -3.5, 0));
			Mix_PlayChannel(-1, success, 0);
		}
		else {
			Util::DrawText(&program, Util::LoadTexture("font1.png"), "You Lose!",
				0.75f, -0.25f, glm::vec3((currentScene->state.player->position.x) + 2, -3.5, 0));
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

		if (currentScene->state.nextScene >= 0) {

			SwitchToScene(sceneList[currentScene->state.nextScene]);

		}

		Render();
	}

	Shutdown();
	return 0;
}