#pragma once
#include "skill.h"
#include <chrono>

class PassSkill : public Skill {
public:
    PassSkill(float goal_x = 4500.0f);
    RobotCmd execute(int robot_id, WorldModel& world) override;

private:
    float goal_x_;
    bool  acted_;
    std::chrono::steady_clock::time_point last_act_;

    // Encontra melhor companheiro para passar
    int best_teammate(int self_id, WorldModel& world);

    // Ângulo para o gol
    float angle_to_goal(float rx, float ry);

    // Tem visão limpa do gol?
    bool has_shot(float rx, float ry);

    static constexpr float KICK_DIST     = 200.0f;
    static constexpr float SHOOT_DIST    = 2500.0f; // chuta se < 2500mm do gol
    static constexpr float PASS_POWER    = 2.5f;
    static constexpr float KICK_POWER    = 3.0f;
    static constexpr float APPROACH_SPD  = 1.3f;
    static constexpr int   COOLDOWN_MS   = 600;
};
