#include "kick_to_goal.h"
#include <cmath>
#include <iostream>

KickToGoal::KickToGoal(float goal_x, float goal_y)
    : goal_x_(goal_x), goal_y_(goal_y), kicked_(false)
{}

RobotCmd KickToGoal::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id   = robot_id;
    cmd.vx = cmd.vy = cmd.vw = 0;
    cmd.kick = false;
    cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    auto ball  = world.get_ball();
    if (!robot || !ball) return cmd;

    auto now = std::chrono::steady_clock::now();
    if (kicked_) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - last_kick_).count();
        if (ms > COOLDOWN_MS) {
            kicked_ = false;
        } else {
            // Recua para posição de apoio
            float support_x = ball->x - 800.0f;
            float support_y = ball->y * 0.5f;
            support_x = std::max(support_x, -3500.0f);
            float dx = support_x - robot->x;
            float dy = support_y - robot->y;
            float d  = std::sqrt(dx*dx + dy*dy);
            if (d > 80.0f) {
                cmd.vx = (dx / d) * 1.2f;
                cmd.vy = (dy / d) * 1.2f;
            }
            return cmd;
        }
    }

    float dx_ball   = ball->x - robot->x;
    float dy_ball   = ball->y - robot->y;
    float dist_ball = std::sqrt(dx_ball*dx_ball + dy_ball*dy_ball);

    if (dist_ball < KICK_DISTANCE) {
        // Chuta em direção ao gol
        float dx_goal = goal_x_ - robot->x;
        float dy_goal = goal_y_ - robot->y;
        float d = std::sqrt(dx_goal*dx_goal + dy_goal*dy_goal);
        cmd.vx = (dx_goal / d) * 1.5f;
        cmd.vy = (dy_goal / d) * 1.5f;
        cmd.kick = true;
        cmd.kick_power = KICK_POWER;
        kicked_ = true;
        last_kick_ = now;
        std::cout << "[KickToGoal] Robo " << robot_id << " CHUTOU!\n";
    } else if (dist_ball < 600.0f) {
        // Perto — vai direto para a bola em alta velocidade
        cmd.vx = (dx_ball / dist_ball) * 1.5f;
        cmd.vy = (dy_ball / dist_ball) * 1.5f;
    } else {
        // Longe — posiciona atrás da bola em relação ao gol
        float dx_goal = goal_x_ - ball->x;
        float dy_goal = goal_y_ - ball->y;
        float d = std::sqrt(dx_goal*dx_goal + dy_goal*dy_goal);
        float approach_x = ball->x - (dx_goal / d) * 300.0f;
        float approach_y = ball->y - (dy_goal / d) * 300.0f;
        float dax = approach_x - robot->x;
        float day = approach_y - robot->y;
        float da  = std::sqrt(dax*dax + day*day);
        if (da > 30.0f) {
            cmd.vx = (dax / da) * APPROACH_SPEED;
            cmd.vy = (day / da) * APPROACH_SPEED;
        }
    }

    return cmd;
}
