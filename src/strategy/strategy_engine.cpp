#include "strategy_engine.h"
#include <cmath>
#include <algorithm>

StrategyEngine::StrategyEngine(WorldModel& world) : world_(world) {
    roles_[0] = { 0, Role::GOALKEEPER };
    roles_[1] = { 1, Role::DEFENDER };
    roles_[2] = { 2, Role::DEFENDER };
    roles_[3] = { 3, Role::MIDFIELDER };
    roles_[4] = { 4, Role::MIDFIELDER };
    roles_[5] = { 5, Role::ATTACKER };
}

float StrategyEngine::distance(float x1, float y1, float x2, float y2) {
    return std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

int StrategyEngine::closest_robot_to_ball() {
    auto ball = world_.get_ball();
    if (!ball) return 3;
    int   closest_id   = 3;
    float closest_dist = 1e9f;
    for (int i = 1; i < 6; i++) {
        auto robot = world_.get_robot_blue(i);
        if (!robot) continue;
        float d = distance(robot->x, robot->y, ball->x, ball->y);
        if (d < closest_dist) { closest_dist = d; closest_id = i; }
    }
    return closest_id;
}

void StrategyEngine::assign_roles() {
    auto ball = world_.get_ball();
    if (!ball) return;

    // Goleiro sempre fixo
    roles_[0] = { 0, Role::GOALKEEPER };

    // Robô mais próximo da bola vai buscar ela (exceto goleiro)
    int chaser = closest_robot_to_ball();

    for (int i = 1; i <= 5; i++) {
        if (i == chaser) {
            roles_[i] = { i, Role::ATTACKER };
        } else if (i <= 2) {
            roles_[i] = { i, Role::DEFENDER };
        } else {
            roles_[i] = { i, Role::MIDFIELDER };
        }
    }
}

void StrategyEngine::update() { assign_roles(); }

std::array<RobotRole, 6> StrategyEngine::get_roles() const { return roles_; }
