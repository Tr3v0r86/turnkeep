# First experimental release

## Matched components

Version `0.1.0-prototype.1`: retained v0.2 body and lid, v0.3 adjustable screen module, two-piece wrap nets and standalone character firmware. Use the complete module: one faceplate, one carrier, two buttons and four insulating supports. The repair plate contains those eight pieces. Full-build instructions and fastener quantities are in `hardware/README.md`.

## Evidence

CAD default build passed 114 geometric checks. A separate nominal manufacturer-board reference run passed 163 checks; that reference is excluded from the distribution. Saved deliverables verified as 10 valid original STEP solids, 11 watertight STL designs with matching hashes, an eight-piece repair plate and a two-page A4 landscape wrap PDF.

ESP-IDF 5.4 clean build passed: application size 666,768 bytes, 79% partition free. Nine CharacterPack test methods and strict C11 character/button tests passed. The desktop simulator built and rendered the included synthetic character. No physical device was flashed during public extraction.

Website checks: local fonts/images loaded, no overflow at 320, 390, 768 and 1440 pixels, valid in-page anchors, first-focus skip link and working native assembly disclosure. Actual-geometry hero and module images were visually inspected on desktop/mobile. Full assistive-technology testing is not claimed.

## Known limits

Physical v0.3 fit acceptance, upholstery trial, battery runtime/current, thermal behavior and repeated recovery tests remain open. No battery model/capacity or final cost is asserted. Character loading requires rebuilding firmware; there is no phone uploader. The public character is synthetic. No ready-to-flash image is distributed.

## Provenance and reuse

The public repository starts with fresh history and original sources only. Manufacturer CAD, third-party miniatures and derived imagery are excluded. Runtime/toolchain dependencies are acquired from their upstream sources. The source ZIP contains only `RELEASE_FILES.txt` entries; `manifest.json` identifies the exact source commit, byte sizes and SHA-256 checksums. The GitHub prerelease assets are the versioned snapshot; later main-branch website builds do not replace those assets.
