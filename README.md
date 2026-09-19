# Turnkeep

**Bring the game back to the table.** A printable case for dice, a miniature and a pen, with an offline character display powered by the non-touch LilyGo T-Display-S3.

Real dice, paper notes and an offline device for the pieces that matter. **Make it. Modify it. Copy it. Reinvent it. Make it yours.**

[Explore the website](https://tr3v0r86.github.io/turnkeep/) · [Hardware and assembly](hardware/) · [Firmware and character packs](firmware/)

## Build this prototype

The first public release combines the **v0.2 body and L-shaped rolling lid** with the **v0.3 adjustable screen module**. Keep the screen-module parts together. The bare shell is nominally 166 × 110 × 36 mm; final wrapped dimensions and mass have not been measured.

- `hardware/`: original editable CAD, printable exports, wrap patterns, BOM and assembly instructions.
- `firmware/`: standalone ESP-IDF project, host simulator and validated compile-time JSON character tooling. The included character is synthetic.
- `site/`: the complete static website, self-hosted fonts and images.
- `tools/`: deterministic release packaging and checksums, using Python's standard library.

Read the hardware guide before buying parts. Print fit checks first. Fit and finish depend on your printer, inserts and covering material. Choose and verify your own compatible battery and connector polarity.

## What works, what remains

The prototype interface supports two-button navigation, health edits, actions, spells, resources and offline state. Initial sleep and wake checks passed on the prototype. Personal JSON packs validate and compile into firmware; phone-based setup is not implemented.

[Try the phone-setup prototype](https://tr3v0r86.github.io/turnkeep/setup.html): scan a QR, preview Beyond/photo import, review a synthetic character and simulate a transfer. No actual OCR, Beyond integration or device pairing is implemented. CYD, colour e-paper and round-display [CAD concepts](hardware/concepts/) are unvalidated envelope studies, separate from the current printable design.

The v0.3 mechanical fit still needs physical acceptance. Battery runtime, standby current, thermal behavior, repeated recovery cycles and a complete tabletop session are not yet validated. This is an experimental maker release, not a finished consumer product. No ready-to-flash binary is advertised.

## Build the website and download package

```sh
python3 tools/release.py
python3 -m http.server 8197 --directory site
```

Open http://localhost:8197. `RELEASE_FILES.txt` is the explicit publication allowlist. The generated ZIP and manifest go into `site/downloads/`; the manifest records sizes, SHA-256 hashes and the source revision. GitHub Pages runs the same script. See [deployment](docs/deployment.md).

## License and contributions

Original hardware: CERN-OHL-P-2.0. Original firmware and website: MIT. Original docs and images: CC BY 4.0. [Full licensing map](LICENSE.md).

Issues and pull requests are welcome. Include the component revision, board version, printer/material or toolchain, exact reproduction steps and photographs or logs where helpful. Do not upload game-book extracts, other people's character art, device dumps, secrets or unlicensed models. Public releases of Turnkeep are maintained here; unrelated prototype projects are not automatically synchronized.

The files, firmware and guides stay free forever. Fully assembled boxes from Trevor are coming soon. [Buy Trevor a coffee](https://buymeacoffee.com/padlano) is optional support, not a preorder.

[Development roadmap](docs/roadmap.md): character import → CYD → phone companion → made-to-order builds → colour e-paper → round display.
