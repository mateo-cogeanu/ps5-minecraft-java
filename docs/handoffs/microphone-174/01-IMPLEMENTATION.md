# Microphone implementation and unresolved engineering questions

Paths below are relative to the consumer checkout unless identified as runtime.
Read the source itself before editing. The bundled manifest hashes identify the
exact handoff version; source line numbers may change.

## End-to-end path

```text
Saved microphoneEnabled setting (default false)
  -> ProsperoLauncher -> ProsperoMicrophone.java
  -> standard System.loadLibrary("ps5openal") + resident JNI
  -> microphone_jni.c -> enable flag or five-second native probe

Original SVC -> LWJGL alcCapture* -> genuine patched OpenAL Soft
  -> Ps5BackendFactory -> Ps5Capture -> exclusive CaptureLease
  -> ps5audio::Capture -> real resident PS5 AudioIn APIs
  -> bounded 16kHz S16 mono -> 48kHz mono conversion
  -> OpenAL captured float32/S16 samples -> original SVC codec/transport
```

This is native PS5 capture, not a browser microphone, Java Sound replacement,
loopback voice source, fake input device or modified game/mod JAR.

## Files and ownership

| Consumer path | Responsibility |
| --- | --- |
| client/ports/openal/ps5_capture.hpp | Actual injectable capture owner, native API calls, sample ring/conversion, mute/reroute handling and cleanup state. |
| client/ports/openal/ps5_backend.cpp | Real AudioOut backend, capture lease/device/worker, opt-in flag, enumeration and test probe. |
| client/ports/openal/ps5_backend.h | OpenAL backend factory declaration. |
| client/ports/openal/ps5_audio_mutex.hpp | Allocation-free scoped nonrecursive audio mutex. |
| client/ports/openal/microphone_jni.c | nativeEnable/nativeProbe JNI glue. |
| client/ports/openal/ps5_audioin_imports.c | Link-only system import declarations; bodies must never supply actual app audio behavior. |
| client/launcher/java/ProsperoMicrophone.java | Standard resident library load and JNI wrapper. |
| client/launcher/java/ProsperoLauncher.java | Saved-setting activation, page16 and asynchronous test task/result. |
| client/launcher/java/ProsperoLibrary.java | microphoneEnabled persisted setting, default false. |
| client/startup/bootstrap.cpp | Pre-JVM output/worker reservation, runtime policy setters, startup/log ownership. |
| tools/build_openal_native.py | Coherent private OpenAL native build/layout and audit. |
| tools/build_openal_startup_boundaries.py | Startup boundary instrumentation and actual PS5 backend integration. |
| tools/package_client_vulkan.py | Resident ps5openal group, JNI/import packaging and app build. |
| tools/build_client_openal_relink.py | Exact inputs, modified source, SDK headers and rebuild/replacement kit. |
| tools/test_microphone_capture.py | Actual capture core with injected OS calls, ASan/UBSan. |
| tools/test_openal_ps5_audio.py | Actual upstream OpenAL plus PS5 backend with controlled OS boundaries. |
| client/ports/socket-preclose-runtime.json | Required runtime0135 source/object hashes. |
| tools/test_client_startup_package.py | Actual package/runtime symbol/hash/import/source audit. |

Reusable runtime code lives in the Java repository. Read
`Development/docs/java25-socket-preclose.md` and its validation, plus
`Development/java25/patches/0135-ps5-socket-preclose-shutdown.patch` and
`Development/tools/test_java25_socket_preclose.py`. The local applied source is
`.tools/java25/openjdk/src/java.base/unix/native/libnio/ch/UnixDispatcher.c`
in the runtime workspace. Cached edits alone are not publishable implementation:
record a proper reproducible patch in the runtime source tree.

## Capture core contract

Native profile: initial user from sceUserServiceGetInitialUser;
sceAudioInOpen(user,1,0,256,16000,1). Current declaration evidence interprets this
as General/index0/S16 mono. One aligned256-sample int16 buffer. No alternate
constants or PS4 ABI guesses are attempted. The successful173 test is stronger
physical evidence than emulator acceptance but is not full ABI/audio validation.

Open locks the core, rejects an owned handle, obtains the initial user, calls
AudioInInit only until it succeeds, then opens a real handle and reads reroute
count. A failed init remains retryable. Successful init remains remembered across
close and failed opens; there is no deinit. This does not accommodate an unknown
external component initializing AudioIn first by accepting an arbitrary negative
“already initialized” code. Such a change would require verified evidence.

OpenAL accepts exactly48000Hz mono, float32 or S16. The single process-wide core
uses a compare-exchange lease. Multiple callers cannot own capture concurrently.
Disabled capture returns no capture enumeration result. Enumeration, when opted
in, actually opens, reads one block and closes; the name is returned only after
confirmed cleanup. Enumeration is not a free or purely descriptive operation.

The five-second probe intentionally bypasses the enable flag after the user
starts Test input; enabling voice capture alone does not open the mic. The probe
owns a lease, pumps313blocks (~5.008s if real reads pace16ms), reports blocks,
nonzero sample count, absolute peak and silent-block count, then closes. It saves
no recording. A successful probe does not run the mod's late pthread worker.
The result is shown in the launcher UI;173 did not persist it in downloadable
logs.174 logs fixed stages/hex errors for OpenAL open/enumeration/read failures;
its successful probe summary still need not appear in those logs.

