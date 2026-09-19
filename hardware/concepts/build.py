"""Original visual envelope studies, NOT printable or electronics-fit-validated parts."""
from pathlib import Path
import json
import cadquery as cq
import trimesh
ROOT=Path(__file__).resolve().parent
SPECS={
 'cyd':{'width':198,'depth':132,'height':42,'screen_bay':[94,64],'panel':[72,48]},
 'round':{'width':190,'depth':142,'height':40,'screen_bay':[82,82],'panel':[62,62],'panel_shape':'circle'},
 'epaper':{'width':224,'depth':150,'height':40,'screen_bay':[112,82],'panel':[90,62]},
}
def box(x,y,z,w,d,h,r=0):
    p=cq.Workplane('XY').box(w,d,h,centered=(False,False,False)).translate((x,y,z))
    return p.edges('|Z').fillet(r) if r else p
validation={}
for name,s in SPECS.items():
    out=ROOT/name;out.mkdir(exist_ok=True)
    w,d,h=s['width'],s['depth'],s['height'];bw,bd=s['screen_bay'];pw,pd=s['panel']
    body=box(0,0,0,w,d,h-12,6)
    # Solid dividing walls; display slab is only a visual placeholder.
    body=body.cut(box(3,3,3,bw-6,bd-6,h,3))
    cw=(w-bw-9)/3
    for row in range(2):
        for col in range(3):body=body.cut(box(bw+3+col*(cw+2),3+row*(bd/2),4,cw-2,bd/2-4,h,2))
    body=body.cut(box(3,bd+3,4,w-6,d-bd-6,h,3))
    deck=box(0,0,h-12,bw,bd,12,6)
    panel=box((bw-pw)/2,(bd-pd)/2,h,pw,pd,1,2)
    if s.get('panel_shape')=='circle':
        panel=cq.Workplane('XY').circle(pw/2).extrude(1).translate((bw/2,bd/2,h))
    lid=box(0,0,h-12,w,d,12,6).cut(box(-1,-1,h-13,bw+1,bd+1,16))
    lid=lid.cut(box(3,3,h-13,w-6,d-6,10,3))
    # The open tray pose is illustrative. No mating magnets/fit are designed.
    docked=lid.rotate((0,0,0),(1,0,0),180).translate((0,2*d+3,h))
    parts={'body':body,'deck':deck,'screen_placeholder':panel,'lid':lid}
    assembly=cq.Assembly(name=name+'_CONCEPT_NOT_FOR_PRINT')
    manifest={}
    for part,obj in parts.items():
        assert obj.val().isValid() and len(obj.solids().vals())==1
        assembly.add(obj,name=part)
        path=out/(part+'.stl');cq.exporters.export(obj,str(path))
        mesh=trimesh.load_mesh(path);assert mesh.is_watertight and mesh.volume>0
        manifest[part]={'watertight':True,'positive_volume':True}
    assembly.export(str(out/'concept.step'))
    cq.exporters.export(docked,str(out/'lid-open.stl'))
    saved=cq.importers.importStep(str(out/'concept.step')).solids().vals()
    assert len(saved)==4 and all(s.isValid() and s.Volume()>0 for s in saved)
    for path in out.glob('*.stl'):
        mesh=trimesh.load_mesh(path)
        assert mesh.is_watertight and mesh.is_winding_consistent and mesh.volume>0
    validation[name]={'assumptions_mm':s,'parts':manifest,'fit_validated':False,'print_release':False}
(ROOT/'validation.json').write_text(json.dumps(validation,indent=2)+'\n')
print(f'PASS: {len(SPECS)} conceptual assemblies, {len(SPECS)*4} valid solids, {len(SPECS)*5} watertight view meshes')
