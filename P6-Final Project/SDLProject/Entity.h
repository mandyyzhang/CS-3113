#pragma once
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
#include "Map.h" 
//-------------------------------------------

enum EntityType { PLAYER, PLATFORM, ENEMY };
enum AIType { RIGHTLEFT, UPDOWN };

//-------------------------------------------
class Entity {
public:

	EntityType entityType;
	AIType aiType;

	EntityType lastCollision;

	glm::vec3 position;
	glm::vec3 movement;
	glm::vec3 acceleration;
	glm::vec3 velocity;
	glm::vec3 startPos;

	float width = 1;
	float height = 1;

	float speed;

	GLuint textureID;

	glm::mat4 modelMatrix;

	int* animRight = nullptr;
	int* animLeft = nullptr;
	int* animUp = nullptr;
	int* animDown = nullptr;

	int* animIndices = nullptr;
	int animFrames = 0;
	int animIndex = 0;
	float animTime = 0;
	int animCols = 0;
	int animRows = 0;

	bool isActive = true;

	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;
	bool goToTop = false;
	bool goToRight = false;


	//-------------------------------------------
	Entity();

	bool CheckCollision(Entity* other);
	void CheckCollisionsY(Entity* objects, int objectCount);
	void CheckCollisionsX(Entity* objects, int objectCount);

	void CheckCollisionsX(Map* map);
	void CheckCollisionsY(Map* map);


	void AI(Entity* player);
	void AIRightLeft();
	void AIUpDown();

	void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);



};