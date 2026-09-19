# Turnkeep development roadmap

An evolving storage system for nerds, for when your hobby is building things for your hobbies.

## Starting point

A real T-Display-S3 prototype and open CAD exist today. Character JSON can be validated and compiled into firmware. The website phone flow is an interactive demonstration; Beyond import, photo recognition and wireless device transfer are not implemented. The current v0.3 mechanical fit, power behavior and recovery still need physical acceptance. The sequence below is intentional; dates come after each milestone is scoped and validated.

## Development sequence

| Order | Work | First useful delivery | Acceptance before moving on |
| --- | --- | --- | --- |
| 1 | Character import | A reviewed CharacterPack from a supported character source, with an explicit path to the current device. Start with the validated JSON format, then a supported D&D Beyond export/link route and sheet-photo extraction. | Missing/uncertain fields are visible; user approves the pack; identity and counter replacement are deliberate; invalid input cannot destroy the working character. Verify real source access before promising direct service integration. |
| 2 | CYD edition | A larger color-display version using a selected CYD board, with a new enclosure, controls and shared character format. | Measure the exact PCB/panel, validate power and controls, print and physically fit the enclosure, run a full offline session. The current concept CAD is only an envelope study. |
| 3 | Phone companion | Boot-and-scan pairing, review/import, transfer to the case, after-game export and level-up replacement. Start with a mobile web companion; evaluate a native app only where device transport requires it. | Real pairing and transfer work on named phone/browser combinations; interrupted sync preserves the last good pack; conflicts are explained; play remains offline. OCR and service connections clearly expose errors and uncertain fields. |
| 4 | Made-to-order builds | Fully assembled, tested boxes from Trevor with a clear bill of materials, price, lead time and supported firmware. | Physical-fit/power/recovery checks pass, packing and repair guidance exist, a repeatable bench acceptance checklist is used, and order terms are published before payment is taken. Coffee support is not a preorder. |
| 5 | Colour e-paper edition | A selected colour e-paper panel and a low-distraction character view in a revised case. | Measure refresh latency, readability, ghosting and power; choose which fields refresh together; verify controls, enclosure and a tabletop session. No battery-life promise from the mockup. |
| 6 | Round-display edition | A compact round-screen take on the same offline companion and modular storage idea. | Design a genuinely readable circular interface; verify display/controller selection, controls, power and physical fit; retain the shared import format where practical. |

## What stays constant

Real dice, paper and people stay central. Phone use belongs before and after play. Working files, firmware and build guides stay free forever. Versions must distinguish tested parts from visual concepts and current features from demonstrations. The website updates as the project evolves; new ideas do not silently become supported hardware.

**Make it. Modify it. Copy it. Reinvent it. Make it yours.**
