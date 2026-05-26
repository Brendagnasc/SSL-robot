#pragma once
#include <boost/asio.hpp>
#include "skills/skill.h"

class RobotComm {
public:
    RobotComm(const std::string& host = "127.0.0.1",
              unsigned short port = 10301);
    ~RobotComm();

    void send(const RobotCmd& cmd);

private:
    boost::asio::io_context        io_;
    boost::asio::ip::udp::socket   socket_;
    boost::asio::ip::udp::endpoint endpoint_;
};
