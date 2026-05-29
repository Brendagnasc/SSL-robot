#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <boost/asio.hpp>
#include "vision/vision_client.h"
#include "referee/referee_client.h"
#include "world/world_model.h"
#include "strategy/strategy_engine.h"
#include "strategy/formation.h"
#include "skills/move_to_position.h"
#include "skills/goalkeeper.h"
#include "skills/kick_to_goal.h"
#include "skills/intercept_ball.h"
#include "dashboard/dashboard.h"
#include "comm/robot_comm.h"

std::pair<float,float> midfielder_position(int robot_id, const BallState& ball) {
    float side = (robot_id == 3) ? -1.0f : 1.0f;
    if (ball.x > 0)
        return { ball.x - 600.0f, side * 600.0f };
    else
        return { -500.0f, side * 500.0f };
}

void apply_formation(const std::array<RobotTarget, 6>& formation,
                     std::array<MoveToPosition, 6>& skills,
                     WorldModel& world, RobotComm& comm) {
    for (const auto& t : formation) {
        skills[t.robot_id].set_target(t.x, t.y);
        auto cmd = skills[t.robot_id].execute(t.robot_id, world);
        comm.send(cmd);
    }
}

int main() {
    std::cout << "=== SSL Robot iniciando ===\n";

    WorldModel     world;
    StrategyEngine strategy(world);
    RobotComm      comm("127.0.0.1", 10301);
    Dashboard      dashboard(world, strategy);

    world.set_game_state("RUNNING");

    Goalkeeper    gk_skill(-3800.0f);
    KickToGoal    kick_skill(4500.0f, 0.0f);
    InterceptBall intercept1(-2500.0f);
    InterceptBall intercept2(-2500.0f);
    MoveToPosition mid3(-500.0f, -500.0f);
    MoveToPosition mid4(-500.0f,  500.0f);
    MoveToPosition mid5( 500.0f,    0.0f);

    std::array<MoveToPosition, 6> formation_skills = {{
        {-3800, 0}, {-2000, -800}, {-2000, 800},
        {-500, 0},  {-800, -600},  {-800, 600}
    }};

    std::atomic<int> frame{0};

    std::thread strategy_thread([&]() {
        while (true) {
            auto state = world.get_game_state();
            strategy.update();
            auto roles = strategy.get_roles();
            auto ball  = world.get_ball();

            if (state == "HALT") {
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
                continue;
            }

            if (state == "STOP")
                apply_formation(formation_stop(), formation_skills, world, comm);
            else if (state == "KICKOFF_US")
                apply_formation(formation_kickoff_us(), formation_skills, world, comm);
            else if (state == "KICKOFF_THEM")
                apply_formation(formation_kickoff_them(), formation_skills, world, comm);
            else if (state == "FREE_KICK_US")
                apply_formation(formation_free_kick_us(), formation_skills, world, comm);
            else {
                for (auto& role : roles) {
                    int id = role.robot_id;
                    RobotCmd cmd;
                    cmd.robot_id = id;
                    cmd.vx = cmd.vy = cmd.vw = 0;
                    cmd.kick = false; cmd.kick_power = 0;

                    switch (role.role) {
                        case Role::GOALKEEPER:
                            cmd = gk_skill.execute(id, world); break;
                        case Role::DEFENDER:
                            cmd = (id == 1) ? intercept1.execute(id, world)
                                            : intercept2.execute(id, world); break;
                        case Role::ATTACKER:
                            cmd = kick_skill.execute(id, world); break;
                        case Role::MIDFIELDER:
                            if (ball) {
                                if (id == 3) {
                                    auto [tx, ty] = midfielder_position(3, *ball);
                                    mid3.set_target(tx, ty);
                                    cmd = mid3.execute(id, world);
                                } else if (id == 4) {
                                    auto [tx, ty] = midfielder_position(4, *ball);
                                    mid4.set_target(tx, ty);
                                    cmd = mid4.execute(id, world);
                                } else {
                                    mid5.set_target(ball->x - 400.0f, ball->y);
                                    cmd = mid5.execute(id, world);
                                }
                            }
                            break;
                    }
                    comm.send(cmd);
                }
            }

            // Dashboard a cada 30 frames (~2x por segundo)
            if (frame++ % 30 == 0)
                dashboard.render();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    });
    strategy_thread.detach();

    boost::asio::io_context io;
    VisionClient  vision(io, world);
    RefereeClient referee(io, world);
    vision.start();
    referee.start();

    std::cout << "[Main] Sistema rodando...\n";
    io.run();

    return 0;
}
