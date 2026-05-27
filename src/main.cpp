#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
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

    // Campo SSL: X=±4500mm, Y=±3000mm
    // Time azul defende o gol em X negativo
    MoveToPosition skills[6] = {
        {-3800,    0},   // R0: goleiro na frente do gol
        {-2500, -800},   // R1: defensor esquerdo
        {-2500,  800},   // R2: defensor direito
        { -500, -500},   // R3: meio esquerdo
        { -500,  500},   // R4: meio direito
        {  500,    0}    // R5: atacante avançado
    };

    std::atomic<int> frame{0};

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

            if (frame++ % 60 == 0) {
                std::cout << "\n--- Frame " << frame << " ---\n";
                if (ball)
                    std::cout << "Bola: (" << ball->x << ", " << ball->y << ")\n";
                for (auto& role : roles) {
                    auto r = world.get_robot_blue(role.robot_id);
                    if (r)
                        std::cout << "R" << role.robot_id
                                  << " [" << role_to_string(role.role) << "]"
                                  << " pos=(" << r->x << ", " << r->y << ")\n";
                }
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
