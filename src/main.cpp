#include <iostream>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include "vision/vision_client.h"
#include "world/world_model.h"
#include "strategy/strategy_engine.h"
#include "skills/move_to_position.h"
#include "comm/robot_comm.h"

int main() {
    std::cout << "=== SSL Robot iniciando ===\n";

    WorldModel     world;
    StrategyEngine strategy(world);
    RobotComm      comm("127.0.0.1", 10301);

    world.set_game_state("RUNNING");

    // Skills para cada robô
    MoveToPosition skills[6] = {
        {0, -4000},   // goleiro
        {-2000, -1000}, // defensor 1
        {-2000,  1000}, // defensor 2
        {0, 0},       // meio 1
        {0, 0},       // meio 2
        {0, 0}        // atacante
    };

    std::thread strategy_thread([&]() {
        while (true) {
            strategy.update();
            auto roles = strategy.get_roles();
            auto ball  = world.get_ball();

            for (auto& role : roles) {
                int id = role.robot_id;

                if (role.role == Role::ATTACKER && ball) {
                    // Atacante segue a bola
                    skills[id].set_target(ball->x, ball->y);
                }

                auto cmd = skills[id].execute(id, world);
                comm.send(cmd);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60Hz
        }
    });
    strategy_thread.detach();

    boost::asio::io_context io;
    VisionClient vision(io, world);
    vision.start();

    std::cout << "[Main] Sistema rodando a 60Hz...\n";
    io.run();

    return 0;
}
