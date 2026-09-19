# Turnkeep v0.3 — matched screen-enclosure repair

Replace the complete screen module's printed parts together. Reuse the case
body, magnetic rolling lid, board and battery. Do not mix in old plungers,
the old retainer or the separate old bezel. This is a CAD-checked fit revision;
the board still needs the physical seating and adjustment checks below.

## 1. What to print

**Easiest: print `REPAIR_PRINT_PLATE.stl` once.** It contains all eight pieces,
already oriented and spaced for the A1 Mini. Import at 100% in millimetres;
keep all pieces on the bed. It is intentionally eight disconnected objects.
Alternatively, use the individual files in `stl/`:

| Part | Copies |
|---|---:|
| 03_faceplate_v03 | 1 |
| 04_adjustable_retainer_v03 | 1 |
| 05_front_button_PRINT_2 | 2 |
| 06_insulating_support_PRINT_4 | 4 |

Use PLA or PLA Matte, 0.4 mm nozzle, **0.16 mm layers**, four walls and 25%
infill, normal speed, no supports. Small buttons/support tips should resolve
as solid in the slicer. Avoid draft layers for this repair. The faceplate is
front-face down; the carrier is flat-back down; both types of pin have their
wide base down. Deburr first-layer edges and keep all sliding surfaces clean.

The module's flat front is now 1.2 mm higher, occupying the old bezel's height.
The separate bezel is eliminated. The glass remains recessed, not perfectly
flush. Keep this module unwrapped until it passes the functional fit test.
Old leather patterns for the faceplate need adapting to the new opening and
button positions. Body/lid patterns are unaffected.

`stl/` contains the unchanged retained body and rolling-lid
STLs if a complete reprint is wanted. They are not needed for this repair.

## 2. Hardware and hole identification

The brass "hot melt nuts" in your kit are heat-set inserts. Use the **M2,
4 mm long, 3.2 mm outside-diameter** size for the eight new insert locations.
Install them straight and flush, with the electronics and plastic pins absent.

| Use | Hardware |
|---|---|
| Four inner faceplate mounting posts | 4 M2 heat-set inserts, 4 mm long |
| Four rear carrier adjustment bosses | 4 M2 heat-set inserts, 4 mm long |
| Carrier to faceplate, positions M1–M4 | 4 M2 x 10 mm screws |
| Rear support adjustment, J1–J4 | 4 M2 x 8 mm screws |
| Faceplate to existing case | Reuse 4 M2 x 8 mm screws and existing case inserts |

See `hardware-map.png`. The carrier's mounting holes are plain clearance
holes. Only its four J holes get brass inserts, installed from the flat BACK.
The plastic support pins enter the wider guide bores from the PCB-facing side.

**M screws seat normally. J screws do NOT seat against the carrier:** their
heads intentionally stand away from its back. Tightening a J screw all the
way could bend the board. Each J screw pushes an insulating plastic tip;
no metal screw should touch the PCB.

## 3. Assembly and physical acceptance check

1. Fit-test the empty faceplate first. Remove the old bezel, all old plungers
   and the old carrier. Rest the new faceplate front-down on a soft clean cloth.
   Lower the board vertically into it. The display FRAME should sit level on
   both short-end shoulders. The glass, native buttons, USB connector and reset
   switch must not jam. There are no edge-gripping grooves and no snap-in step.
   If it rocks or needs force, stop here; do not use screws to flatten it.
2. Lift out the board and insert the two new FRONT buttons from inside,
   narrow end first. The wide feet stay inside. Check they slide freely and
   their stems emerge through the front. Refit the board; it must still sit
   level. Do not proceed if the button end rises.
3. Before attaching the carrier, install its four support pins, wide body
   into each guide and narrow tip toward the PCB. They are loose until captured.
   A small strip of thin removable tape across each tip can hold it in place
   during assembly; keep it flat if it remains over the tip. Do not use felt
   or foam as the support. Start the J screws from the back only far enough
   to engage the inserts, leaving all pins fully retracted. Confirm each pin
   can slide in its guide before assembly; do not force a tight printed pin.
4. Lower the carrier straight onto the four faceplate posts, with its USB-end
   arm matching the board's USB end. Fit M1–M4 using M2 x 10 screws and tighten
   gently to the mounting shoulders. The board may have play at this stage;
   the carrier must not bow and its pins must not lift the board.
5. Keep the frame seated in the faceplate. Advance J1–J4 a little at a time,
   stopping each at FIRST light contact with the PCB. Use the diagram to see
   their contact locations. Tips must meet bare PCB, not a chip, connector or
   reset switch. Do not add a tightening turn after contact. The four settings
   may differ. This takes up actual assembly height, including any thin tape.
6. Turn the module over. Both front buttons must click and release independently
   without moving the PCB or flexing either printed plate. Check screen seating
   and reset clearance again. Test powered operation before fitting the module
   to the case. If a press moves the board, inspect the corresponding support;
   if the frame starts lifting or bowing, back the adjustment off.
7. Route the battery lead through the open centre, clear of the adjustment
   screws. Fit the module using the existing case screws. Recheck both buttons,
   reset access and USB cable access with the module installed. Repeat the
   check after the first session of use; PLA creep and loose screws were not
   evaluated by the CAD checks.

## 4. What was checked, and what remains physical

The downloadable package includes `validation.json`, the parts-only STEP
assembly, source and previews. With the optional external board model, automated checks cover valid connected solids,
watertight exported meshes, A1 Mini print bounds, nominal board/component
clearance, bare-PCB contact area under every support tip, front FRAME seating,
button insertion and plastic travel envelopes, pin adjustment, screw envelopes,
board approach, small XY positioning offsets, battery clearance, existing body,
closed lid and docked rolling tray. Print orientation was visually reviewed.

The glass is not the intended clamp surface. The native-switch pockets now
have about 1.68 mm nominal height clearance outside the plunger foot, versus
0.08 mm in the old rectangular recess. The plunger stems and native switches
share centre lines. The carrier has no board-gripping rails.

These checks do **not** certify your printer's dimensional accuracy, exact
board revision, switch force/travel, structural load performance or a successful
physical fit. No FEA or physical button-load test has been performed. The
adjustable supports accommodate height variation but cannot compensate for a
board that fails steps 1–2. Do those checks before wrapping or final assembly.

Reference: LILYGO's public `T-Display-S3/dimensions/t-display-s3-full.stp`.
Its SHA256 is D33DC105AA1E25A1408F3EFFDA5A59319EF69044984D57CCB6D482BB78A589A5.
The board model is not redistributed here; its reuse license has not been established. This repair
changes how that geometry is located and supported; it does not claim a newer
or independently measured board model.
