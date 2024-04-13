#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 24
#define LEVEL_HEIGHT 8

unsigned int LEVELC_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 2,
    2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2
};

LevelC::~LevelC()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelC::initialise(int lives)
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("C:/Users/zouan/CS3113/Assignment3/grass.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 4, 1);

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
    m_state.player->m_texture_id = Utility::load_texture("C:/Users/zouan/CS3113/Assignment3/soko.png");

    // Walking
    //m_state.player->m_walking[m_state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    //m_state.player->m_walking[m_state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    //m_state.player->m_walking[m_state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    //m_state.player->m_walking[m_state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    //m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    //m_state.player->m_animation_frames = 4;
    //m_state.player->m_animation_index = 0;
    //m_state.player->m_animation_time = 0.0f;
    //m_state.player->m_animation_cols = 4;
    //m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    m_state.player->m_jumping_power = 5.0f;

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("C:/Users/zouan/CS3113/Assignment3/fire.png");

    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(23.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].m_speed = 3.0f;
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("C:/Users/zouan/Past Sadness.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(50.0f);

    m_state.jump_sfx = Mix_LoadWAV("C:/Users/zouan/CS3113/mixkit-casino-bling-achievement-2067.wav");
}

void LevelC::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);


    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map);
    }

    if ((m_state.player->get_position().y < -10.0f && m_state.player->get_position().x < 20.0f) || m_state.player->num_lives == 0) {
        m_state.next_scene_id = 3;
        number_of_lives--;
    }

    if (m_state.player->death_flag == true) {
        m_state.next_scene_id = 3;
    }
}

void LevelC::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);

    GLuint font_texture = Utility::load_texture("C:/Users/zouan/Downloads/font1.png");

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].render(program);
    }

    if (m_state.player->get_position().y < -10.0f && m_state.player->get_position().x > 20.0f) {
        Utility::draw_text(program, font_texture, "YOU WIN!!!", 1.0f, -0.45f, glm::vec3(m_state.player->get_position().x - 2, -2.0f, 0.0f));
    }
    if (number_of_lives <= 0) {
        Utility::draw_text(program, font_texture, "YOU LOSE!!", 1.0f, -0.45f, glm::vec3(m_state.player->get_position().x - 2, -2.0f, 0.0f));
    }
}