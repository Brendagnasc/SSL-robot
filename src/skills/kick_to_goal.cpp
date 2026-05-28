#include "kick_to_goal.h"
#include <cmath>
#include <iostream>

KickToGoal::KickToGoal(float goal_x, float goal_y)
    : goal_x_(goal_x), goal_y_(goal_y) {}

RobotCmd KickToGoal::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id   = robot_id;
    cmd.vx = cmd.vy = cmd.vw = 0;
    cmd.kick = false;
    cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    auto ball  = world.get_ball();
    if (!robot || !ball) return cmd;

    float dx_ball = ball->x - robot->x;
    float dy_ball = ball->y - robot->y;
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
        std::cout << "[KickToGoal] Robô " << robot_id << " CHUTANDO!\n";
    } else {
        // Calcula ponto de abordagem: atrás da bola em relação ao gol
        float dx_goal = goal_x_ - ball->x;
        float dy_goal = goal_y_ - ball->y;
        float d = std::sqrt(dx_goal*dx_goal + dy_goal*dy_goal);

        // Ponto 200mm atrás da bola (lado oposto ao gol)
        float approach_x = ball->x - (dx_goal / d) * 200.0f;
        float approach_y = ball->y - (dy_goal / d) * 200.0f;

        float dax = approach_x - robot->x;
        float day = approach_y - robot->y;
        float da  = std::sqrt(dax*dax + day*day);

        float vx = (dax / da) * APPROACH_SPEED;
        float vy = (day / da) * APPROACH_SPEED;
        cmd.vx = vx;
        cmd.vy = vy;
    }

    return cmd;
}
