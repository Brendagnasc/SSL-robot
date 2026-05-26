#include <iostream>
#include <boost/asio.hpp>
#include "vision/vision_client.h"
#include "world/world_model.h"

int main() {
    std::cout << "=== SSL Robot iniciando ===\n";

    WorldModel world;
    world.set_game_state("HALT");

    boost::asio::io_context io;
    VisionClient vision(io, world);
    vision.start();

    std::cout << "[Main] Aguardando dados do SSL Vision...\n";
    io.run();

    return 0;
}
