#include "LevelS.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

// Trim everything down after

unsigned int LEVELS_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

LevelS::~LevelS()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelS::initialise(int lives)
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("C:/Users/zouan/Downloads/truman.jpg");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELS_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->m_speed = 2.5f;
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("C:/Users/zouan/Downloads/intospiderverse.jpg");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    m_state.player->m_jumping_power = 5.0f;

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("C:/Users/zouan/Downloads/poor_things.jpg");

    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].m_speed = 1.0f;
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("C:/Users/zouan/Past Sadness.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(0.0f);

    m_state.jump_sfx = Mix_LoadWAV("C:/Users/zouan/CS3113/mixkit-casino-bling-achievement-2067.wav");
}

void LevelS::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
}

void LevelS::render(ShaderProgram* program)
{
    m_state.map->render(program);
    //m_state.player->render(program);

    GLuint font_texture = Utility::load_texture("C:/Users/zouan/Downloads/font1.png");
    Utility::draw_text(program, font_texture, "WELCOME TO:", 1.0f, -0.45f, glm::vec3(2.0f, -2.0f, 0.0f));
    Utility::draw_text(program, font_texture, "DOG DAYS", 1.0f, -0.45f, glm::vec3(3.0f, -3.0f, 0.0f));
    Utility::draw_text(program, font_texture, "PRESS ENTER TO PLAY", 1.0f, -0.45f, glm::vec3(0.0f, -4.0f, 0.0f));
}