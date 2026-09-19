# Future display envelope studies

These three original CAD mockups explore larger Turnkeep cases. They are visual concepts, **not printable release parts, compatible replacements, or verified electronics enclosures**. Use the current hardware directory for the actual prototype.

| Study | Assumed outer envelope | Assumed display bay | Visual screen panel |
|---|---|---|---|
| CYD | 198 x 132 x 42 mm | 94 x 64 mm | 72 x 48 mm |
| Colour e-paper | 224 x 150 x 40 mm | 112 x 82 mm | 90 x 62 mm |
| Round screen | 190 x 142 x 40 mm | 82 x 82 mm | 62 mm diameter |

All dimensions above are chosen design envelopes. The CYD study is inspired by a larger touchscreen; its panel is not a claim about any particular CYD active area or PCB. The colour e-paper and round-screen modules are deliberately unspecified. All three need a selected board/module, official drawings, measured mounting/connector/battery dimensions and a complete mechanical redesign before printing. No electronics, holes, switch access, screw mounts, magnets, wiring routes or working docking interface have been engineered into these studies. Their lid-open poses illustrate the intended arrangement only.

## Files and regeneration

Run `python concepts/build.py` then `python concepts/render.py` from `hardware/`, with the parent directory's pinned requirements installed. Rendering requires a graphics-capable session (VTK). All geometry is original; no vendor CAD or miniature asset is embedded.

Each study has an editable `concept.step` assembly with four solids plus five view STL meshes (`body`, `deck`, `screen_placeholder`, `lid`, `lid-open`). STL files remain in assembly coordinates; they are visual envelopes, not print-oriented components. `build.py` is the editable parameter source. `validation.json` records shape validity/watertightness; those checks do not establish fit or function.

The render script creates `media/cyd-concept.png` and `media/epaper-concept.png` and `media/round-concept.png` with explicit concept labels. The colour e-paper image uses original coloured patches and text-line shapes as illustrative screen content, not working UI; these render-only marks are not part of the enclosure CAD. The round concept uses a thin circular placeholder, with no selected display or board. It also renders `media/closed.png` and `media/exploded.png` from the unchanged current prototype meshes in `hardware/preview_meshes/`; those two images show actual current geometry with electronics omitted. Exploded offsets are visual separation only.
