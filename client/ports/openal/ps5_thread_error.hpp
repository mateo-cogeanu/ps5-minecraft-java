/* SPDX-License-Identifier: LGPL-2.0-or-later */
#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
namespace ps5audio {
// Context-local AL errors on each calling thread. Generation tags prevent old
// errors surviving a context slot/address reuse. Compiler TLS uses the title's
// existing emutls owner, rather than allocating one native key per late context.
// Calls after context destruction remain invalid, as in the upstream API.
template<class T> class ThreadError {
 static_assert(std::is_integral_v<T>);
 static constexpr unsigned Capacity=16;
 struct Value {std::uint64_t generation=0;T error{};};
 inline static std::array<std::atomic<std::uint64_t>,Capacity> owners{};
 inline static std::atomic<std::uint64_t> sequence{1};
 inline static thread_local std::array<Value,Capacity> values{};
 unsigned slot=Capacity;std::uint64_t generation;T initial;
public:
 explicit ThreadError(T init):generation(sequence.load(std::memory_order_relaxed)),initial(init){
  for(;;){if(generation==UINT64_MAX)throw std::runtime_error{"PS5 AL error generation exhausted"};if(sequence.compare_exchange_weak(generation,generation+1,std::memory_order_relaxed))break;}
  for(unsigned i=0;i<Capacity;++i){std::uint64_t empty=0;if(owners[i].compare_exchange_strong(empty,generation,std::memory_order_acq_rel)){slot=i;break;}}
  if(slot==Capacity)throw std::runtime_error{"PS5 AL error context capacity exhausted"};
 }
 ~ThreadError(){owners[slot].store(0,std::memory_order_release);}
 ThreadError(const ThreadError&)=delete;ThreadError& operator=(const ThreadError&)=delete;
 T get()const noexcept {const auto& value=values[slot];return value.generation==generation?value.error:initial;}
 void set(T error) noexcept {values[slot]={generation,error};}
};
}
