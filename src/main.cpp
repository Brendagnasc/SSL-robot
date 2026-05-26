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

    // Posições fixas em mm (campo SSL: 9000x6000mm)
    MoveToPosition skills[6] = {
        {   0, -2500},  // R0: goleiro (embaixo do gol)
        {-1500,  -800},  // R1: defensor esquerdo
        {-1500,   800},  // R2: defensor direito
        {   0,     0},  // R3: meio (começa no centro)
        { 500,  -500},  // R4: meio ofensivo
        {1000,     0}   // R5: atacante
    };

    std::thread strategy_thread([&]() {
        while (true) {
            strategy.update();
            auto roles = strategy.get_roles();
            auto ball  = world.get_ball();

            for (auto& role : roles) {
                int id = role.robot_id;

                if (role.role == Role::ATTACKER && ball)
                    skills[id].set_target(ball->x, ball->y);

                auto cmd = skills[id].execute(id, world);
                comm.send(cmd);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
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
