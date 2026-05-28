#pragma once
#include "skill.h"

class KickToGoal : public Skill {
public:
    KickToGoal(float goal_x = 4500.0f, float goal_y = 0.0f);
    RobotCmd execute(int robot_id, WorldModel& world) override;

private:
    float goal_x_;
    float goal_y_;

    static constexpr float KICK_DISTANCE  = 150.0f;  // mm
    static constexpr float APPROACH_SPEED = 1.0f;    // m/s
    static constexpr float KICK_POWER     = 5.0f;
    static constexpr float KP             = 0.003f;
};
