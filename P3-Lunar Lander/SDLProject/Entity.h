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

enum EntityType {PLAYER, PLATFORM, LANDING};

class Entity {
public:

    EntityType entityType;
    EntityType lastCollision;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float speed;

    float width = 1;
    float height = 1;

    GLuint textureID;

    glm::mat4 modelMatrix;

    bool isActive = true;
    bool collided = false;

    Entity();

    //Functions------------------------------
    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);

    void Update(float deltaTime, Entity* objects, int objectCount);

    void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
        float size, float spacing, glm::vec3 position);

    void Render(ShaderProgram* program);

 
};