SVC start clears the ring and creates a late128KiB pthread. One producer pumps
until stopped, error or disconnect. Stop sets kill and joins before clear/close.
No extra always-running microphone thread exists. Two pre-JVM workers elsewhere
are output mixers; they are not capture workers. Late thread/resource pressure
is relevant only if new logs get past the actual open;173 failed before start.

Each pump checks silent state and route count before and after input. Current
read contract requires result==256. Negative state/read/route or unexpected read
count marks unhealthy and empties the queue. Any nonzero silence state, route
change during the read or changed route discards queued/history audio and zeroes
the entire block. No mute/gain/routing setter is called. The implementation then
emits zeros for that block; physical privacy behavior still needs observation.

Linear interpolation emits exactly3 output samples per source sample. Initial
history starts at the first sample. Source Nyquist bandwidth is8kHz; this is not
a48kHz microphone fidelity claim. The16384-float ring (~341ms at48kHz) drops oldest
samples on overflow. Available/take/clear use the core mutex. One producer and
joining before close are assumed; do not add concurrent pump/close callers.

Close success clears handle/ring/history. Close failure marks unhealthy,
close_uncertain, and keeps the handle and global lease pinned until app restart.
No speculative second close, orphan release, ownership reset or fake success.
Error stages are fixed strings; no PCM, device/user identifier or auth data is
added to diagnostics.

## Important review targets, not established root causes

1. **Fresh174 physical result first.** Determine enumeration, open, worker start,
   sample delivery and SVC codec/transport status separately. Do not guess from a
   generic mod fallback message. Repeated AudioIn init was repaired but the
   earlier actual failure code is unavailable.
2. **Stop/join failure ownership.** Current Ps5Capture.stop ignores pthread_join's
   return, clears started and continues. A failed join could leave a producer
   using the core/device after cleanup. Review conservatively; never free/reuse
   memory or close a handle while a worker may still run. This is an observed
   code risk, not the173 crash cause.
3. **Read/stop blocking.** Five seconds is a fixed block count, not a proven
   wall-clock deadline if the native input call hangs. stop also waits for the
   native read to return. Verify actual read pacing and shutdown behavior; do
   not add an unverified close-from-another-thread race.
4. **Enable OFF during active capture.** Enable(false) changes the flag; it does
   not itself cancel a running capture pump. Current launcher/game flow may
   prevent this situation, but API-level privacy/lifecycle behavior needs review.
5. **Error fidelity.** Unexpected positive short reads become error=-1; the raw
   count is not retained. Initial-user returns negative user with rc0 could
   produce an unhelpful result0. Reroute-open cleanup failure may replace the
   original error with close error. Improve bounded diagnostics if evidence
   requires it, without leaking private paths/account data.
6. **Signal quality/mute/identity.** Peak32766 and many nonzero samples can be
   speech, noise, clipping or something else. Compare deliberately quiet/speaking
   and hardware-muted/unmuted states; no automatic DualSense identity exists.
7. **Init versus resource timing.** Launcher probe succeeds outside the game;
   late SVC calls can encounter native pressure or earlier ownership failure.
   Capture initialization may be shared with other components; preserve actual
   return codes and ownership when investigating.

## SVC and output context

Observed mod: original Fabric2.6.27+26.3; original Minecraft26.3. Artifact pins,
API evidence hashes and dependencies: `client/ports/voicechat-investigation.json`.
The inspected upstream source commit2e56198016e02fd99139d37f382b7155c5bd63f5 is
2.6.28 development, NOT a claim of byte equivalence to publisher2.6.27. Metadata
and upstream code can change; verify primary sources when researching further.

SVC's normal request is48kHz mono Float32,960frames per20ms. Java Sound fallback
currently cannot load jsound and has no matching capture line; enabling a fake
Java Sound provider is not a fix. Native Opus/noise-suppression/recording support
is not established by microphone counts. Inspected SVC has a genuine Java
Concentus fallback, but its exact console codec path needs validation. Matching
server-side SVC and reachable UDP voice transport are separate requirements.
Do not change the user's existing mod/config choices silently.

Earlier172 context error storage failed in al::tss::tss(T), causing OpenAL version
lookup failure and a stereo capture workaround.173 replaced only per-context
mLastThreadError with bounded generation-tagged compiler TLS. Actual173 capture
is now mono. Do not reintroduce that native key allocation or assume every future
capture failure is the old stereo problem.

Game/voice output have two genuine independently opened pre-JVM AudioOut ports,
two precreated mixer/event workers and a bounded retained drained-port pool.
Do not merge devices, fabricate speaker success or steal an output worker for
capture without a separate ownership/lifecycle design. C++ exception globals
use the runtime's genuine compiler-TLS branch; native memory/thread pressure
remains incompletely understood. Preserve working runtime allocation policies.

## Related title-screen repair — context only; primary maintainer owns this

173 close calls reach UnixDispatcher.preClose0, whose upstream dup2 is forbidden
with EPERM in the title sandbox. Runtime0135 uses checked shutdown(SHUT_RDWR)
for PS5 sockets, retries EINTR and accepts ENOTCONN. Java closing state, thread
signals and end/tryClose own the eventual close. It does not close/recycle an
in-flight descriptor. ENOTSOCK retains original non-socket dup2 errors; other
platform behavior stays unchanged. Host blocked UDP/TCP/accept tests pass, but
PS5 wakeup/close/rejoin/save is pending. Do not suppress exceptions broadly in
SVC or close early to hide the failure.
