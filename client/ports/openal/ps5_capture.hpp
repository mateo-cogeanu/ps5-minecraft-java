/* SPDX-License-Identifier: LGPL-2.0-or-later */
#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <algorithm>
#include <mutex>
#include "ps5_audio_mutex.hpp"
namespace ps5audio {
// OS boundary is injected for actual-code host failure/lifecycle tests.
struct CaptureApi {
 int (*initialUser)(int*);int (*init)();int (*open)(int,unsigned,unsigned,unsigned,unsigned,unsigned);
 int (*input)(int,void*);int (*silent)(int);int (*reroute)(int);int (*close)(int);
};
struct Capture {
 static constexpr unsigned Grain=256,InputRate=16000,Rate=48000,Capacity=16384;
 ps5audio::Mutex mutex;CaptureApi api{};int handle=-1,route=-1,error=0;
 bool healthy=false,close_uncertain=false;float previous=0;bool previous_valid=false;
 std::array<float,Capacity> ring{};unsigned read=0,count=0;
 std::uint64_t blocks=0,nonzero=0,silent_blocks=0,dropped=0;int peak=0;
 alignas(64) std::array<std::int16_t,Grain> pcm{};
 bool open(CaptureApi calls){
  const auto lock=std::lock_guard{mutex};if(handle>=0)return false;api=calls;
  int user=-1;error=api.initialUser(&user);if(error<0||user<0)return false;
  error=api.init();if(error<0)return false;
  handle=api.open(user,1,0,Grain,InputRate,1); // PS5 General, index0, S16 mono profile.
  if(handle<0){error=handle;return false;}
  route=api.reroute(handle);if(route<0){error=route;closeLocked();return false;}
  healthy=true;error=0;read=count=0;previous_valid=false;blocks=nonzero=silent_blocks=dropped=0;peak=0;return true;
 }
 bool closeLocked(){if(handle<0)return true;if(close_uncertain)return false;int rc=api.close(handle);if(rc<0){error=rc;healthy=false;close_uncertain=true;return false;}handle=-1;healthy=false;read=count=0;previous_valid=false;return true;}
 bool close(){const auto lock=std::lock_guard{mutex};return closeLocked();}
 // One producer. Caller joins it before close. Silence queries before AND after
 // input conservatively discard the complete block during mute/route changes.
 bool pump(){
  if(handle<0||!healthy)return false;int before=api.silent(handle),route_before=api.reroute(handle);
  int result=api.input(handle,pcm.data());int after=api.silent(handle),route_after=api.reroute(handle);
  const auto lock=std::lock_guard{mutex};
  if(result!=int(Grain)||before<0||after<0||route_before<0||route_after<0){error=result<0?result:before<0?before:after<0?after:route_before<0?route_before:route_after<0?route_after:-1;healthy=false;read=count=0;return false;}
  bool silent=before!=0||after!=0||route_before!=route_after||route_after!=route;
  if(route_after!=route){route=route_after;read=count=0;}++blocks;
  if(silent){++silent_blocks;std::fill(pcm.begin(),pcm.end(),0);previous_valid=false;read=count=0;}
  for(auto sample:pcm){int magnitude=sample<0?-int(sample):int(sample);if(magnitude){++nonzero;peak=std::max(peak,magnitude);}float current=float(sample)/32768.f;if(!previous_valid){previous=current;previous_valid=true;}
   // Exact rational 16->48k interpolation, bounded history; no rate drift.
   for(int step=1;step<=3;step++){if(count==Capacity){read=(read+1)%Capacity;--count;++dropped;}ring[(read+count)%Capacity]=previous+(current-previous)*(float(step)/3.f);++count;}previous=current;
  }
  return true;
 }
 unsigned available(){const auto lock=std::lock_guard{mutex};return count;}
 bool take(void* output,unsigned frames,bool floating){
  const auto lock=std::lock_guard{mutex};if(frames>count)return false;
  for(unsigned i=0;i<frames;i++){float value=ring[read];if(floating)static_cast<float*>(output)[i]=value;else static_cast<std::int16_t*>(output)[i]=std::int16_t(std::clamp(value*32768.f,-32768.f,32767.f));read=(read+1)%Capacity;}count-=frames;return true;
 }
 void clear(){const auto lock=std::lock_guard{mutex};read=count=0;previous_valid=false;}
};
}
