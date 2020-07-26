#include "Entity.h"
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
        //lastCollide = other->entityType;
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
                collidedTop = true;
            }

            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
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
                collidedRight = true;
            }

            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}
//-------------------------------------------
void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}
//-------------------------------------------

void Entity::CheckCollisionsX(Map* map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
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
            //Mix_PlayChannel(-1, bounce, 0);
        }
        else {
            player->isActive = false;

        }
    }

}



//=================================================================================================

void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map) {

    if (isActive == false) { //if its not active, then just get out of update -> to not do anything
        return;
    }

    //collidedTop = false;
    //collidedBottom = false;
    //collidedLeft = false;
    //collidedRight = false;

    if (entityType == ENEMY) {
        AI(player);
        //AI(objects);

    }

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;


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
            //this line prevents the awkwardness of movement when the user tells the object to stop
            //the index is resetting to frame 0, where the object looks like its in still motion
            //ex. is not in a jumping position when stopped
        }
    }

    if (jump == true) {
        jump = false;
        velocity.y += jumpPower; //giving it instance velocity
    }

    velocity.x = movement.x * speed; //gravity will take care of the y coordinates

    velocity += acceleration * deltaTime;
    //position += velocity * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount); // Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount); // Fix if needed



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