#include "Scene.h"

class LevelB : public Scene {
public:
    int ENEMY_COUNT = 5;

    ~LevelB();

    void initialise(int lives) override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};