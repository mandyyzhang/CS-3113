#include "Entity.h"
#include <SDL_mixer.h>
#include <vector>
//=================================================================================================

Entity::Entity()
{

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
        return true;
    }
    return false;

}
//-------------------------------------------
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

//-------------------------------------------
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

void Entity::AIWaitAndGo(Entity* player) {
    switch (aiState) {

    case IDLE:
        if (glm::distance(position, player->position) < 5.0f) {
            aiState = WALKING;
        }
        break;

    case WALKING:
        if (player->position.x < position.x) {
            movement = glm::vec3(-1, 0, 0); //move to the left 
        }
        else {
            movement = glm::vec3(1, 0, 0); //move to the right
        }
        break;

    }
}

//-------------------------------------------
void Entity::AIPatroller() {
    if (position.x < -7.0f) {
        movement = glm::vec3(1, 0, 0);
    }
    else if (position.x > -2.0f) {
        movement = glm::vec3(-1, 0, 0);
    }
}


//-------------------------------------------
void Entity::AIJumper() {
    if (jump == true) {
        velocity.y += jumpPower;
        jump = false;
    }
}

//-------------------------------------------

void Entity::AI(Entity* player) {
    Mix_Chunk* bounce;
    bounce = Mix_LoadWAV("bounce.wav");

    switch (aiType) {

        case WAITANDGO:
            AIWaitAndGo(player);
            break;

        case PATROLLER:
            AIPatroller();
            break;

        case JUMPER:
            AIJumper();
            break;

    }
    if (CheckCollision(player)) {

        if (player->position.y > position.y) {
            isActive = false;
            Mix_PlayChannel(-1, bounce, 0);
        }
        else{
            player->isActive = false;

        }
    }


}



//=================================================================================================
void Entity::Update(float deltaTime, Entity* player, Entity* platforms, int platformCount) {

    if (isActive == false) { //if its not active, then just get out of update -> to not do anything
        return;
    }

    collided = false;

    if (entityType == ENEMY) {
        AI(player);

    }


    if (animIndices != nullptr) {
        //this line of code prevents the object from moving without permission from the user
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f) {//the frame waits for this amount of time
                animTime = 0.0f;
                animIndex++; //goes to next frame

                if (animIndex >= animFrames) { //when reaches last frame
                    animIndex = 0; // loops back to first frame, at index 0
                }
            }
        }
        else {
            animIndex = 0;
        }
    }
   

    if (jump == true) {
        velocity.y += jumpPower; //giving it instance velocity
        jump = false;
    }

    velocity.x = movement.x * speed; //gravity will take care of the y coordinates

    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed


    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

//=================================================================================================

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

//=================================================================================================

void Entity::DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position) {

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

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

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


