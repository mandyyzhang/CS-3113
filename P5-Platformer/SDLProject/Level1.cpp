#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] =
{
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
     3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
     3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

//=================================================================================================

void Level1::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0); //will always be in a state of free fall   
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

    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    state.enemies->entityType = ENEMY;

    state.enemies->textureID = Util::LoadTexture("slime.png");

    state.enemies[0].position = glm::vec3(6, 0, 0);
    //state.enemies->movement = glm::vec3(0);

    state.enemies[0].acceleration = glm::vec3(0, -9.8f, 0);
    state.enemies[0].speed = 0.1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

    state.enemies[0].isActive = true;



}

//=================================================================================================

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);


    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }

    if (state.player->position.x >= 12) {
        state.nextScene = 2;
    }
}

//=================================================================================================

void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }

    state.player->Render(program);

    //state.enemies->Render(program);
}
