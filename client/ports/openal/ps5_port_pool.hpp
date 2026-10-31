/* SPDX-License-Identifier: LGPL-2.0-or-later */
#pragma once
#include <mutex>
#include "ps5_audio_mutex.hpp"
namespace ps5audio {
// Keep at most two drained native output reserved across logical device reloads.
// The process owns this reservation until title exit; no mixer thread is retained.
class PortPool {
 ps5audio::Mutex mutex;int idle[2]={-1,-1};
public:
 int take(){const auto lock=std::lock_guard{mutex};for(auto& value:idle)if(value>=0){int port=value;value=-1;return port;}return -1;}
 bool keep(int port){const auto lock=std::lock_guard{mutex};if(port<0)return false;for(auto value:idle)if(value==port)return false;for(auto& value:idle)if(value<0){value=port;return true;}return false;}
};
}
