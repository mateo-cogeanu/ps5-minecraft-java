# Local build storage cleanup —2026-10-31

The user explicitly requested removal of obsolete local test builds to recover
disk space.103 old candidate/local-test/withdrawn hardware-test directories were
removed. Historical notes saying those local binaries are retained describe the
state at the time of those tests; they do not establish current local availability.
Existing validation records remain preserved, rather than rewritten.

Retained working copies are local candidate173 (package, matching symbols,
corresponding OpenAL source/relink ZIP, checksums and validation) and accepted
launcher167 as recovery. Frozen passing snapshots in accepted/milestone/history
storage, original GitHub releases/assets/tags and project source were preserved.
No release, Store change, game/mod transformation or runtime behavior change was
made. Worlds, account data and key material were preserved; small local build
records/key files from discarded folders remain under ignored cleanup-records.

Duplicate baseline files were removed only after comparing full SHA-256 with the
canonical workspace copy. Extracted relink kits were removed only after every
file matched a retained ZIP with valid CRC. Original ZIPs remain; extract the
corresponding source/relink ZIP into a temporary folder when needed. Old generated
test/preview/relink-check caches were discarded and may need regeneration before
running optional prepared-build audits. No full app rebuild was run for cleanup.

macOS transparent compression and independent copy-on-write clones compacted
retained symbols and old logs while preserving logical bytes and filenames.
Final checks compare affected retained files with their original SHA-256 values.
Metadata operations on compressed clones must preserve the compression flag;
clearing that flag without decompression can hide the data fork. The cleanup
caught/restored this flag on affected clones and verified their original hashes.
Always verify final bytes after all metadata operations, not only before them.

The detailed filesystem journal remains local and ignored. No raw logs, personal
files, dependency caches, private keys or game assets were uploaded. This was a
one-time cleanup, not an automatic retention job. Future builds can recreate
caches; avoid copying entire build/dist trees for test baselines when a source-only
checkout and protected-input hashes suffice.
