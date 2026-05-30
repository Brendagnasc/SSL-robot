#include "move_to_position.h"
#include <cmath>
#include <algorithm>

MoveToPosition::MoveToPosition(float target_x, float target_y, float target_angle)
    : target_x_(target_x), target_y_(target_y), target_angle_(target_angle), done_(false)
{}

void MoveToPosition::set_target(float x, float y, float angle) {
    target_x_     = std::clamp(x, -4400.0f, 4400.0f);
    target_y_     = std::clamp(y, -2900.0f, 2900.0f);
    target_angle_ = angle;
    done_         = false;
}

bool MoveToPosition::is_done() const { return done_; }

static std::pair<float,float> compute_repulsion(
    float rx, float ry, WorldModel& world, int self_id)
{
    float fx = 0, fy = 0;
    // Repulsão dos próprios robôs
    for (int i = 0; i < 6; i++) {
        if (i == self_id) continue;
        auto r = world.get_robot_blue(i);
        if (!r) continue;
        float dx = rx - r->x, dy = ry - r->y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 180.0f && dist > 1.0f) {
            float f = (180.0f - dist) / 180.0f * 0.4f;
            fx += (dx/dist)*f; fy += (dy/dist)*f;
        }
    }
    // Repulsão dos robôs adversários
    for (int i = 0; i < 6; i++) {
        auto r = world.get_robot_yellow(i);
        if (!r) continue;
        float dx = rx - r->x, dy = ry - r->y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 220.0f && dist > 1.0f) {
            float f = (220.0f - dist) / 220.0f * 0.5f;
            fx += (dx/dist)*f; fy += (dy/dist)*f;
        }
    }
    return {fx, fy};
}

RobotCmd MoveToPosition::execute(int robot_id, WorldModel& world) {
    RobotCmd cmd;
    cmd.robot_id = robot_id;
    cmd.vx = cmd.vy = cmd.vw = 0;
    cmd.kick = false; cmd.kick_power = 0;

    auto robot = world.get_robot_blue(robot_id);
    if (!robot) return cmd;

    float dx   = target_x_ - robot->x;
    float dy   = target_y_ - robot->y;
    float dist = std::sqrt(dx*dx + dy*dy);

    if (dist < 30.0f) return cmd;

    float vx = KP * dx;
    float vy = KP * dy;

    if (dist > 150.0f) {
        auto [rx, ry] = compute_repulsion(robot->x, robot->y, world, robot_id);
        vx += rx; vy += ry;
    }

    float speed = std::sqrt(vx*vx + vy*vy);
    if (speed > MAX_SPEED) { vx = vx/speed*MAX_SPEED; vy = vy/speed*MAX_SPEED; }

    float ae = target_angle_ - robot->angle;
    while (ae >  M_PI) ae -= 2*M_PI;
    while (ae < -M_PI) ae += 2*M_PI;

    cmd.vx = vx; cmd.vy = vy; cmd.vw = 2.0f*ae;
    return cmd;
}
