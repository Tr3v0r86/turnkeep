# Two connected upholstery nets

Exactly two connected material pieces: lid outside top with connected side flaps, and base underside with connected side flaps. The screen surround remains painted.

## Print and fit

Print `output/pdf/Turnkeep-TWO-PIECE-wrap-nets-A4.pdf` on A4 landscape at 100% actual size. One complete net fits on each page; no tiling or joining. Verify each 100 mm line. Solid outlines are cuts; blue dashed lines are gentle folds, never cuts or scores. Grey dotted lines locate the corner wings, which remain attached. The base net is mirrored to show the underside from outside; its USB opening appears on the right.

Cut and tape the paper first. Keep the printed face outward when trial-fitting. Put that printed face DOWN when tracing onto the PVC backing. Test the bend with a scrap of actual 1 mm PVC before cutting the final material. Glue the broad face first, fold the flaps around the edges, then curve the attached wings around the vertical corners. Butt and trim the joins; do not force overlaps. Small compound-corner reliefs and the screen surround remain painted. No material turns into the case or covers the inner L-notch walls.

## Geometry and limitations

Derived from the retained v0.2 body/lid (166 x 110 mm; base lower wall 20 mm; lid 16 mm; fixed screen corner 31.6 mm). The current v0.3 deck remains exposed and removable. R5 corner wings use the neutral radius of nominal 1 mm material (5.5 mm). Flaps include a provisional 0.5 mm bend/trim allowance with 0.3 mm seam setback; this is a trial pattern, not a measured material-specific bend model. Body upper corner flaps stop at the original straight-wall transitions. Docking collars, sockets, finger recess and USB have clearance cutouts.

Shapely verifies each finished cut outline is one valid connected polygon after all reliefs and openings; exactly two material pieces. The PDF is two actual-size A4 landscape pages, rendered and visually inspected. Physical PVC fit and glue behavior remain unverified. Do not infer tested upholstery fit from the geometry checks.

## Reproduction

Run `python make_patterns.py` with Shapely, ReportLab and pypdf installed. Outputs live under `output/pdf/`; `validation.json` records dimensions and connectivity.
