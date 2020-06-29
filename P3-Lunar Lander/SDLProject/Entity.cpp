#include "Entity.h"
#include <vector>
//=================================================================================================

Entity::Entity(){

    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);

    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}
//=================================================================================================

bool Entity::CheckCollision(Entity* other) {

    //if either one is not active, then there is no way it should collide
    if (isActive == false || other->isActive == false) {
        return false;
    }

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    // Colliding!
    if (xdist < 0 && ydist < 0) {
        lastCollision = other->entityType;
        return true;
    }
    return false;

}
//=================================================================================================
void Entity::CheckCollisionsY(Entity* objects, int objectCount) {

    for (int i = 0; i < objectCount; i++) {

        Entity* object = &objects[i];

        if (CheckCollision(object)) {

            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));

            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collided = true;
            }

            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collided = true;
            }
        }
    }
}

//=================================================================================================
void Entity::CheckCollisionsX(Entity* objects, int objectCount) {

    for (int i = 0; i < objectCount; i++) {

        Entity* object = &objects[i];

        if (CheckCollision(object)) {

            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));

            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collided = true;
            }

            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collided = true;
            }
        }
    }
}

//=================================================================================================
void Entity::Update(float deltaTime, Entity* objects, int objectCount) {

    if (isActive == false) { //if its not active, then just get out of update -> to not do anything
        return;
    }

    collided = false;

    velocity.x = movement.x * speed; //gravity will take care of the y coordinates

    velocity += acceleration * deltaTime;


    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(objects, objectCount);// Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(objects, objectCount);// Fix if needed

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);


}

//=================================================================================================

void Entity::DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position){

    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {

        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        vertices.insert(vertices.end(), {
             offset + (-0.5f * size), 0.5f * size,
             offset + (-0.5f * size), -0.5f * size,
             offset + (0.5f * size), 0.5f * size,
             offset + (0.5f * size), -0.5f * size,
             offset + (0.5f * size), 0.5f * size,
             offset + (-0.5f * size), -0.5f * size,
        });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });

    } // end of for loop

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

}

//=================================================================================================

void Entity::Render(ShaderProgram* program) {

    if (isActive == false) { //if its not active, then just get out of Render -> to not do anything
        return;
    }

    program->SetModelMatrix(modelMatrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

}
