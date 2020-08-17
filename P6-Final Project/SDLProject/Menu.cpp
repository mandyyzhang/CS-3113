#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 8
//=================================================================================================
unsigned int menu_data[] =
{
	42, 42, 42, 42, 42, 413, 42, 42, 42, 42, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42,
	42, 28, 28, 28, 28, 28, 28, 28, 28, 28, 42,
	42, 42, 42, 42, 42, 413, 42, 42, 42, 42, 42,

};

//=================================================================================================

void Menu::Initialize() {

	state.nextScene = -1;

	state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, 
		Util::LoadTexture("tile23.png"), 1.0f, 23, 21);

	//-------------------------------------------

	state.player = new Entity();
	state.player->entityType = PLAYER;

	state.player->position = glm::vec3(5, -3.6, 0);
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


}
//=================================================================================================

void Menu::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
}

//=================================================================================================

void Menu::Render(ShaderProgram* program) {

	state.map->Render(program);


	Util::DrawText(program, Util::LoadTexture("font1.png"), "*Castle in the Sky*", 0.65f, -0.25f,
		glm::vec3(state.player->position.x - 3.4f, state.player->position.y + 2.5f, 0));

	Util::DrawText(program, Util::LoadTexture("font1.png"), "(avoid all slimes to reach goal)",
		0.4f, -0.25f,
		glm::vec3(state.player->position.x - 2.4f, state.player->position.y - 1.75f, 0));

	Util::DrawText(program, Util::LoadTexture("font1.png"), "Press Enter!", 0.5f, -0.25f,
		glm::vec3(state.player->position.x - 1.6f, state.player->position.y - 2.5f, 0));

	state.player->Render(program);
}
