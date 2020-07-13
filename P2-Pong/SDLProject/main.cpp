//#define GL_SILENCE_DEPRECATION
//
//#ifdef _WINDOWS
//#include <GL/glew.h>
//#endif
//
//#define GL_GLEXT_PROTOTYPES 1
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include "glm/mat4x4.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "ShaderProgram.h"
//
////======================================================================================
//
//SDL_Window* displayWindow;
//bool gameIsRunning = true;
//
//ShaderProgram program;
//glm::mat4 viewMatrix, paddle1, paddle2, ball, projectionMatrix;
//
////Paddle 1----------------------------------
//glm::vec3 paddle1_position = glm::vec3(4.9, 0, 0);
//glm::vec3 paddle1_movement = glm::vec3(0, 0, 0);
//float paddle1_speed = 1.5f; 
//
////Paddle 2----------------------------------
//glm::vec3 paddle2_position = glm::vec3(-4.9, 0, 0);
//glm::vec3 paddle2_movement = glm::vec3(0, 0, 0);
//float paddle2_speed = 1.5f;
//
////Paddle Info-------------------------------
//float paddle_w = 0.5f;
//float paddle_h = 1.5f;
//
////ball--------------------------------------
//float ball_w = 0.3f;
//float ball_h = 0.3f;
//
//glm::vec3 ball_position = glm::vec3(0, 0, 0);
//float ball_speed = 1.5f;
//float ball_x = 1.0;
//float ball_y = 1.0;
//
////======================================================================================
//
//void Initialize() {
//    SDL_Init(SDL_INIT_VIDEO);
//
//    displayWindow = SDL_CreateWindow("Project 2!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//        640, 480, SDL_WINDOW_OPENGL);
//
//    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
//    SDL_GL_MakeCurrent(displayWindow, context);
//
//#ifdef _WINDOWS
//    glewInit();
//#endif
//
//    glViewport(0, 0, 640, 480);
//
//    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
//
//    viewMatrix = glm::mat4(1.0f);
//    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
//
//    program.SetProjectionMatrix(projectionMatrix);
//    program.SetViewMatrix(viewMatrix);
//
//    glUseProgram(program.programID);
//
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //background color
//    
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//}
//
////======================================================================================
//
//void ProcessInput() {
//    paddle1_movement = glm::vec3(0);
//    paddle2_movement = glm::vec3(0);
//    //ball_movement = glm::vec3(0);
//
//    SDL_Event event;
//    while (SDL_PollEvent(&event)) {
//        switch (event.type) {
//        case SDL_QUIT:
//        case SDL_WINDOWEVENT_CLOSE:
//            gameIsRunning = false;
//            break;
//
//        case SDL_KEYDOWN:
//            switch (event.key.keysym.sym) {
//            }
//            break; 
//        }
//    }
//    //--------------------------------------
//    const Uint8* keys = SDL_GetKeyboardState(nullptr);
//
//    //Paddle 1------------------------------
//    if (keys[SDL_SCANCODE_UP]) {
//        paddle1_movement.y = 1.0f;
//    }
//    else if (keys[SDL_SCANCODE_DOWN]) {
//        paddle1_movement.y = -1.0f;
//    } 
//
//    if (glm::length(paddle1_movement) > 1.0f) {
//        paddle1_movement = glm::normalize(paddle1_movement);
//    }
//
//    //Paddle 2------------------------------
//    if (keys[SDL_SCANCODE_W]) {
//        paddle2_movement.y = 1.0f;
//    }
//    else if (keys[SDL_SCANCODE_S]) {
//        paddle2_movement.y = -1.0f;
//    }
//
//    if (glm::length(paddle2_movement) > 1.0f) {
//        paddle2_movement = glm::normalize(paddle2_movement);
//    }
//}
//
////======================================================================================
//bool isColliding() {
//
//    float xdist = fabs(paddle1_position.x - ball_position.x) - ((paddle_w + ball_w) / 2.0f);
//
//    float ydist = fabs(paddle1_position.y - ball_position.y) - ((paddle_h + ball_h) / 2.0f);
//
//    //----------------------------------------------------
//
//    float xdist2 = fabs(paddle2_position.x - ball_position.x) - ((paddle_w + ball_w) / 2.0f);
//
//    float ydist2 = fabs(paddle2_position.y - ball_position.y) - ((paddle_h + ball_h) / 2.0f);
//
//    //----------------------------------------------------
//    if ((xdist < 0 && ydist < 0) || (xdist2 < 0 && ydist2 < 0)){
//        return true;
//    }
//
//    return false;
//}
//
////======================================================================================
//float lastTicks = 0.0f;
//
//void Update() {
//    float ticks = (float)SDL_GetTicks() / 1000.0f;
//    float deltaTime = ticks - lastTicks;
//    lastTicks = ticks;
//
//    float paddle_y_axis = 3.0;
//    // Paddle 1---------------------------------------------------- 
//    paddle1_position += paddle1_movement * paddle1_speed * deltaTime;
//    paddle1 = glm::mat4(1.0f);
//    paddle1 = glm::translate(paddle1, paddle1_position);
//    paddle1 = glm::scale(paddle1, glm::vec3(paddle_w, paddle_h, 1.0f));
//
//    while (paddle1_position.y > paddle_y_axis) {
//        paddle1_position.y = paddle_y_axis;
//    }
//    while (paddle1_position.y < -paddle_y_axis) {
//        paddle1_position.y = -paddle_y_axis;
//    }
//
//    // Paddle 2----------------------------------------------------
//    paddle2_position += paddle2_movement * paddle2_speed * deltaTime;
//    paddle2 = glm::mat4(1.0f);
//    paddle2 = glm::translate(paddle2, paddle2_position);
//    paddle2 = glm::scale(paddle2, glm::vec3(paddle_w, paddle_h, 1.0f));
//
//    while (paddle2_position.y > paddle_y_axis) {
//        paddle2_position.y = paddle_y_axis;
//    }
//    while (paddle2_position.y < -paddle_y_axis) {
//        paddle2_position.y = -paddle_y_axis;
//    }
//
//    //ball----------------------------------------------------------
//    ball_position.x += ball_x * ball_speed * deltaTime;
//    ball_position.y += ball_y * ball_speed * deltaTime;
//    ball = glm::mat4(1.0f);
//    ball = glm::translate(ball, ball_position);
//    ball = glm::scale(ball, glm::vec3(ball_w, ball_h, 1.0f));
//
//    if (ball_position.y > 3.55 || ball_position.y < -3.55) {
//        ball_y *= -1.0f;
//    }
//
//    if (isColliding() == true) {
//        ball_x *= -1.0f;
//    }
//
//    while (ball_position.x > 4.85 || ball_position.x < -4.85) {
//        ball_speed = 0;
//        //cout << "Game Over" << endl;
//    }
//}
//
////======================================================================================
//void DrawPaddle1() {
//    program.SetModelMatrix(paddle1);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
//void DrawPaddle2() {
//    program.SetModelMatrix(paddle2);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
//void DrawBall(){
//    program.SetModelMatrix(ball);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
////======================================================================================
//
//void Render() {
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
//    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
//
//    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
//    glEnableVertexAttribArray(program.positionAttribute);
//
//    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
//    glEnableVertexAttribArray(program.texCoordAttribute);
//
//    DrawPaddle1();
//    DrawPaddle2();
//    DrawBall();
//
//    glDisableVertexAttribArray(program.positionAttribute);
//    glDisableVertexAttribArray(program.texCoordAttribute);
//
//    SDL_GL_SwapWindow(displayWindow); 
//}
//
////======================================================================================
//
//void Shutdown() {
//    SDL_Quit();
//}
//
////======================================================================================
//
//int main(int argc, char* argv[]) {
//    Initialize();
//
//    while (gameIsRunning) {
//        ProcessInput();
//        Update();
//        Render();
//    }
//
//    Shutdown();
//    return 0;
//}
