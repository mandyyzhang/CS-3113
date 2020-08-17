#include "Level2.h"

#define LEVEL2_ENEMY_COUNT 8

#define LEVEL2_WIDTH 16
#define LEVEL2_HEIGHT 28

//=================================================================================================
unsigned int level2_data[] =
{
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	437, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,//5

	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 28, 28, 28, 42, 42,//14

	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, //18

	42, 28, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	42, 28, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	42, 28, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	42, 28, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	42, 28, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	42, 28, 28, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, //24

	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 437,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42, 42,
	42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, //28



};

//=================================================================================================

void Level2::Initialize() {

	state.nextScene = -1;

	state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, 
		Util::LoadTexture("tile23.png"), 1.0f, 23, 21);


	//-------------------------------------------
	state.player = new Entity();
	state.player->entityType = PLAYER;

	state.player->position = glm::vec3(1, -2, 0);
	state.player->startPos = glm::vec3(1, -2, 0);
	state.player->movement = glm::vec3(0);
	state.player->speed = 3.0f;

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

	state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
	GLuint enemyTextureID = Util::LoadTexture("slime.png");

	for (int i = 0; i < LEVEL2_ENEMY_COUNT; ++i) {
		state.enemies[i].textureID = enemyTextureID;
		state.enemies[i].entityType = ENEMY;
		state.enemies[i].speed = 1.0f;

	}

	for (int i = 0; i <= 3; ++i) {
		state.enemies[i].aiType = UPDOWN;
	}
	state.enemies[0].position = glm::vec3(8, -3, 0);
	state.enemies[1].position = glm::vec3(1, -15, 0);
	state.enemies[2].position = glm::vec3(6, -25, 0);
	state.enemies[3].position = glm::vec3(10, -25, 0);

	for (int i = 4; i <= 7; ++i) {
		state.enemies[i].aiType = RIGHTLEFT;
	}
	state.enemies[4].position = glm::vec3(12, -7, 0);
	state.enemies[5].position = glm::vec3(12, -10, 0);
	state.enemies[6].position = glm::vec3(12, -15, 0);
	state.enemies[7].position = glm::vec3(2, -25, 0);



}

//=================================================================================================

void Level2::Update(float deltaTime) {

	state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

	for (int i = 0; i < LEVEL2_ENEMY_COUNT; ++i) {
		state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
	}

	if (state.player->position.x > 14) {
		state.nextScene = 3;
	}


}

//=================================================================================================

void Level2::Render(ShaderProgram* program) {

	state.map->Render(program);

	Util::DrawText(program, Util::LoadTexture("font1.png"), "Level: 2", 0.45f, -0.25f, 
		glm::vec3(state.player->position.x - 4.55f, state.player->position.y + 3.3f, 0));

	state.player->Render(program);

	for (int i = 0; i < LEVEL2_ENEMY_COUNT; ++i) {
		state.enemies[i].Render(program);
	}

}