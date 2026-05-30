#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cmath>
#include <algorithm>
#include <boost/asio.hpp>
#include "vision/vision_client.h"
#include "referee/referee_client.h"
#include "world/world_model.h"
#include "strategy/strategy_engine.h"
#include "strategy/formation.h"
#include "skills/move_to_position.h"
#include "skills/goalkeeper.h"
#include "skills/pass_skill.h"
#include "skills/intercept_ball.h"
#include "dashboard/dashboard.h"
#include "comm/robot_comm.h"

struct FieldPos { float x, y; };

FieldPos support_position(int robot_id, const BallState& ball) {
    float bx = ball.x;
    float by = ball.y;

    // Meias sempre ficam na frente da bola criando opções de passe
    if (bx > 0) {
        // Campo adversário
        switch (robot_id) {
            case 1: return { std::max(bx-1800,-3000.f), -600 };
            case 2: return { std::max(bx-1800,-3000.f),  600 };
            case 3: return { std::min(bx+400, 4000.f), -700 }; // frente esquerda
            case 4: return { std::min(bx+400, 4000.f),  700 }; // frente direita
            case 5: return { std::min(bx+200, 4000.f), by*0.3f }; // suporte centro
            default: return {-3800,0};
        }
    } else {
        // Campo nosso — defende mas mantém um na frente
        switch (robot_id) {
            case 1: return { -2500, -700 };
            case 2: return { -2500,  700 };
            case 3: return { bx+800, -600 };
            case 4: return { bx+800,  600 };
            case 5: return { std::max(bx+1200,-500.f), by*0.2f };
            default: return {-3800,0};
        }
    }
}

void apply_formation(const std::array<RobotTarget,6>& f,
                     std::array<MoveToPosition,6>& s,
                     WorldModel& w, RobotComm& c) {
    for (auto& t : f) {
        s[t.robot_id].set_target(t.x, t.y);
        c.send(s[t.robot_id].execute(t.robot_id, w));
    }
}

int main() {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║     SSL Robot - RoboCup Division A   ║\n";
    std::cout << "║     World Model com Memcached         ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";

    WorldModel     world;
    StrategyEngine strategy(world);
    RobotComm      comm("127.0.0.1", 10301);
    Dashboard      dashboard(world, strategy);

    world.set_game_state("RUNNING");

    std::cout << "\n[DEMO] Mostrando operacoes do Memcached por 5 segundos...\n\n";
    world.enable_log(true);

    Goalkeeper    gk_skill(-3800.0f);
    PassSkill     pass_skill(4500.0f);
    InterceptBall intercept1(-2500.0f);
    InterceptBall intercept2(-2500.0f);

    std::array<MoveToPosition,6> skills = {{
        {-3800,0},{-2500,-700},{-2500,700},{500,-700},{500,700},{800,0}
    }};
    std::array<MoveToPosition,6> fskills = {{
        {-3800,0},{-2000,-800},{-2000,800},{-500,0},{-800,-600},{-800,600}
    }};

    std::atomic<int>  frame{0};
    std::atomic<bool> show_log{true};

    std::thread([&](){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        world.enable_log(false); show_log=false;
        std::cout << "\n[DEMO] Log desligado — modo jogo!\n\n";
    }).detach();

    std::thread([&](){
        while(true) {
            auto state = world.get_game_state();
            strategy.update();
            auto roles = strategy.get_roles();
            auto ball  = world.get_ball();

            if (state=="HALT") { std::this_thread::sleep_for(std::chrono::milliseconds(16)); continue; }

            if      (state=="STOP")         apply_formation(formation_stop(),         fskills, world, comm);
            else if (state=="KICKOFF_US")   apply_formation(formation_kickoff_us(),   fskills, world, comm);
            else if (state=="KICKOFF_THEM") apply_formation(formation_kickoff_them(), fskills, world, comm);
            else if (state=="FREE_KICK_US") apply_formation(formation_free_kick_us(), fskills, world, comm);
            else {
                for (auto& role : roles) {
                    int id = role.robot_id;
                    RobotCmd cmd; cmd.robot_id=id;
                    cmd.vx=cmd.vy=cmd.vw=0; cmd.kick=false; cmd.kick_power=0;

                    if (role.role==Role::GOALKEEPER)
                        cmd = gk_skill.execute(id, world);
                    else if (role.role==Role::ATTACKER)
                        cmd = pass_skill.execute(id, world);
                    else if (role.role==Role::DEFENDER)
                        cmd = (id==1) ? intercept1.execute(id,world) : intercept2.execute(id,world);
                    else if (ball) {
                        auto pos = support_position(id, *ball);
                        skills[id].set_target(pos.x, pos.y);
                        cmd = skills[id].execute(id, world);
                    }
                    comm.send(cmd);
                }
            }

            if (!show_log && frame++%30==0) dashboard.render();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }).detach();

    boost::asio::io_context io;
    VisionClient  vision(io, world);
    RefereeClient referee(io, world);
    vision.start(); referee.start();
    io.run();
}
