# Operations, verification and continuation plan

## Canonical local layout

The current Mac workspace is:

```text
/Users/mateocogeanu/Documents/ChatGPT/PS5_Homebrew
  java25/patches/                 runtime patches (publishable)
  tools/                         runtime tools (publishable)
  .tools/java25/openjdk/          cached applied JDK source/build (not Git)
  .tools/github-publish/ps5-java25-homebrew/  runtime publication checkout
  .tools/github-publish/ps5-minecraft-java/   active consumer checkout
```

Use these as local navigation facts, not machine paths to hardcode into new
portable tooling. On another machine use the repositories' source-organization
and build guides. The snapshot ZIP alone is not sufficient for a target build.
Read both current AGENTS.md files and docs/maintainer-constraints.md first.

## Scope override for Claude

These operations document the shared project environment. Claude is assigned only
the microphone/SVC capture repair. The primary maintainer retains ownership of
socket0135/title-screen fixes, runtime policy, output playback, controllers/UI/web,
release decisions and cleanup. Read-only inspection of related context is fine;
do not edit or validate those areas as a separate workstream. Request a narrowly
specified dependency through the handback if a microphone repair needs changes
outside its boundary. Do not delete builds or expand this into maintenance.

## Suggested next sequence

1. Inspect current Git state; preserve unrelated edits. Identify the actual open
   console build/run and ask whether174 was installed if it cannot be established.
   No174 console outcome was supplied at handoff time.
2. Retrieve the current and previous run reports before another restart rotates
   them. Match build and run IDs. Keep private raw reports local.
3. On174 with voice capture ON, check SVC enumeration/open/start separately from
   the launcher probe. Look for `PS5 AudioIn stage=... result=0x...`, enumeration
   stage failures, OpenAL ALC errors and Java microphone fallback. If worker
   creation is next to fail, capture its actual pthread return; do not infer
   allocation quotas solely from aggregate free memory.
4. Check microphone stop/reopen across world/server transitions as part of the
   capture lifecycle. If the separate socket/title-screen issue blocks this,
   preserve the evidence and hand it back to the primary maintainer.
5. Repair demonstrated microphone issues within the assignment boundary; add focused actual-code
   lifecycle/failure tests. Review join/close uncertainty before widening capture
   concurrency or adding a reserve. Avoid speculative broad allocator changes.
6. Build the next distinct local version if source/native inputs change; do not
   silently replace174 and transfer its validation. Publish source with pending
   console tests explicit. Deliver the exact package/checksum and concise tests.

## Host and target commands

From the consumer root, with a complete JDK25 and the required pinned host libs:

```sh
python3 tools/run_host_tests.py --java-home /path/to/jdk25 --fetch-deps
python3 tools/run_host_tests.py --lane native --java-home /path/to/jdk25
python3 tools/test_microphone_capture.py
python3 tools/test_openal_ps5_audio.py
python3 tools/test_client_startup_host.py --java-home /path/to/jdk25
python3 tools/check_documentation.py
git diff --check
```

The fast lane has15 checks, native has7 at this snapshot. The actual OpenAL test
is separate from the native lane and requires prepared pinned OpenAL/build
prerequisites; it exercises rendering/backend integration with host OS doubles,
not host speech. Use `docs/host-test-inventory.md` for prerequisites and boundaries.
Python3.10+ without optimization; Clang ASan/UBSan, native host arch, JDK25.
Portable tools accept JAVA_HOME/CC/CXX/LLVM_HOME/PATH/HOMEBREW_PREFIX or explicit
options. Do not silently use game-cache JARs as host dependencies. For changes to
portable lanes, verify source-only/outside-root invocation with empty dep cache.
Do not broaden/repeat checks without a new change or unresolved concern.

Runtime socket regression, from runtime root:

```sh
python3 tools/test_java25_socket_preclose.py --java-home /path/to/jdk25
```

