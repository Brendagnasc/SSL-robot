#include "intercept_ball.h"
#include <cmath>
#include <algorithm>

InterceptBall::InterceptBall(float defense_x) : defense_x_(defense_x) {}

RobotCmd InterceptBall::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id   = robot_id;
    cmd.vx = cmd.vy = cmd.vw = 0;
    cmd.kick = false;
    cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    auto ball  = world.get_ball();
    if (!robot || !ball) return cmd;

    float target_x, target_y;
    float base_y = (robot_id == 1) ? -800.0f : 800.0f;

    if (ball->x < DANGER_LINE) {
        // Bola no nosso campo — intercepta na linha de defesa
        target_x = defense_x_;
        target_y = std::clamp(ball->y, -2500.0f, 2500.0f);
    } else {
        // Bola longe — mantém posição base
        target_x = defense_x_;
        target_y = base_y;
    }

    // Garante que o robô não sai do campo
    target_x = std::clamp(target_x, -4400.0f, 4400.0f);
    target_y = std::clamp(target_y, -2900.0f, 2900.0f);

    float dx   = target_x - robot->x;
    float dy   = target_y - robot->y;
    float dist = std::sqrt(dx*dx + dy*dy);

    if (dist < ARRIVAL_THRESH) return cmd;

    float vx = KP * dx;
    float vy = KP * dy;
    float speed = std::sqrt(vx*vx + vy*vy);
    if (speed > MAX_SPEED) {
        vx = vx / speed * MAX_SPEED;
        vy = vy / speed * MAX_SPEED;
    }

    cmd.vx = vx;
    cmd.vy = vy;
    return cmd;
}
