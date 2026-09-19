# Bring the game back to the table

## Thesis

Keep tabletop games tactile and social: real dice, paper notes, a miniature and the people around the table. Turnkeep adds a small offline companion for health, resources and reference details. The phone handles preparation; it goes away when play starts.

**Make it. Modify it. Copy it. Reinvent it. Make it yours.** Original files, firmware and guides stay free forever under their open licenses. The website follows the evolving design. Fully assembled boxes from Trevor are planned; optional coffee support does not buy or reserve a box.

## Intended session

Boot the box, scan its QR code, pair the phone and choose a character. Bring in a D&D Beyond character link or a photograph of a paper sheet, review the extracted fields, then transfer the character to Turnkeep. Play offline with physical dice and paper. After the session, review/export changed counters. When the character levels up, review an updated sheet and replace the device pack deliberately.

Review is essential: character import must identify missing/uncertain fields rather than invent rules or silently overwrite session state. Future sync must resolve character identity and counter conflicts explicitly. Uploading to Turnkeep and writing back to D&D Beyond are separate capabilities; the latter depends on supported service access and is not promised.

## Current prototype boundary

Current firmware uses validated compile-time CharacterPack JSON. `site/setup.html` demonstrates the intended phone journey using synthetic data. A pasted Beyond URL is format-checked but never fetched. A selected photo is previewed locally but never uploaded or recognized; sample extraction remains explicitly synthetic. The review can download a valid CharacterPack JSON; device pairing/transfer and after-game sync are simulations. Nothing is saved to a server.

The QR in this website opens the public demo. It is not a pairing credential. Real pairing, transport, authentication, firmware upload handling, image recognition, approved character-source access and conflict handling remain implementation work. Choose and validate the device transport before promising a phone/browser compatibility matrix.

## Hardware direction

Current build: T-Display-S3 case with v0.2 body/lid and v0.3 module. Future studies: larger CYD color display a colour e-paper display and a round-screen edition. `hardware/concepts/` contains shared parametric envelope studies, not fit-tested printable parts. Panel selection, controls, refresh behavior, power and mechanical compatibility must be verified before either becomes a build release.

The broader direction is an evolving storage system for nerds: for when your hobby is building things for your hobbies. The [roadmap](roadmap.md) records the ordered milestones.
