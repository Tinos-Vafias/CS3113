#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

struct GameState
{
    Map* map;
    Entity* player;
    Entity* enemies;

    Mix_Music* bgm;
    Mix_Chunk* jump_sfx;

    int next_scene_id;
};

class Scene {
public:
    int m_number_of_enemies = 1;
    int total_swings = 3;
    int ball_direction = 0;

    //for stage 1
    int stagea_swings = 0;
    int stagea_par = 3;
    //for stage 2
    int stageb_swings = 0;
    int stageb_par = 3;

    //for stage 3
    int stagec_swings = 0;
    int stagec_par = 3;


    GameState m_state;

    virtual void initialise(int lives) = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    GameState const get_state() const { return m_state; }
};