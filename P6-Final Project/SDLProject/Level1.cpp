#include "Level1.h"

#define LEVEL1_ENEMY_COUNT 7

#define LEVEL1_WIDTH 36
#define LEVEL1_HEIGHT 8

//=================================================================================================
unsigned int level1_data[] =
{
	42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 437,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 410, 42, 42, 42, 42, 42, 42
};

//=================================================================================================

void Level1::Initialize() {

	state.nextScene = -1;

	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, 
		Util::LoadTexture("tile23.png"), 1.0f, 23, 21);

	//-------------------------------------------


	state.player = new Entity();
	state.player->entityType = PLAYER;

	state.player->position = glm::vec3(2, -3.3, 0);
	state.player->startPos = glm::vec3(2, -3.3, 0);
	state.player->movement = glm::vec3(0);
	state.player->speed = 2.0f;

	state.player->textureID = Util::LoadTexture("May.png");

	state.player->animDown = new int[4]{ 0, 1, 2, 3 };
	state.player->animLeft = new int[4]{ 4, 5, 6, 7 };
	state.player->animRight = new int[4]{ 8, 9, 10, 11 };
	state.player->animUp = new int[4]{ 12, 13, 14, 15 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 4;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 4;
	state.player->animRows = 4;

	state.player->height = 0.8f;
	state.player->width = 0.8f;

	//-------------------------------------------

	state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
	GLuint enemyTextureID = Util::LoadTexture("slime.png");

	for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
		state.enemies[i].textureID = enemyTextureID;
		state.enemies[i].entityType = ENEMY;
		state.enemies[i].aiType = UPDOWN;
		state.enemies[i].speed = 3.0f;
	}


	state.enemies[0].position = glm::vec3(8, -1, 0);
	state.enemies[1].position = glm::vec3(12, -5, 0);
	state.enemies[2].position = glm::vec3(16, -1, 0);
	state.enemies[3].position = glm::vec3(20, -5, 0);
	state.enemies[4].position = glm::vec3(24, -1, 0);
	state.enemies[5].position = glm::vec3(28, -5, 0);
	state.enemies[6].position = glm::vec3(32, -1, 0);


}

//=================================================================================================

void Level1::Update(float deltaTime) {

	state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

	for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
		state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
	}

	if (state.player->position.x > 34) {
		state.nextScene = 2;
	}

}

//=================================================================================================

void Level1::Render(ShaderProgram* program) {

	state.map->Render(program);

	Util::DrawText(program, Util::LoadTexture("font1.png") , "Level: 1", 0.45f, -0.25f, 
		glm::vec3(state.player->position.x - 4.55f, state.player->position.y + 3.3f, 0));

	state.player->Render(program);

	for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
		state.enemies[i].Render(program);
	}


}