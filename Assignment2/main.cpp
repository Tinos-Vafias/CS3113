/**
* Author: [Constantinos Vafias]
* Assignment: Pong Clone
* Date due: 2024-03-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'


#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

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
const float DEGREES_PER_SECOND = 90.0f;

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;   // this value MUST be zero

//const char PLAYER_SPRITE_FILEPATH[] = "C:/Users/zouan/source/repos/SDLSimple/Color_circle.png";
const char PLAYER_SPRITE_FILEPATH[] = "C:/Users/zouan/CS3113/Assignment2/beach_ball.png";
const char PLAYER_SPRITE_FILEPATH2[] = "C:/Users/zouan/CS3113/Assignment2/sandcastle.png";
const char PLAYER_SPRITE_FILEPATH3[] = "C:/Users/zouan/CS3113/Assignment2/game_over_p1.png";
const char PLAYER_SPRITE_FILEPATH4[] = "C:/Users/zouan/CS3113/Assignment2/game_over_p2.png";

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_is_growing = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_model_matrix, g_projection_matrix, g_trans_matrix, g_second_model_matrix, g_ball_model_matrix,
            g_game_over_model_matrix, g_ball2_model_matrix, g_ball3_model_matrix;

GLuint g_player_texture_id;
GLuint g_player_texture_id2;
GLuint g_player_texture_id3;
GLuint g_player_texture_id4;


// For delta time
float g_previous_ticks = 0.0f;

// left paddle position and movement
glm::vec3 g_l_paddle_position = glm::vec3(0.0f, 0.0f, 0.0f);     
glm::vec3 g_l_paddle_movement = glm::vec3(-3.5f, 0.0f, 0.0f);     
                                                    
// right paddle position and movement
glm::vec3 g_r_paddle_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_r_paddle_movement = glm::vec3(3.5f, 0.0f, 0.0f);

// ball position and movement
glm::vec3 g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball3_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball3_movement = glm::vec3(0.0f, 0.0f, 0.0f);


float g_player_speed = 5.0f;  // move 1 unit per second   
float g_ball_speed = 1.0f;

// scaling vectors
glm::vec3 g_paddle_scale    = glm::vec3(0.5f, 1.5f, 0.0f);
glm::vec3 g_ball_scale      = glm::vec3(0.5f, 0.5f, 0.0f);
glm::vec3 g_game_over_scale = glm::vec3(7.0f, 7.0f, 0.0f);

// boolean for two_player mode
bool two_player = true;
bool up_down = true;
// boolean for game_over
bool game_over = false;
bool left = false;
bool winner_chosen = false;
// bool for amount of balls (default set to 1)
int ball_one = true;
int ball_two = false;
int ball_three = false;

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Project 2: Pong Clone",
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

    // Load our player image
    g_player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);    // beach_ball
    g_player_texture_id2 = load_texture(PLAYER_SPRITE_FILEPATH2);  // castle
    g_player_texture_id3 = load_texture(PLAYER_SPRITE_FILEPATH3);  // left game over
    g_player_texture_id4 = load_texture(PLAYER_SPRITE_FILEPATH4);  // right game over

    // setting initial position and size of the left paddle 
    g_model_matrix = glm::mat4(1.0f);
    g_model_matrix = glm::translate(g_model_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
    g_l_paddle_position += g_l_paddle_movement;

    // setting initial position and size of the right paddle
    g_second_model_matrix = glm::mat4(1.0f);
    g_second_model_matrix = glm::translate(g_second_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    g_r_paddle_position += g_r_paddle_movement;
    g_second_model_matrix = glm::scale(g_second_model_matrix, glm::vec3(0.5f, 6.25f, 0.0f)); //doesn't work?



    // setting starting position and movement direction of the ball
    g_ball_model_matrix = glm::mat4(1.0f);
    g_ball_movement.x = 1.0f;
    g_ball_movement.y = 1.0f;

    g_ball2_model_matrix = glm::mat4(1.0f);
    g_ball2_movement.x = -1.0f;
    g_ball2_movement.y = -1.0f;

    g_ball3_model_matrix = glm::mat4(1.0f);
    g_ball3_movement.x = -1.0f;
    g_ball3_movement.y = 1.0f;

    // setting game_over images
    g_game_over_model_matrix = glm::mat4(1.0f);

    g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);


    glUseProgram(g_shader_program.get_program_id());

    glClearColor(.76, .698, .52,  BG_OPACITY);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // In case there is no input 
    g_l_paddle_movement = glm::vec3(0.0f);
    g_r_paddle_movement = glm::vec3(0.0f);

    // code for "T" keystroke
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game                                                      
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;


        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_t:
                two_player = !two_player;
                break;

            case SDLK_1:
                ball_one = true;
                ball_two = false;
                ball_three = false;
                break;
            case SDLK_2:
                ball_one = false;
                ball_two = true;
                ball_three = false;
                break;
            case SDLK_3:
                ball_one = false;
                ball_two = false;
                ball_three = true;
                break;
            case SDLK_p:
                ball_one = false;
                ball_two = true;
                ball_three = false;
                break;
            }
        }
    }
    // following code for keyholds regarding moving the paddles up and down 

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_W])
    {
        g_l_paddle_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        g_l_paddle_movement.y = -1.0f;
    }

    if (key_state[SDL_SCANCODE_UP] && two_player)
    {
        g_r_paddle_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_DOWN] && two_player)
    {
        g_r_paddle_movement.y = -1.0f;
    }

    // Normalization                                                               
    if (glm::length(g_l_paddle_movement) > 1.0f)
    {
        g_l_paddle_movement = glm::normalize(g_l_paddle_movement);
    }
    if (glm::length(g_r_paddle_movement) > 1.0f)
    {
        g_r_paddle_movement = glm::normalize(g_r_paddle_movement);
    }
}

void update()
{
    // Code to set up delta time
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    // code setting up all existing model matrices
    g_model_matrix = glm::mat4(1.0f);
    g_model_matrix = glm::translate(g_model_matrix, g_l_paddle_position);
    g_model_matrix = glm::scale(g_model_matrix, g_paddle_scale);


    g_second_model_matrix = glm::mat4(1.0f);
    g_second_model_matrix = glm::translate(g_second_model_matrix, g_r_paddle_position);
    g_second_model_matrix = glm::scale(g_second_model_matrix, g_paddle_scale);

    g_ball_model_matrix = glm::mat4(1.0f);
    g_ball_model_matrix = glm::translate(g_ball_model_matrix, g_ball_position);
    g_ball_model_matrix = glm::scale(g_ball_model_matrix, g_ball_scale);


    g_game_over_model_matrix = glm::mat4(1.0f);
    g_game_over_model_matrix = glm::scale(g_game_over_model_matrix, g_game_over_scale);


    // Code regarding paddle movement 
    if (game_over == false) {
        // left paddle movement
        if (g_l_paddle_position.y >= 3.2f) {
            g_l_paddle_position = glm::vec3(g_l_paddle_position.x, 3.199999f, g_l_paddle_position.z);
        }
        else if (g_l_paddle_position.y <= -3.2f) {
            g_l_paddle_position = glm::vec3(g_l_paddle_position.x, -3.199999f, g_l_paddle_position.z);
        }
        else {
            // Add direction * units per second * elapsed time
            g_l_paddle_position += g_l_paddle_movement * g_player_speed * delta_time;
        }

        // right paddle movement
        if (g_r_paddle_position.y >= 3.2f) {
            g_r_paddle_position = glm::vec3(g_r_paddle_position.x, 3.199999f, g_r_paddle_position.z);
            if (!two_player) {
                up_down = false;
            }
        }
        else if (g_r_paddle_position.y <= -3.2f) {
            g_r_paddle_position = glm::vec3(g_r_paddle_position.x, -3.199999f, g_r_paddle_position.z);
            if (!two_player) {
                up_down = true;
            }
        }
        else {
            // Add direction * units per second * elapsed time
            g_r_paddle_position += g_r_paddle_movement * g_player_speed * delta_time;
            if (!two_player) {
                if (up_down) {
                    g_r_paddle_movement.y = 1;
                    g_r_paddle_position += g_r_paddle_movement * g_player_speed * delta_time;
                }
                else {
                    g_r_paddle_movement.y = -1;
                    g_r_paddle_position += g_r_paddle_movement * g_player_speed * delta_time;
                }
            }
        }
    }

    // code regarding ball movement
    // ball movement 
    g_ball_position += g_ball_movement * g_ball_speed * delta_time;
    // change y-axis movement when collding with top or bottom of screen
    if (g_ball_position.y >= 3.5f) {
        g_ball_movement.y = -1;
        g_ball_position += g_ball_movement * g_ball_speed * delta_time;
    }
    else if (g_ball_position.y <= -3.5f) {
        g_ball_movement.y = 1;
        g_ball_position += g_ball_movement * g_ball_speed * delta_time;
    }

    // collision logic between ball and left paddle. 
    float x_distance_left = fabs(g_l_paddle_position.x - g_ball_position.x) - ((1.0f + 1.0f) / 2.0f);
    float y_distance_left = fabs(g_l_paddle_position.y - g_ball_position.y) - ((1.0f + 1.0f) / 2.0f);

    float x_distance_right = fabs(g_r_paddle_position.x - g_ball_position.x) - ((1.0f + 1.0f) / 2.0f);
    float y_distance_right = fabs(g_r_paddle_position.y - g_ball_position.y) - ((1.0f + 1.0f) / 2.0f);

    if ((x_distance_left < 0.0f && y_distance_left < 0.0f) || (x_distance_right < 0.0f && y_distance_right < 0.0f)) {
        g_ball_movement.x = g_ball_movement.x * -1;
    }

    // when the ball passes the right of the screen
    if (g_ball_position.x >= 4.8f) {
        game_over = true;
    } // when the ball passes the left of the screen
    else if (g_ball_position.x <= -4.8f) {
        game_over = true;
        left = true;
    }

    // code for second ball 
    if (ball_two == true || ball_three == true) {
        g_ball2_model_matrix = glm::mat4(1.0f);
        g_ball2_model_matrix = glm::translate(g_ball2_model_matrix, g_ball2_position);
        g_ball2_model_matrix = glm::scale(g_ball2_model_matrix, g_ball_scale);

        g_ball2_position += g_ball2_movement * g_ball_speed * delta_time;

        // change y-axis movement when collding with top or bottom of screen
        if (g_ball2_position.y >= 3.5f) {
            g_ball2_movement.y = -1;
            g_ball2_position += g_ball2_movement * g_ball_speed * delta_time;
        }
        else if (g_ball2_position.y <= -3.5f) {
            g_ball2_movement.y = 1;
            g_ball2_position += g_ball2_movement * g_ball_speed * delta_time;
        }

        // collision logic between ball and left paddle. 
        float x_distance2_left = fabs(g_l_paddle_position.x - g_ball2_position.x) - ((1.0f + 1.0f) / 2.0f);
        float y_distance2_left = fabs(g_l_paddle_position.y - g_ball2_position.y) - ((1.0f + 1.0f) / 2.0f);

        float x_distance2_right = fabs(g_r_paddle_position.x - g_ball2_position.x) - ((1.0f + 1.0f) / 2.0f);
        float y_distance2_right = fabs(g_r_paddle_position.y - g_ball2_position.y) - ((1.0f + 1.0f) / 2.0f);

        if ((x_distance2_left < 0.0f && y_distance2_left < 0.0f) || (x_distance2_right < 0.0f && y_distance2_right < 0.0f)) {
            g_ball2_movement.x = g_ball2_movement.x * -1;
        }

        // when the ball passes the right of the screen
        if (g_ball2_position.x >= 4.8f) {
            game_over = true;
        } // when the ball passes the left of the screen
        else if (g_ball2_position.x <= -4.8f) {
            game_over = true;
            left = true;
        }
    } // code for ball 3
    if (ball_three == true) {
        g_ball3_model_matrix = glm::mat4(1.0f);
        g_ball3_model_matrix = glm::translate(g_ball3_model_matrix, g_ball3_position);
        g_ball3_model_matrix = glm::scale(g_ball3_model_matrix, g_ball_scale);

        g_ball3_position += g_ball3_movement * g_ball_speed * delta_time;

        // change y-axis movement when collding with top or bottom of screen
        if (g_ball3_position.y >= 3.5f) {
            g_ball3_movement.y = -1;
            g_ball3_position += g_ball3_movement * g_ball_speed * delta_time;
        }
        else if (g_ball3_position.y <= -3.5f) {
            g_ball3_movement.y = 1;
            g_ball3_position += g_ball3_movement * g_ball_speed * delta_time;
        }

        // collision logic between ball and left paddle. 
        float x_distance3_left = fabs(g_l_paddle_position.x - g_ball3_position.x) - ((1.0f + 1.0f) / 2.0f);
        float y_distance3_left = fabs(g_l_paddle_position.y - g_ball3_position.y) - ((1.0f + 1.0f) / 2.0f);

        float x_distance3_right = fabs(g_r_paddle_position.x - g_ball3_position.x) - ((1.0f + 1.0f) / 2.0f);
        float y_distance3_right = fabs(g_r_paddle_position.y - g_ball3_position.y) - ((1.0f + 1.0f) / 2.0f);

        if ((x_distance3_left < 0.0f && y_distance3_left < 0.0f) || (x_distance3_right < 0.0f && y_distance3_right < 0.0f)) {
            g_ball3_movement.x = g_ball3_movement.x * -1;
        }

        // when the ball passes the right of the screen
        if (g_ball3_position.x >= 4.8f) {
            game_over = true;
        } // when the ball passes the left of the screen
        else if (g_ball3_position.x <= -4.8f) {
            game_over = true;
            left = true;
        }
    }
    
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    // -----------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f  // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    // -----------------------------------------------------------------------

    //calling draw_object for every object we have
    draw_object(g_model_matrix, g_player_texture_id2);
    draw_object(g_second_model_matrix, g_player_texture_id2);

    // drawing the game over screen when either left (P1) or right (P2) wins
    if (game_over == true) {
        if (left == true) {
            // put in left texture
            draw_object(g_game_over_model_matrix, g_player_texture_id3);
        }
        else {
            // put in right texture
            draw_object(g_game_over_model_matrix, g_player_texture_id4);
        }
    }

    if (ball_one == true) {
        draw_object(g_ball_model_matrix, g_player_texture_id);
    }
    else if (ball_two == true) {
        draw_object(g_ball_model_matrix, g_player_texture_id);
        draw_object(g_ball2_model_matrix, g_player_texture_id); 

    }
    else if (ball_three == true) {
        draw_object(g_ball_model_matrix, g_player_texture_id);
        draw_object(g_ball2_model_matrix, g_player_texture_id);
        draw_object(g_ball3_model_matrix, g_player_texture_id);
    }



    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

/**
 Start herewe can see the general structure of a game loop without worrying too much about the details yet.
 */
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
