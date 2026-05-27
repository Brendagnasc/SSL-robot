#pragma once
#include "skill.h"

class Goalkeeper : public Skill {
public:
    Goalkeeper(float goal_x = -3800.0f);
    RobotCmd execute(int robot_id, WorldModel& world) override;

private:
    float goal_x_;
    static constexpr float MAX_SPEED      = 1.5f;
    static constexpr float KP             = 0.003f;
    static constexpr float MAX_Y          = 900.0f;  // limite lateral do gol
    static constexpr float ARRIVAL_THRESH = 30.0f;
};
