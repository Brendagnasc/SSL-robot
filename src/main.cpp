#include <iostream>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include "vision/vision_client.h"
#include "world/world_model.h"
#include "strategy/strategy_engine.h"
#include "skills/move_to_position.h"

int main() {
    std::cout << "=== SSL Robot iniciando ===\n";

    WorldModel world;
    world.set_game_state("RUNNING");

    StrategyEngine strategy(world);
    MoveToPosition move_skill(0, 0);

    // Thread de táticas e skills (100ms)
    std::thread strategy_thread([&]() {
        while (true) {
            strategy.update();

            // Atacante (robô 5) segue a bola
            auto ball = world.get_ball();
            if (ball) {
                move_skill.set_target(ball->x, ball->y);
                auto cmd = move_skill.execute(5, world);
                std::cout << "[Main] Cmd robô " << cmd.robot_id
                          << " vx=" << cmd.vx
                          << " vy=" << cmd.vy << "\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    strategy_thread.detach();

    boost::asio::io_context io;
    VisionClient vision(io, world);
    vision.start();

    std::cout << "[Main] Aguardando dados do SSL Vision...\n";
    io.run();

    return 0;
}
