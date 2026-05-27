#include "goalkeeper.h"
#include <cmath>
#include <algorithm>

Goalkeeper::Goalkeeper(float goal_x) : goal_x_(goal_x) {}

RobotCmd Goalkeeper::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id   = robot_id;
    cmd.vx = cmd.vy = cmd.vw = 0;
    cmd.kick = false;
    cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    auto ball  = world.get_ball();
    if (!robot || !ball) return cmd;

    // Goleiro se move lateralmente (Y) acompanhando a bola
    // Limitado à largura do gol
    float target_y = std::clamp(ball->y, -MAX_Y, MAX_Y);
    float target_x = goal_x_;

    float dx   = target_x - robot->x;
    float dy   = target_y - robot->y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < ARRIVAL_THRESH) return cmd;

    float vx = KP * dx;
    float vy = KP * dy;
    float speed = std::sqrt(vx * vx + vy * vy);
    if (speed > MAX_SPEED) {
        vx = vx / speed * MAX_SPEED;
        vy = vy / speed * MAX_SPEED;
    }

    cmd.vx = vx;
    cmd.vy = vy;
    return cmd;
}
