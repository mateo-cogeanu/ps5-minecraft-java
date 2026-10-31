# Local171: readable pairing, instance actions and voice-thread crash repair

01.000.171 is a local physical-console candidate, not a release or Store update.
It retains170's opt-in system microphone and launcher-style browser manager with
exact launcher icons. Original game/mod JARs and saved data remain unchanged.

Pair with six random letters/digits displayed as A2E-34E. At least one letter and
one digit appear in random positions; ambiguous I/O/0/1 are excluded. Lowercase
and an omitted hyphen are accepted; browser typing inserts the hyphen. Codes
rotate after five minutes. Existing five-attempt/minute limit and separate128bit,
15-minute browser sessions remain. Closing management revokes access.

The displayed and QR addresses now both use `/i`, which redirects to the instance
selected when management opened. Stable UUID routes remain; All instances leads
to the home cards. Host/port still come from actual network discovery/binding.
No shortener service or fixed device address is introduced.

Microphone and Browser uploads occupy the right action grid above the final row.
**Done and Delete instance always occupy the bottom row.** Controller navigation
and pointer hitboxes match that order. Opening Microphone invalidates cached UI
state immediately. The pairing screen has one footer and suppresses stale status
and progress overlays, including while the frontend is idle.

## World-start crash evidence and limited repair

Cyberduck retrieved today's native crash reports and mounted app diagnostics.
The170 SIGABRT on VoicechatSoundManag resolves through the matching retained ELF
(load adjustment0x400000) to Ps5Backend::open → __cxa_throw → __cxa_get_globals →
abort_message. Matching stderr reports AudioOut0x8026000d, followed by
`std::__libcpp_tls_set failure in __cxa_get_globals()`.
The underlying AudioOut refusal remains unresolved. This is playback failure,
not proof that microphone capture caused the abort.

The runtime-owned java25_cxxabi_tls.py now builds pinned genuine LLVM18.1.8
exception-global storage using its existing HAS_THREAD_LOCAL branch. This consumer
explicitly links it before SDK libc++abi archives. Existing compiler TLS adapter
allocation/retirement policy remains. SDK/default runtime behavior is unchanged.
See the runtime repository's docs/java25-cxxabi-compiler-tls.md.
No unavailable audio device is reported as working, and voice-chat playback is
not claimed to work. Exact PS5 crash recovery remains pending.

## Validation and physical checks

Fast15/native6 lanes pass. Native production UI renders through host Vulkan
lavapipe with Done/Delete at the bottom; stale-footer fixtures match clean pixels.
Firefox confirms `/i` redirects to the selected instance and typing raw lowercase
six characters formats and pairs successfully. Host fixtures are synthetic data.
Runtime tests exercise genuine exception globals across800 worker lifecycles,
concurrent isolation, nested throw/rethrow with native key setters refusing;
existing actual compiler-TLS adapter failure/lifecycle regression passes.
Target object audit confirms compiler TLS and no native key-set dependency.

Physical171 checks still required: six-character pairing and reachability; readable
footer; all instance actions with controller and mouse; microphone speech/mute and
route behavior; original voice-chat mod world entry/error recovery; save/reload and
endurance. No console pass or release approval is implied. Raw reports stay local.
