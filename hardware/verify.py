"""Check saved public deliverables, independently of the generating build."""
from pathlib import Path
import json, hashlib
import cadquery as cq
import trimesh
from pypdf import PdfReader
ROOT=Path(__file__).resolve().parent
solids=cq.importers.importStep(str(ROOT/'case-assembly.step')).solids().vals()
assert len(solids)==10 and all(s.isValid() and s.Volume()>0 for s in solids)
validation=json.loads((ROOT/'validation.json').read_text())
assert len(validation['parts'])==11
for name,entry in validation['parts'].items():
    path=ROOT/'stl'/f'{name}.stl';mesh=trimesh.load_mesh(path)
    assert mesh.is_watertight and mesh.is_winding_consistent and len(mesh.split())==1 and mesh.volume>0
    assert hashlib.sha256(path.read_bytes()).hexdigest()==entry['sha256']
plate=trimesh.load_mesh(ROOT/'REPAIR_PRINT_PLATE.stl')
assert plate.is_watertight and len(plate.split())==8 and max(plate.extents)<180
pdf=PdfReader(ROOT/'upholstery/output/pdf/Turnkeep-TWO-PIECE-wrap-nets-A4.pdf')
assert len(pdf.pages)==2
for page in pdf.pages:
    assert abs(float(page.mediabox.width)-297*72/25.4)<.01
    assert abs(float(page.mediabox.height)-210*72/25.4)<.01
    assert '100 mm' in page.extract_text()
(ROOT/'step-validation.json').write_text(json.dumps({'original_printed_solids':10,'all_valid':True,'all_positive_volume':True,'external_reference_geometry_included':False},indent=2)+'\n')
print('PASS: 10 STEP solids, 11 STL files + hashes, eight-piece plate, two A4 pages')
