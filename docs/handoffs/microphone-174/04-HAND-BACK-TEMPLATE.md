# Microphone repair handback to Codex

Template only: no work or console pass is implied. Claude should fill a separate
HAND-BACK.md here after completing the bounded microphone assignment.

## Result and scope

- Microphone behavior before/after:
- Demonstrated root cause and supporting evidence (sanitized):
- Remaining hypotheses, with confidence and contrary evidence:
- Scope stayed within microphone capture; any exception and its authorization:

## Exact source and integration

- Consumer branch, base/head commits and changed files:
- Published commit(s), remote and verified push status:
- Runtime/output dependency needed from Codex, if any (do not implement silently):
- Unrelated user edits preserved:

## Verification

| Check | Exact command/build | Result | Evidence location / limitation |
| --- | --- | --- | --- |
| Focused capture/core | | | |
| Genuine OpenAL backend | | | |
| Required host lanes | | | |
| Target/package/relink if built | | | |
| Physical launcher input | | | |
| Physical SVC samples/speech | | | |
| Hardware mute and route | | | |
| Capture stop/reopen | | | |

Report failures/timeouts/retries and skipped checks explicitly. Distinguish host
OS doubles from real PS5 reads, speech correlation and remote two-way voice.

## Local artifact, if one was built

- Exact version/title:
- Package path and SHA-256:
- Matching symbols path and SHA-256:
- Corresponding-source/relink ZIP path and SHA-256:
- What physical tests actually used this exact package:
- No release/prerelease/Store action performed:

## Remaining work for Codex

- Blocking finding that requires the primary maintainer:
- Smallest next physical microphone test:
- Known risks or unverified ownership/lifecycle behavior:
- Generated local files worth retaining; no cleanup/deletion performed:

Keep raw logs, private paths from game reports, device/account identifiers and
credentials out of public documentation. Reference local evidence by safe label
and leave sensitive artifacts local.
