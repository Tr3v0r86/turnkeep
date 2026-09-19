# Turnkeep hardware

A compact 166 × 110 × 36 mm bare case with a fixed screen corner and removable L-shaped lid. Flip the lid 180° and magnetically dock it to the front edge to make a rolling tray. Both halves rest on the table; the dock is not a carrying handle. Current geometry combines the retained v0.2 body/lid with the v0.3 faceplate and adjustable board support. This remains a prototype pending physical acceptance.

## Print and assemble

Print one each of `stl/01_body.stl` and `stl/02_rolling_lid.stl`, then `REPAIR_PRINT_PLATE.stl` once (eight electronics-module parts). Alternatively print the four module STL designs in the quantities encoded in their filenames. Units: millimetres, 100% scale. Body/lid: 0.4 mm nozzle, 0.20 mm layers, four walls, five top/bottom layers and 15–20% infill as an unverified starting profile. Inspect support preview around the horizontal docking holes. Module: 0.16 mm layers, four walls, 25% infill, no supports. Parts fit a 180 mm build envelope.

Print `08_fit_coupon` first to test insert/magnet fit, `09`/`10` to test lid clearance and `11`/`12` to test docking. Coupon rows at y=12/33/52 test 2.9–3.2 mm insert pilots, 6.1–6.4 mm large magnet pockets and 3.1–3.4 mm small magnet pockets, increasing left to right. Final pilot/pocket diameters are 3.0/6.3/3.3 mm. Test a module button/support before a full print; do not force tight fits.

Follow [the module assembly guide](ASSEMBLY.md), including first-contact-only support adjustment. Never use the screws to flatten a rocking board. Fit hardware and verify button return, display seating, reset and USB access before applying coverings. The front frame supports the display; the glass is not a clamp surface.

| Full-build item | Quantity / specification |
|---|---|
| Board | 1 LILYGO T-Display-S3, bare headers; verify your revision |
| Battery envelope | Up to 57 × 39 × 8 mm; compatible protected cell, connector/polarity verified against your board |
| Heat-set inserts | 12 M2, 4 mm long, 3.2 mm OD: four body, four faceplate, four adjusters |
| M2 × 8 mm screws | 8: four case screws, four adjusters |
| M2 × 10 mm screws | 4 carrier mounts |
| 6 × 2 mm magnets | 10: four closure, six docking |
| 3 × 2 mm magnets | 6 closure |
| Coverings | Nominal 2 mm felt floors; optional 1 mm PVC outside |
| Consumables | Suitable magnet/cover adhesive, thin soft battery mounting pad |

Dry-fit and mark all magnet polarities before gluing. Keep magnet faces, collars, sockets and lid mating edges clear of felt/PVC/glue. Route the battery wire clear of bosses and screw tips; its connector and bend path are not fully modelled. Do not clamp or puncture the cell. Seven dice pockets and a 155 × 16 mm pen pocket are nominal envelopes; test your contents. The small accessory pocket is generic and includes no miniature model.

## Source and reproduction

Use Python 3.14 (tested on macOS arm64). Create a virtual environment and install `requirements.txt`; `requirements-lock.txt` records the complete tested environment.

```sh
python -m venv .venv
. .venv/bin/activate
pip install -r requirements.txt
python build.py
python package.py
python upholstery/make_patterns.py
python upholstery/make_felt.py
python render.py
python verify.py
```

Run from `hardware/`. `case.py` defines the retained body/lid parametrically; `build.py` defines the current module and generates eleven printable designs (including five fit coupons), original-parts-only `case-assembly.step`, preview meshes and `validation.json`. STEP is editable boundary-representation geometry; Python is the parameter source. `package.py` produces the eight-piece print plate and a portable hardware map. `render.py` renders the actual case meshes to `../media/hero.png` without external fonts or private assets. No historical source mutation scripts are required. VTK rendering needs a graphics-capable session; on headless Linux provide a compatible EGL/OSMesa or Xvfb setup.

For nominal board/contact checks, acquire the manufacturer's model independently and run `python build.py --board-step /path/to/t-display-s3-full.stp`. It is never added to exported STEP or preview meshes. The default build explicitly records board checks as skipped; see [provenance](PROVENANCE.md).

## Coverings and verification

`upholstery/felt-pocket-templates.svg` and `make_felt.py` supply editable floor and lid felt templates with magnet/docking trim guides; verify the 100 mm line.

[Two-piece upholstery](upholstery/README.md) provides the editable generator and two actual-size A4 landscape PDF pages. Paper-test first, then test a scrap of the actual 1 mm material. Keep the current screen surround painted and removable. The shape check establishes exactly two connected nets; it does not prove physical PVC fit.

Build assertions check connected valid solids, watertight meshes, dimensions, non-overlap, insertion/adjustment sweeps, screw/battery clearances and open/closed lid geometry. Optional board checks cover clearances and contact areas. Physical printer tolerances, actual board revision, magnetic holding force, button loads, cable fit, upholstery and long-term creep remain unverified. This package does not claim a completed physical fit or slice validation.
