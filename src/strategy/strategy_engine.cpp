#include "strategy_engine.h"
#include <cmath>
#include <iostream>

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
    if (!ball) return -1;
    int   closest_id   = -1;
    float closest_dist = 1e9f;
    for (int i = 0; i < 6; i++) {
        auto robot = world_.get_robot_blue(i);
        if (!robot) continue;
        float dist = distance(robot->x, robot->y, ball->x, ball->y);
        if (dist < closest_dist) { closest_dist = dist; closest_id = i; }
    }
    return closest_id;
}

void StrategyEngine::assign_roles() {
    auto ball = world_.get_ball();
    if (!ball) return;
    roles_[0] = { 0, Role::GOALKEEPER };
    roles_[1] = { 1, Role::DEFENDER };
    roles_[2] = { 2, Role::DEFENDER };
    int closest = closest_robot_to_ball();
    for (int i = 3; i <= 5; i++)
        roles_[i] = { i, (i == closest) ? Role::ATTACKER : Role::MIDFIELDER };
}

void StrategyEngine::update() { assign_roles(); }

std::array<RobotRole, 6> StrategyEngine::get_roles() const { return roles_; }
