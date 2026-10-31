# Local173: independent voice speaker and context-error storage

01.000.173 is a local console candidate. No release, prerelease or Store update.
It retains172's explicitly enabled64MiB owned Java/compiler stack budget and171's
pairing/layout/microphone work. Original game/mod JARs and saved data stay intact.

## Observed172 audio failure

Safari's live session identifies172; its current native stderr shows the game
adopting the real pre-JVM AudioOut port, creating a context and submitting PCM.
Simple Voice Chat's later two default playback opens fail with0x8026000d.
The mod displays Speaker unavailable / operating system might not be supported.
This is a specific failed device initialization, not evidence that all PS5 audio
is unsupported or a missing voice-server connection.

A later real OpenAL loopback device opens but context construction throws
al::tss::tss(T). Upstream's per-context mLastThreadError constructor sets a native
pthread key; refusal throws that exception. The mod cannot determine the OpenAL
version and requests stereo Float32 microphone capture, which our mono-only
capture profile rejects. Java fallback cannot load jsound. Those are separate
capture limitations; no successful microphone speech/mute/two-way voice is claimed.
Raw reports and session data remain local.

## Native changes

Startup now attempts two real SYSTEM/main256frame48kHz stereoS16 output ports
before JVM/native memory pressure. Failure of the second reservation remains
nonfatal and is recorded. OpenAL independently adopts each port; the drained
port pool retains at most two across logical device reloads. Duplicate/invalid
handles are rejected. An additional late device still performs a real native open
and reports its actual refusal. No silent sink, device sharing fiction, modified
mod or fabricated success is introduced. Both mixer workers already existed.
Early second-port availability, simultaneous audible mixing, haptics and cleanup
remain physical-console pending.

Only OpenAL's per-context AL error member now uses compiler TLS with generation
tags instead of a native key per context. Sixteen context slots are bounded;
exhaustion fails explicitly. Each thread retains independent errors, error-read
clearing remains upstream, and a reused slot cannot inherit a destroyed context's
error. A saturated generation counter refuses rather than wrapping. The title's
existing compiler-TLS adapter supplies allocation/lifecycle; global JVM/SDK/native
keys and ordinary OpenAL context selection stay unchanged. No arbitrary thread-ID
map or global shared error value is used. This removes the observed late context
key-set path; other native allocation failures remain possible. The unchanged
mono capture path may still refuse the mod's stereo workaround if version lookup
fails for another reason.

## Verification and physical checks

Fast15/native7 lanes pass, including a portable actual-header test with1000
context generation reuses,8 concurrent isolated threads,16-context exhaustion
and reuse with key-set refusal scoped to the tested implementation (sanitizer/runtime keys remain normal), under ASan/UBSan. Two-port
ownership tests retain/reuse/limit/deduplicate handles with concurrent claims.

The real pinned OpenAL host build tests two independent playback devices plus a
loopback context across20 cycles, with host native opens forced to refuse after
the two genuine fixture reservations. Distinct PCM sources produce nonzero output
on both native port fixtures. Context errors remain isolated; both handles are
retained without extra opens or closes. Existing init/busy/refusal/disconnect,
pause/resume, resample and mono capture lifecycle cases also pass. Host OS doubles
do not prove PS5 output or capture. No host microphone is recorded.

Exact173 PS5 checks: confirm primary and secondary early-port results; enter the
same world and enable voice chat; verify speaker selection and another consenting
participant's voice alongside game audio. Check loopback/version detection and
selected mono system microphone separately, non-silent speech, hardware mute,
route changes, world/server transitions, save/reload and endurance. Preserve the
current logs if the mod still reports unavailable. No exact173 console pass is
implied. Existing earlier packages/symbols/releases remain preserved.
