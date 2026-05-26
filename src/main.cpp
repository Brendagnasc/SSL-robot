#include <iostream>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include "vision/vision_client.h"
#include "world/world_model.h"
#include "strategy/strategy_engine.h"

int main() {
    std::cout << "=== SSL Robot iniciando ===\n";

    WorldModel world;
    world.set_game_state("RUNNING");

    // Thread do Strategy Engine (roda a cada 100ms)
    StrategyEngine strategy(world);
    std::thread strategy_thread([&]() {
        while (true) {
            strategy.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    strategy_thread.detach();

    // Vision Client (loop principal)
    boost::asio::io_context io;
    VisionClient vision(io, world);
    vision.start();

    std::cout << "[Main] Aguardando dados do SSL Vision...\n";
    io.run();

    return 0;
}
