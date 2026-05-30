#include "intercept_ball.h"
#include <cmath>
#include <algorithm>

InterceptBall::InterceptBall(float defense_x) : defense_x_(defense_x) {}

static std::pair<float,float> repulsion_intercept(float rx, float ry, WorldModel& world, int self_id) {
    float fx = 0, fy = 0;
    for (int i = 0; i < 6; i++) {
        if (i == self_id) continue;
        auto other = world.get_robot_blue(i);
        if (!other) continue;
        float dx = rx - other->x;
        float dy = ry - other->y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 250.0f && dist > 1.0f) {
            float force = (250.0f - dist) / 250.0f * 0.6f;
            fx += (dx / dist) * force;
            fy += (dy / dist) * force;
        }
    }
    return {fx, fy};
}

RobotCmd InterceptBall::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id   = robot_id;
    cmd.vx = cmd.vy = cmd.vw = 0;
    cmd.kick = false;
    cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    auto ball  = world.get_ball();
    if (!robot || !ball) return cmd;

    float base_y = (robot_id == 1) ? -800.0f : 800.0f;
    float target_x, target_y;

    if (ball->x < DANGER_LINE) {
        target_x = defense_x_;
        target_y = std::clamp(ball->y, -2500.0f, 2500.0f);
    } else {
        target_x = defense_x_;
        target_y = base_y;
    }

    target_x = std::clamp(target_x, -4400.0f, 4400.0f);
    target_y = std::clamp(target_y, -2900.0f, 2900.0f);

    float dx   = target_x - robot->x;
    float dy   = target_y - robot->y;
    float dist = std::sqrt(dx*dx + dy*dy);

    if (dist < ARRIVAL_THRESH) return cmd;

    float vx = KP * dx;
    float vy = KP * dy;

    auto [rx, ry] = repulsion_intercept(robot->x, robot->y, world, robot_id);
    vx += rx;
    vy += ry;

    float speed = std::sqrt(vx*vx + vy*vy);
    if (speed > MAX_SPEED) {
        vx = vx / speed * MAX_SPEED;
        vy = vy / speed * MAX_SPEED;
    }

    cmd.vx = vx;
    cmd.vy = vy;
    return cmd;
}
