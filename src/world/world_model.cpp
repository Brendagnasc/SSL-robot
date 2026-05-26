#include "world_model.h"
#include <iostream>
#include <sstream>

WorldModel::WorldModel() {
    mc_ = memcached_create(nullptr);
    memcached_server_st* servers = memcached_server_list_append(
        nullptr, "127.0.0.1", 11211, nullptr
    );
    memcached_server_push(mc_, servers);
    memcached_server_list_free(servers);
    memcached_behavior_set(mc_, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
    std::cout << "[WorldModel] Conectado ao Memcached\n";
}

WorldModel::~WorldModel() {
    memcached_free(mc_);
}

void WorldModel::mc_set(const std::string& key, const std::string& value, time_t ttl) {
    memcached_return_t rc = memcached_set(
        mc_, key.c_str(), key.size(),
        value.c_str(), value.size(), ttl, 0
    );
    if (rc != MEMCACHED_SUCCESS)
        std::cerr << "[WorldModel] Erro em " << key << ": "
                  << memcached_strerror(mc_, rc) << "\n";
}

std::string WorldModel::mc_get(const std::string& key) {
    size_t val_len;
    uint32_t flags;
    memcached_return_t rc;
    char* val = memcached_get(mc_, key.c_str(), key.size(), &val_len, &flags, &rc);
    if (rc != MEMCACHED_SUCCESS || !val) return "";
    std::string result(val, val_len);
    free(val);
    return result;
}

void WorldModel::set_ball(const BallState& b) {
    std::ostringstream ss;
    ss << b.x << "," << b.y << "," << b.vx << "," << b.vy << "," << b.timestamp_ms;
    mc_set("ssl:ball:pos", ss.str(), 1);
}

std::optional<BallState> WorldModel::get_ball() {
    std::string raw = mc_get("ssl:ball:pos");
    if (raw.empty()) return std::nullopt;
    BallState b;
    sscanf(raw.c_str(), "%f,%f,%f,%f,%ld", &b.x, &b.y, &b.vx, &b.vy, &b.timestamp_ms);
    return b;
}

void WorldModel::set_robot_blue(const RobotState& r) {
    std::string key = "ssl:robot:blue:" + std::to_string(r.id);
    std::ostringstream ss;
    ss << r.x << "," << r.y << "," << r.angle << "," << r.timestamp_ms;
    mc_set(key, ss.str(), 1);
}

std::optional<RobotState> WorldModel::get_robot_blue(int id) {
    std::string key = "ssl:robot:blue:" + std::to_string(id);
    std::string raw = mc_get(key);
    if (raw.empty()) return std::nullopt;
    RobotState r; r.id = id;
    sscanf(raw.c_str(), "%f,%f,%f,%ld", &r.x, &r.y, &r.angle, &r.timestamp_ms);
    return r;
}

void WorldModel::set_robot_yellow(const RobotState& r) {
    std::string key = "ssl:robot:yellow:" + std::to_string(r.id);
    std::ostringstream ss;
    ss << r.x << "," << r.y << "," << r.angle << "," << r.timestamp_ms;
    mc_set(key, ss.str(), 1);
}

std::optional<RobotState> WorldModel::get_robot_yellow(int id) {
    std::string key = "ssl:robot:yellow:" + std::to_string(id);
    std::string raw = mc_get(key);
    if (raw.empty()) return std::nullopt;
    RobotState r; r.id = id;
    sscanf(raw.c_str(), "%f,%f,%f,%ld", &r.x, &r.y, &r.angle, &r.timestamp_ms);
    return r;
}

void WorldModel::set_game_state(const std::string& state) {
    mc_set("ssl:game:state", state, 0);
}

std::string WorldModel::get_game_state() {
    return mc_get("ssl:game:state");
}
