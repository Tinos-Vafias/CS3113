#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 12
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelS.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"

// ����� CONSTANTS ����� //
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;


// ����� GLOBAL VARIABLES ����� //
Scene* g_current_scene;
LevelS* g_levelS;
LevelA* g_levelA;
LevelB* g_levelB;
LevelC* g_levelC;

Scene* g_levels[4];

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;

int g_total_lives = 0;
int direction = 0;
float x_vel = 0.0f;
float y_vel = 0.0f;
float sign = 1.0f;

// ����� GENERAL FUNCTIONS ����� //
void switch_to_scene(Scene* scene, int lives)
{
    //change parameters to add num lives that can be passed through
    g_current_scene = scene;
    g_current_scene->initialise(lives); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("ASSIGNMENT 6",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(.200, .255, .67, BG_OPACITY);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_levelS = new LevelS();


    g_levels[0] = g_levelS;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;

    // Start at level S (Start menu)
    switch_to_scene(g_levels[2], g_total_lives);
    //g_total_lives = g_current_scene->number_of_lives;
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));
    float friction_coeff = 0.1f;
    float accel = 1.0f;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                //( for some reason, you mus sleect the strength of the shot first, then sleect dorection for it to work properly.)) 
                // PRESSING 1, 2 OR 3 CAN SET THE STRENGTH OF THE SHOT
                // 1 BEING THE LOWEST, 3 BEING THE HARDEST
            case SDLK_1:
                x_vel = 1.0f; y_vel = 1.0f;
                break;
            case SDLK_2:
                x_vel = 2.5f; y_vel = 2.5f;
                break;
            case SDLK_3:
                x_vel = 5.75f; y_vel = 5.75f;
                break;
                // DIRECTIONS DECIDE WHERE THE BALL WILL BE HIT
                // KEY HAS DIRECT CORRELATION TO CARDINAL DIRECTIONS
                // Q  W  E  ----->  NW  N  NE 
                // A     D  ----->  W       E
                // Z  X  C  ----->  SW  S  SE
                // PRESS DIRECTION BUTTON TO SET WHERE YOU WANT TO HIT
                // THEN PRESS SPACE TO HIT THE BALL
            case SDLK_q: // NW DIRECTION
                g_current_scene->m_state.player->m_movement.y = 1.0f;
                g_current_scene->m_state.player->m_movement.x = -1.0f;
                x_vel *= -sign; y_vel *= sign;
                g_current_scene->ball_direction = 1;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->UPLEFT];
                break;
            case SDLK_w: // N  DIRECTION
                g_current_scene->m_state.player->m_movement.y = 1.0f;
                g_current_scene->m_state.player->m_movement.x = 0.0f;
                x_vel *= 0.0f; y_vel *= sign;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->UP];
                break;
            case SDLK_e: // NE DIRECTION
                g_current_scene->m_state.player->m_movement.y = 1.0f;
                g_current_scene->m_state.player->m_movement.x = 1.0f;
                x_vel *= sign; y_vel *= sign;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->UPRIGHT];
                break;
            case SDLK_d: // E  DIRECTION
                g_current_scene->m_state.player->m_movement.y = 0.0f;
                g_current_scene->m_state.player->m_movement.x = -1.0f;
                x_vel *= sign; y_vel *= 0.0f;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
                break;
            case SDLK_c: // SE DIRECTION
                g_current_scene->m_state.player->m_movement.y = -1.0f;
                g_current_scene->m_state.player->m_movement.x = 1.0f;
                x_vel *= sign; y_vel *= -sign;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->DOWNRIGHT];
                break;
            case SDLK_x: // S  DIRECTION
                g_current_scene->m_state.player->m_movement.y = -1.0f;
                g_current_scene->m_state.player->m_movement.x = 0.0f;
                x_vel *= 0.0f; y_vel *= -sign;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->DOWN];
                break;
            case SDLK_z: // SW DIRECTION
                g_current_scene->m_state.player->m_movement.y = -1.0f;
                g_current_scene->m_state.player->m_movement.x = -1.0f;
                x_vel *= -sign; y_vel *= -sign;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->DOWNLEFT];
                break;
            case SDLK_a: // W  DIRECTION
                g_current_scene->m_state.player->m_movement.y = 0.0f;
                g_current_scene->m_state.player->m_movement.x = -1.0f;
                x_vel *= -sign; y_vel *= 0.0f;
                g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
                break;
            case SDLK_SPACE:
                // HIT THE BALL
                // used jumping power as a signal for the level, which helps add the total swings
                if (g_current_scene->m_state.player->can_swing) {

                    if (g_current_scene->m_state.player->m_jumping_power == 1.0f) {
                        g_current_scene->stagea_swings++;
                    }
                    if (g_current_scene->m_state.player->m_jumping_power == 2.0f) {
                        g_current_scene->stageb_swings++;
                    }
                    if (g_current_scene->m_state.player->m_jumping_power == 3.0f) {
                        g_current_scene->stagec_swings++;
                    }
                    Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
                    g_current_scene->m_state.player->set_velocity(glm::vec3(x_vel, y_vel, 0.0f));
                }
                break;
            case SDLK_RETURN:
                // At start screen
                if (g_current_scene == g_levelS) {
                    switch_to_scene(g_levels[1], g_total_lives);
                }
                break;
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (glm::length(g_current_scene->m_state.player->m_movement) > 1.0f)
    {
        g_current_scene->m_state.player->m_movement = glm::normalize(g_current_scene->m_state.player->m_movement);
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);


        g_is_colliding_bottom = g_current_scene->m_state.player->m_collided_bottom;

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);

    //if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
    //    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
    //}
    //else {
    //    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    //}
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, -g_current_scene->m_state.player->get_position().y, 0));

    if (g_current_scene == g_levelA && g_current_scene->m_state.player->x_vel_flag && g_current_scene->m_state.player->y_vel_flag) switch_to_scene(g_levelB, g_total_lives);
    if (g_current_scene == g_levelB && g_current_scene->m_state.player->x_vel_flag && g_current_scene->m_state.player->y_vel_flag) switch_to_scene(g_levelC, g_total_lives);

    //if (g_current_scene == g_levelB && g_current_scene->m_state.player->get_position().y < -10.0f && g_current_scene->m_state.player->get_position().x > 20.0f) switch_to_scene(g_levelC,g_total_lives);
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_levelS;
}

// ����� DRIVER GAME LOOP ����� //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();

        if (g_current_scene->m_state.next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id], g_total_lives);

        render();
    }

    shutdown();
    return 0;
}