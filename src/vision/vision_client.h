#pragma once
#include <array>
#include <string>
#include <boost/asio.hpp>
#include "messages_robocup_ssl_wrapper.pb.h"
#include "world/world_model.h"

class VisionClient {
public:
    VisionClient(boost::asio::io_context& io,
                 WorldModel& world,
                 const std::string& host = "224.5.23.2",
                 unsigned short port = 10006);
    void start();

private:
    void receive();
    void process(const SSL_WrapperPacket& pkt);

    boost::asio::ip::udp::socket   socket_;
    boost::asio::ip::udp::endpoint sender_;
    WorldModel&                    world_;
    std::array<char, 65536>        buffer_;
};