A target native-runtime change requires rebuilding the affected target libraries
in the prepared JDK cache, with the SDK LLVM toolchain configured. For0135 the
cached build was incrementally rebuilt with java.base-java and java.base-libs.
On this Mac the SDK wrapper requires LLVM_CONFIG; without it the wrapper attempted
`/llvm-clang` and failed. Setting it to the installed LLVM llvm-config repaired
the invocation. Do not mistake that host setup error for a source compile failure.
The general java25_source.py bootstrap has a previously recorded old patch replay
conflict at0009; do not assume a pristine-source replay passed from an incremental
build. Verify newly added patch apply/reverse-check and report scope honestly.

Full prepared app build, from consumer root:

```sh
python3 tools/package_client_vulkan.py \
  --runtime-workspace /path/to/PS5_Homebrew \
  --java-home /path/to/jdk25 \
  --startup --launcher-channel temp_gametest
python3 tools/test_client_startup_package.py \
  --runtime-workspace /path/to/PS5_Homebrew
```

Version identity: `client/startup/param.json`, via `tools/app_identity.py`.
Change full01.000.174 tokens deliberately; never blanket-replace bare digits.
Root sce_sys/param.json represents the published client, not every local test.
Current build outputs include `dist/client-startup/PPSA99874.zip`, channel copy
`dist/client-startup-temp_gametest/PPSA99874.zip`, and
`build/client-startup-probe/bootstrap-01.000.174-symbols.elf`.
The delivered174 copy is in `dist/local-tests/client174/`.

Full package audit checks runtime0135 source/object hash, linked JNI symbol and
error literal, actual system imports, matching source/header audits and preserved
historical artifacts. Its native dependency baseline is a small SHA-proven167
DT_NEEDED fixture, plus only AudioIn; it no longer requires deleted153 local
symbols. Do not recreate obsolete binaries merely to satisfy old audits.

Corresponding-source/relink kit, after a new actual build:

```sh
python3 tools/build_client_openal_relink.py \
  --runtime-workspace /path/to/PS5_Homebrew \
  --output /path/to/new-local-candidate --variant startup \
  --version 01.000.NNN --tested-source FULL_CONSUMER_COMMIT
```

Within an extracted kit:

```sh
python3 relink.py --lld /path/to/ld.lld --output /tmp/relinked.elf
python3 relink.py --lld /path/to/ld.lld --objcopy /path/to/llvm-objcopy \
  --openal-object accepted-startup-openal.o --output /tmp/replacement.elf
```

Both174 paths reproduced the exact hash in START-HERE. On this Mac ld.lld is in
`/opt/homebrew/opt/lld/bin`, not `/opt/homebrew/opt/llvm/bin`. Preserve source/relink
materials/licenses for OpenAL LGPL compliance; no game JARs enter the kit.

## Log retrieval

Direct HTTP/FTP from the shell previously failed with macOS “No route to host”
while the same PS5 was reachable in Safari/Cyberduck. Do not repeatedly diagnose
that as proof the console is down. Use a relevant connected tool if sufficient;
otherwise native CUA Safari/Cyberduck worked. Use existing related diagnostic tabs
and connections and preserve unrelated user browser activity. Obtain the current
console address at runtime from its UI or authorized existing connection; no
address/client filter should be baked into source or a new inbound test app.

Read-only diagnostic HTTP service port8765 provides:

| Route | Meaning |
| --- | --- |
| /build.txt and /session.txt | Open app identity and recorded run; compare both. |
| /native-stderr.log | Current native OpenAL/audio/graphics diagnostics. |
| /exceptions.log | Current Java console/exception output for this launcher. |
| /previous-native-stderr.log and /previous-exceptions.log | Retained previous run;173 game/crash evidence was here after reopening. |
| /startup-tail.log | Last512KiB of historical startup log, may contain older fault tails. |
| /abort.txt and /previous-abort.txt | VM abort record or explicit no-abort note. |
| /hs-error.log and /previous-hs-error.log | JVM fatal reports if present. |
| /exception.txt and /previous-exception.txt | Probe exception reports if present. |
| /vm-trace.log, /resource-status.txt, /file-activity.txt | Runtime/resource diagnostics when accessible. |

