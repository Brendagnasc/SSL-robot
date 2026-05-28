#pragma once
#include "skill.h"

class InterceptBall : public Skill {
public:
    InterceptBall(float defense_x = -2500.0f);
    RobotCmd execute(int robot_id, WorldModel& world) override;

private:
    float defense_x_;

    static constexpr float MAX_SPEED      = 1.5f;
    static constexpr float KP             = 0.003f;
    static constexpr float ARRIVAL_THRESH = 40.0f;
    // Bola considerada "perigosa" quando cruza esta linha
    static constexpr float DANGER_LINE    = -1000.0f;
};
