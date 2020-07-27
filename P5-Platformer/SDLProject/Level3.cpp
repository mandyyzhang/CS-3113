#include "Level3.h"
#include <SDL_mixer.h>

#define LEVEL3_ENEMY_COUNT 1

#define LEVEL3_WIDTH 18
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
{
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 
	 3, 0, 0, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};
//=================================================================================================

void Level3::Initialize() {

	state.nextScene = -1;

	state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, Util::LoadTexture("tileset.png"),
		1.0f, 4, 1);

	//-------------------------------------------

	state.player = new Entity();
	state.player->entityType = PLAYER;

	state.player->position = glm::vec3(2, 0, 0);
	state.player->movement = glm::vec3(0);
	state.player->acceleration = glm::vec3(0, -9.81f, 0);//will always be in a state of free fall 
	state.player->speed = 2.0f;

	state.player->textureID = Util::LoadTexture("george_0.png");

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
	state.player->width = 0.8f;

	state.player->jumpPower = 6.0f;

	//-------------------------------------------

	state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
	enemyNum = LEVEL3_ENEMY_COUNT;

	state.enemies[0].position = glm::vec3(6, -5, 0);

	state.enemies[0].entityType = ENEMY;
	state.enemies[0].textureID = Util::LoadTexture("slime.png");

	state.enemies[0].position = glm::vec3(13, -4, 0);
	state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
	state.enemies[0].speed = 2.0f;

	state.enemies[0].aiType = WAITANDGO;
	state.enemies[0].aiState = IDLE;



}
//=================================================================================================
void Level3::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

	for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
		state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
	}

	if (enemyNum == 0) {
		winGame = true;
		gameOver = true;
		//Mix_PlayChannel(-1, success, 0);
	}



}
//=================================================================================================
void Level3::Render(ShaderProgram* program) {
	state.map->Render(program);

	state.player->Render(program);

	for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
		state.enemies[i].Render(program);
	}

}