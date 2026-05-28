#pragma once
#include <string>
#include <boost/asio.hpp>
#include "ssl_referee_simple.pb.h"
#include "world/world_model.h"

class RefereeClient {
public:
    RefereeClient(boost::asio::io_context& io,
                  WorldModel& world,
                  const std::string& host = "224.5.23.1",
                  unsigned short port = 10003);
    void start();

private:
    void receive();
    void process(const Referee& ref);

    boost::asio::ip::udp::socket   socket_;
    boost::asio::ip::udp::endpoint sender_;
    WorldModel&                    world_;
    std::array<char, 65536>        buffer_;
};
