#pragma once
#include "skill.h"
#include <chrono>

class KickToGoal : public Skill {
public:
    KickToGoal(float goal_x = 4500.0f, float goal_y = 0.0f);
    RobotCmd execute(int robot_id, WorldModel& world) override;

private:
    float goal_x_;
    float goal_y_;
    bool  kicked_;
    std::chrono::steady_clock::time_point last_kick_;

    static constexpr float KICK_DISTANCE  = 200.0f;
    static constexpr float APPROACH_SPEED = 1.2f;
    static constexpr float KICK_POWER     = 6.0f;
    static constexpr float KP             = 0.003f;
    static constexpr int   COOLDOWN_MS    = 500;
};
