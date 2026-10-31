# Local172: terrain worker stack budget

01.000.172 is a local console candidate, not a release or Store update. It keeps
171's launcher/pairing improvements, opt-in microphone and exception TLS repair.
Original game/mod JARs, shader packs, worlds and account data remain unchanged.

## Observed171 failure

User reports freezing on Loading terrain. Cyberduck retrieved the current Java
console and historical startup log; several other live files were denied by FTP.
Safari's read-only HTTP diagnostics subsequently retrieved the complete native
trace. The current console shows Fabric26.3, Sodium and Vitrail loading
ComplementaryUnbound_r5.9.3. Vitrail compile worker4 and Sodium Async Cull Thread
fail native creation with EAGAIN; the game reports unable to create native thread,
then emergency save/server shutdown. Heap-use samples do not establish heap
exhaustion. The previous170 C++ exception-key abort does not recur in this report;
voice playback/microphone remain unavailable in this session.

Accounting complete successful native owned-stack create and successful joined
reap records shows32MiB live stacks and eight further failed native creation
attempts while that boundary is held, before emergency recovery. All use1MiB
stacks. This reaches the existing fallback budget. The original native allocator
refusal is still unresolved. Live file status at retrieval showed192 descriptors,
soft/hard12555; there is no evidence of EMFILE in this run. Raw reports stay local.

## Repair and limits

Runtime patch0134 adds bootstrap-only explicit64MiB owned-stack budget. Default
runtime remains32MiB/64slots.172 enables64MiB before JVM initialization and fails
clearly if policy is rejected. Requested stack sizes, native create/quotas,
shared256MiB native-data ledger, guards, joined retirement and conservative failed
cleanup ownership remain. No heap/GC/processor-count or shader-setting change.
Runtime patch0133 also corrects the fallback gate to use returned EAGAIN without
requiring errno ENOMEM and adds capacity-refusal diagnostics. That is a separate
correctness fix:171's trace had errno ENOMEM and used its old fallback.
Runtime sources stay in the Java repository; consumer integration pin records the
patch/test and actual os_bsd.o hashes. Final package audit requires that object,
setter export and embedded policy/refusal messages. Corresponding source/relink
materials preserve the exact pin and relevant runtime implementation/patches.

## Checks and console plan

Fast15/native6 host lanes pass. Actual runtime owner ASan/UBSan tests cover default
32 live1MiB stacks rejecting another, extended33 concurrent real1MiB custom-stack
threads, complete joined reclamation,220 lifecycles, TLS-destructor ordering,
create/map/attribute/join/release faults, slots/budget and late policy rejection.
The actual create-thread gate matrix covers stale/zero errno and other errors or
thread kinds. Target HotSpot rebuild passes. These do not establish a PS5 pass.

On exact172, load the same instance/world with the same shaders; verify terrain
appears, movement works, then save/reload and repeat world transitions. Compare
shaders disabled if still stuck. Check mic speech/mute and voice playback
separately; no microphone success is inferred. Retain native trace, Java console,
resource samples and exact build/run identity if any failure persists.172's
64MiB budget remains bounded; more native failures remain possible. No release,
prerelease, Store update or historical snapshot replacement is authorized here.
