#include "pass_skill.h"
#include <cmath>
#include <iostream>

PassSkill::PassSkill(float goal_x) : goal_x_(goal_x), acted_(false) {}

bool PassSkill::has_shot(float rx, float ry) {
    float dist = std::sqrt((goal_x_-rx)*(goal_x_-rx) + ry*ry);
    return dist < 3000.0f; // chuta se < 3m do gol
}

int PassSkill::best_teammate(int self_id, WorldModel& world) {
    auto ball = world.get_ball();
    if (!ball) return -1;
    int   best_id    = -1;
    float best_score = -1e9f;
    for (int i = 1; i < 6; i++) {
        if (i == self_id) continue;
        auto r = world.get_robot_blue(i);
        if (!r) continue;
        // Aceita qualquer companheiro que esteja na metade ofensiva
        if (r->x < 0) continue;
        float dist_goal = std::sqrt((goal_x_-r->x)*(goal_x_-r->x) + r->y*r->y);
        float score = -dist_goal;
        if (score > best_score) { best_score = score; best_id = i; }
    }
    return best_id;
}

RobotCmd PassSkill::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id = robot_id;
    cmd.vx = cmd.vy = cmd.vw = 0;
    cmd.kick = false; cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    auto ball  = world.get_ball();
    if (!robot || !ball) return cmd;

    auto now = std::chrono::steady_clock::now();
    if (acted_) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - last_act_).count();
        if (ms > COOLDOWN_MS) acted_ = false;
        else {
            float dx = (ball->x - 500.0f) - robot->x;
            float dy = ball->y*0.3f - robot->y;
            float d  = std::sqrt(dx*dx + dy*dy);
            if (d > 80.0f) { cmd.vx=(dx/d)*1.0f; cmd.vy=(dy/d)*1.0f; }
            return cmd;
        }
    }

    float dx_b = ball->x - robot->x;
    float dy_b = ball->y - robot->y;
    float db   = std::sqrt(dx_b*dx_b + dy_b*dy_b);

    if (db < KICK_DIST) {
        if (has_shot(robot->x, robot->y)) {
            // Chuta ao gol — aponta para o centro
            float dx_g = goal_x_ - robot->x;
            float dy_g = 0 - robot->y * 0.2f;
            float d    = std::sqrt(dx_g*dx_g + dy_g*dy_g);
            cmd.vx = (dx_g/d)*1.5f; cmd.vy = (dy_g/d)*1.5f;
            cmd.kick = true; cmd.kick_power = KICK_POWER;
            acted_ = true; last_act_ = now;
            std::cout << "[CHUTE] Robo " << robot_id << " ao gol!\n";
        } else {
            // Passa para melhor companheiro
            int target = best_teammate(robot_id, world);
            if (target >= 0) {
                auto tr = world.get_robot_blue(target);
                if (tr) {
                    float dx_t = tr->x - robot->x;
                    float dy_t = tr->y - robot->y;
                    float d    = std::sqrt(dx_t*dx_t + dy_t*dy_t);
                    cmd.vx = (dx_t/d)*1.2f; cmd.vy = (dy_t/d)*1.2f;
                    cmd.kick = true; cmd.kick_power = PASS_POWER;
                    acted_ = true; last_act_ = now;
                    std::cout << "[PASSE] Robo " << robot_id
                              << " -> Robo " << target << "\n";
                }
            } else {
                // Avança com a bola chutando para frente
                float dx_g = goal_x_ - robot->x;
                float dy_g = -robot->y * 0.1f;
                float d    = std::sqrt(dx_g*dx_g + dy_g*dy_g);
                cmd.vx = (dx_g/d)*1.5f; cmd.vy = (dy_g/d)*1.5f;
                cmd.kick = true; cmd.kick_power = 2.0f;
                acted_ = true; last_act_ = now;
                std::cout << "[AVANCA] Robo " << robot_id << "\n";
            }
        }
    } else if (db < 600.0f) {
        cmd.vx = (dx_b/db)*APPROACH_SPD;
        cmd.vy = (dy_b/db)*APPROACH_SPD;
    } else {
        float dx_g = goal_x_ - ball->x;
        float dy_g = -ball->y;
        float d    = std::sqrt(dx_g*dx_g + dy_g*dy_g);
        if (d < 1.0f) d = 1.0f;
        float ax = ball->x - (dx_g/d)*280.0f;
        float ay = ball->y - (dy_g/d)*280.0f;
        float dax = ax - robot->x;
        float day = ay - robot->y;
        float da  = std::sqrt(dax*dax + day*day);
        if (da > 30.0f) {
            float spd = std::min(da*0.003f, APPROACH_SPD);
            cmd.vx = (dax/da)*spd; cmd.vy = (day/da)*spd;
        }
    }
    return cmd;
}
