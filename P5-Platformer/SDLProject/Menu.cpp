#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 8

//=================================================================================================

unsigned int menu_data[] =
{
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

//=================================================================================================

void Menu::Initialize() {

	state.nextScene = -1;

	state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, Util::LoadTexture("tileset.png"),
		1.0f, 4, 1);

	//-------------------------------------------

	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(5, 0, 0);
	state.player->movement = glm::vec3(0);
	state.player->acceleration = glm::vec3(0, -9.81f, 0);
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

}

//=================================================================================================
void Menu::Update(float deltaTime) {

	state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);

}

//=================================================================================================

void Menu::Render(ShaderProgram* program) {

	state.map->Render(program);

	GLuint fontTextureID = Util::LoadTexture("font1.png");

	Util::DrawText(program, fontTextureID, "*Slime Kill!*",
		0.75f, -0.25f, glm::vec3(2.0f, -1.5, 0));

	Util::DrawText(program, fontTextureID, "Press Enter! Press Space Bar to Jump!", 
		0.4f, -0.25f, glm::vec3(2.2f, -3.0f, 0));

	state.player->Render(program);
}