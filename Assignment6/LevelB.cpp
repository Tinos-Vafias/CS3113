#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 25
#define LEVEL_HEIGHT 17

unsigned int LEVELB_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 10, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 4, 8, 8, 8, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 7, 13, 13, 13, 13, 13, 13, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 8, 8, 8, 8, 8, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

LevelB::~LevelB()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise(int lives)
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("C:/Users/zouan/CS3113/Assignment3/mini-golf-tilemap.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 7, 2);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(5.0f, -12.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->m_speed = 2.0f;
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("C:/Users/zouan/CS3113/Assignment3/golfspritesheet.png");

    // Walking
    m_state.player->m_walking[m_state.player->UP] = new int[4]{ 0, 4, 8,  12 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4]{ 12, 8, 4, 0 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4]{ 1, 5, 9, 13 };
    m_state.player->m_walking[m_state.player->LEFT] = new int[4]{ 13, 9, 5,  1 };
    m_state.player->m_walking[m_state.player->UPRIGHT] = new int[4]{ 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWNLEFT] = new int[4]{ 14, 10, 6, 2 };
    m_state.player->m_walking[m_state.player->UPLEFT] = new int[4]{ 3, 7, 11,  15 };
    m_state.player->m_walking[m_state.player->DOWNRIGHT] = new int[4]{ 15, 11, 7, 3 };


    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->UP];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;

    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    m_state.player->m_jumping_power = 2.0f;

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("C:/Users/zouan/CS3113/Assignment3/rubber.png");
    GLuint enemy_texture_id2 = Utility::load_texture("C:/Users/zouan/CS3113/Assignment3/flag.png");


    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(WALKING_Y);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(8.0f, -10.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    m_state.enemies[0].m_speed = 1.0f;
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].set_ai_state(WALKING_Y);
    m_state.enemies[1].m_texture_id = enemy_texture_id;
    m_state.enemies[1].set_position(glm::vec3(9.0f, -10.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].m_speed = 2.0f;
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_state.enemies[2].set_entity_type(ENEMY);
    m_state.enemies[2].set_ai_type(GUARD);
    m_state.enemies[2].set_ai_state(WALKING_Y);
    m_state.enemies[2].m_texture_id = enemy_texture_id;
    m_state.enemies[2].set_position(glm::vec3(10.0f, -10.0f, 0.0f));
    m_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_state.enemies[2].m_speed = 2.0f;
    m_state.enemies[2].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_state.enemies[3].set_entity_type(ENEMY);
    m_state.enemies[3].set_ai_type(GUARD);
    m_state.enemies[3].set_ai_state(WALKING_Y);
    m_state.enemies[3].m_texture_id = enemy_texture_id;
    m_state.enemies[3].set_position(glm::vec3(11.0f, -10.0f, 0.0f));
    m_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_state.enemies[3].m_speed = 2.0f;
    m_state.enemies[3].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_state.enemies[4].set_entity_type(ENEMY);
    m_state.enemies[4].set_ai_type(FLAG);
    m_state.enemies[4].set_ai_state(IDLE);
    m_state.enemies[4].m_texture_id = enemy_texture_id2;
    m_state.enemies[4].set_position(glm::vec3(21.0f, -4.0f, 0.0f));
    m_state.enemies[4].set_movement(glm::vec3(0.0f));
    m_state.enemies[4].m_speed = 2.0f;
    m_state.enemies[4].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("C:/Users/zouan/Past Sadness.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(50.0f);

    m_state.jump_sfx = Mix_LoadWAV("C:/Users/zouan/Downloads/mixkit-golf-ball-swing-2117.wav");
}

void LevelB::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, NULL, 0, m_state.map);
    }

}

void LevelB::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].render(program);
    }

    GLuint font_texture = Utility::load_texture("C:/Users/zouan/Downloads/font1.png");
    if (m_state.player->can_swing) {
        std::string swings_str = std::to_string(stageb_swings);

        // SHOW THESE MESSAGES WHEN THE PLAYER IS ABLE TO SWING
        Utility::draw_text(program, font_texture, "PAR: 3", 0.5f, -0.25f, glm::vec3(m_state.player->get_position().x - 4.8f, m_state.player->get_position().y + 3.53f, 0.0f));
        Utility::draw_text(program, font_texture, "SWING: ", 0.5f, -.25f, glm::vec3(m_state.player->get_position().x - 4.8f, m_state.player->get_position().y + 3.22f, 0.0f));
        Utility::draw_text(program, font_texture, swings_str, 0.5f, -.25f, glm::vec3(m_state.player->get_position().x - 3.0f, m_state.player->get_position().y + 3.22f, 0.0f));

        Utility::draw_text(program, font_texture, "1: L 2: M 3: S", 0.5f, -.25f, glm::vec3(m_state.player->get_position().x - 4.8f, m_state.player->get_position().y + 2.62f, 0.0f));
        Utility::draw_text(program, font_texture, "NW:Q N:W NE:R", 0.5f, -.25f, glm::vec3(m_state.player->get_position().x - 4.8f, m_state.player->get_position().y + 2.31f, 0.0f));
        Utility::draw_text(program, font_texture, "W:A E:D", 0.5f, -.25f, glm::vec3(m_state.player->get_position().x - 4.8f, m_state.player->get_position().y + 2.0f, 0.0f));
        Utility::draw_text(program, font_texture, "SW:Z S:X SE:C", 0.5f, -.25f, glm::vec3(m_state.player->get_position().x - 4.8f, m_state.player->get_position().y + 1.71f, 0.0f));
    }
}