The service root lists other allowed report routes. A response isn't necessarily
current: match run IDs. `server.log` is not a substitute for this launcher's Java
exceptions stream. Historical SHADERC fault text in startup history is not proof
of the latest crash. On173 the current reopened run contained only launcher/auth
startup; the previous Java console held the actual game crash report.

Safari Save As (Cmd+Shift+S) lets you save full text; AX snippets truncate it.
Use a fresh unique filename and setValue on the whole Save As field; typeText
previously appended to the selected basename. If asked to append .txt, choose
Don't Append when keeping the intended suffix. Avoid overwriting old retrievals.
Read saved logical bytes with shell tools, and keep raw reports out of Git.

Cyberduck existing FTP connection uses port2121. Relevant remote folders are
`/mnt/sandbox/PPSA99874_000/download0` and `/data/devtools/crashes`.
Files include probe-native-stderr.log, java25-exceptions.log, java25-startup.log,
java25-vm-trace.log, probe-session.txt and mc-bootstrap-abort.txt with previous
variants. Some live FTP reads returned550; HTTP worked. A genuine process crash
may also need the native crash JSON, matched to exact symbols.

Local private evidence, not in the public ZIP or Git:
`/Users/mateocogeanu/Downloads/prospero173-previous-native.log`,
`prospero173-previous-java.log`, `prospero173-startup-tail.log`, plus the reopened
current files `prospero173-mic-exit.log` and `prospero173-exceptions.log` in the
same Downloads folder. Do not upload them to the other model automatically.
The user may deliberately supply selected sanitized evidence if needed.

## Acceptance matrix: report each layer separately

- Launcher input: intentional quiet/speech test; confirm correlation, pacing and
  sane magnitudes. Muted/unmuted compare and silent-block counts. No recording.
- Route: system-select DualSense explicitly; distinguish headset/Remote Play and
  physical hardware mute. No forced gain/unmute/routing change.
- SVC: enable saved Voice capture; Java microphone option OFF for native OpenAL;
  exact mod version/config identified, enumeration/name selection, open/start,
  samples and codec initialization. Do not silently auto-configure user files.
- Transport: matching server voice mod, reachable UDP and consenting participant;
  test remote hears PS5 and PS5 hears remote with game audio simultaneously.
- Lifecycle: repeated start/stop, world/server disconnect/title/rejoin, shutdown
  while voice receive is active, no pinned lease/resource creep or hangs.
- Regression: saves/reload, terrain/shaders, playback, input, rumble/gyro/triggers,
  controlled-duration endurance and resource counts. Avoid calling one successful
  menu/host/aggregate test “ALL PASS”.

## Publication, releases and disk discipline

Consumer source/docs/tests: commit and push ProsperoCraft main without overwriting
remote work. Runtime reusable patches/tools/docs: from the runtime workspace run
`python3 tools/sync_github.py --message "Describe the completed changes"`.
Verify push success for each. Publish work in progress with explicit pending
console tests; console failure does not block authorized source publication.
No force push. No credentials/logs/cache/game JARs/build outputs in source commits.

Claude must not create releases, prereleases or Store actions. Those remain
with the primary maintainer, subject to exact physical-build acceptance and
applicable user authorization. Preserve original JARs and frozen history;
do not add bytecode hooks, game replacements or platform spoofing. Fix runtime
compatibility in Java, consumer integration/policy/UI in ProsperoCraft. The
history archive repository supersedes recreating retired milestone source trees.

A previous user-directed cleanup recovered tens of GiB. Keep accepted167,
latest173/174 recovery material and frozen/archive assets. Do not delete unproven
sources/compatibility code or release assets to save space. Expanded relink kits
can be discarded only after a retained ZIP's CRC and every file hash match.
Some retained ELF/log files use macOS transparent compression: clearing
UF_COMPRESSED during metadata copying can make logical data appear empty.
Read and verify final logical bytes; do not blindly copy flags from an
uncompressed target onto a compressed clone. See the canonical storage guide.
