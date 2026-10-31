#!/usr/bin/env python3
"""Exercise the actual context-error owner under native-key refusal and reuse."""
from pathlib import Path
import subprocess,tempfile
from host_toolchain import executable
ROOT=Path(__file__).resolve().parents[1]
code=r'''
#include "ps5_thread_error.hpp"
#include <cassert>
#include <cerrno>
#include <thread>
#include <atomic>
#include <memory>
extern "C" int pthread_setspecific(pthread_key_t,const void*)
#ifdef __linux__
 noexcept
#endif
 {return ENOMEM;}
using Error=ps5audio::ThreadError<int>;
int main(){
 for(int n=0;n<1000;++n){Error a(0),b(0);assert(!a.get()&&!b.get());a.set(7);assert(a.get()==7&&!b.get());}
 Error a(0),b(0);a.set(9);std::atomic<unsigned> ready{0};std::thread threads[8];
 for(unsigned i=0;i<8;++i)threads[i]=std::thread([&,i]{assert(!a.get()&&!b.get());a.set(i+1);b.set(i+11);++ready;while(ready<8)std::this_thread::yield();assert(a.get()==int(i+1)&&b.get()==int(i+11));});
 for(auto&t:threads)t.join();assert(a.get()==9&&!b.get());
 std::unique_ptr<Error> slots[14];for(auto&v:slots)v=std::make_unique<Error>(0);bool refused=false;try{Error overflow(0);}catch(const std::runtime_error&){refused=true;}assert(refused);
 slots[3].reset();slots[3]=std::make_unique<Error>(0);assert(!slots[3]->get());
 ps5audio::PortPool *unused=nullptr;(void)unused;
}
'''.replace(' ps5audio::PortPool *unused=nullptr;(void)unused;','')
with tempfile.TemporaryDirectory()as folder:
 w=Path(folder);(w/'check.cpp').write_text(code)
 subprocess.run([executable('clang++',env='CXX'),'-std=c++17','-pthread','-fsanitize=address,undefined','-I'+str(ROOT/'client/ports/openal'),str(w/'check.cpp'),'-o',str(w/'check')],check=True)
 subprocess.run([str(w/'check')],check=True)
print('PASS actual context error storage:1000 generation reuses,8 concurrent isolated threads,16-context exhaustion/reuse,native key setter refusal; ASan/UBSan')
