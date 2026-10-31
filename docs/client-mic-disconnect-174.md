# Local174: microphone open diagnostics and SVC disconnect

01.000.174 is a local test candidate; no release, prerelease or Store update.
Original game/mod JARs, settings and worlds remain intact. It retains173's two
independent output ports, OpenAL error storage and terrain/runtime policies.

## Console evidence from173

The retained173 run reaches a world with Simple Voice Chat2.6.27 on Fabric26.3.
OpenAL opens a real second playback device and its context; the microphone request
is now the expected48kHz mono Float32. Both actual capture opens fail before a
capture worker is created. The Java sound fallback has no PS5 capture provider.
The old log omits the native AudioIn stage/code, so neither the failing call nor
its underlying reason is established. The launcher five-second input test can
report the error code; speech/mute/route/two-way voice remain unvalidated.

Returning to title invokes SVC's DatagramSocket.close. It throws java.lang.Error
wrapping IOException "Operation not permitted" from UnixDispatcher.preClose0.
The controller only invokes the pause-menu action. This run writes a Minecraft
crash report; it has no recorded VM abort. Raw reports and instance paths remain
local.

## Changes and ownership

Reusable runtime patch0135 uses shutdown(SHUT_RDWR) for PS5 socket pre-close,
retrying EINTR and accepting ENOTCONN. Existing Java closing state/signals and
reader/writer ownership still perform deferred final close. It never closes and
recycles an in-flight descriptor. Genuine errors propagate; non-sockets and
other platforms retain their existing dup2/error behavior. The implementation,
actual JNI test and validation are in the Java repository. The consumer pin
requires the rebuilt UnixDispatcher object and the linked native entry point.

The single capture owner now remembers successful process-lifetime AudioIn
initialization instead of calling init again for every enumeration/test/open.
Failed initialization remains retryable; no negative code is treated as success.
This corrects lifecycle behavior but is not proof of the observed failure's cause.
OpenAL and the launcher test report a fixed stage name plus the actual hex code.
No audio samples, account data or user identifiers are added to diagnostics.
The existing native format, default-off setting, exclusive lease, mute checks,
resampling and cleanup ownership remain. No guessed alternate format is tried.

## Verification and exact-build checks

The runtime test registers the actual JNI in JDK25 and repeats blocked UDP receive,
TCP read and accept shutdown40times each, with bounded joins and error/descriptor
controls under XcheckJNI/UBSan. The target java.base libraries rebuild. Host
kernels do not establish PS5 shutdown wakeup behavior.

The capture host fixture verifies failed-init retry and one confirmed init across
failed/successful opens and200restarts, alongside existing bounds/mute/route/
conversion/concurrency controls. Host fixtures do not establish microphone speech.
Build/package/relink results and checksums are recorded after local verification.

On174, test the microphone from Instance Settings → Microphone, then enable voice
capture and try SVC. Record the exact stage/code on failure. On success establish
speech correlation, physical mute and route distinction with a consenting remote
participant. Return to title while voice receive is active, re-enter the same
world repeatedly, check saved state and repeat server joins. Also check game and
voice output, controller/haptics and sustained terrain loading. Exact174 console
acceptance is pending; no successful voice or world-exit result is claimed.
