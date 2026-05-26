#include "move_to_position.h"
#include <cmath>
#include <algorithm>
#include <iostream>

MoveToPosition::MoveToPosition(float target_x, float target_y, float target_angle)
    : target_x_(target_x), target_y_(target_y), target_angle_(target_angle), done_(false)
{}

void MoveToPosition::set_target(float x, float y, float angle) {
    target_x_     = x;
    target_y_     = y;
    target_angle_ = angle;
    done_         = false;
}

bool MoveToPosition::is_done() const {
    return done_;
}

RobotCmd MoveToPosition::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id   = robot_id;
    cmd.vx         = 0;
    cmd.vy         = 0;
    cmd.vw         = 0;
    cmd.kick       = false;
    cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    if (!robot) return cmd;

    float dx   = target_x_ - robot->x;
    float dy   = target_y_ - robot->y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < ARRIVAL_THRESHOLD) {
        done_ = true;
        std::cout << "[Skill] Robô " << robot_id << " chegou ao destino\n";
        return cmd;
    }

    // Controle proporcional simples
    float vx = KP * dx;
    float vy = KP * dy;

    // Limita velocidade máxima
    float speed = std::sqrt(vx * vx + vy * vy);
    if (speed > MAX_SPEED) {
        vx = vx / speed * MAX_SPEED;
        vy = vy / speed * MAX_SPEED;
    }

    // Erro angular
    float angle_err = target_angle_ - robot->angle;
    while (angle_err >  M_PI) angle_err -= 2 * M_PI;
    while (angle_err < -M_PI) angle_err += 2 * M_PI;
    float vw = 2.0f * angle_err;

    cmd.vx = vx;
    cmd.vy = vy;
    cmd.vw = vw;

    std::cout << "[Skill] Robô " << robot_id
              << " → (" << target_x_ << ", " << target_y_ << ")"
              << " dist=" << dist << " vx=" << vx << " vy=" << vy << "\n";

    return cmd;
}
