"""Matched electronics enclosure with independently adjustable rear PCB supports.
Millimetres. CadQuery/trimesh. Historical case geometry is retained by source.
"""
from pathlib import Path
import json,math,hashlib
import cadquery as cq
import trimesh
ROOT=Path(__file__).resolve().parent
OUT=ROOT/'stl'; OUT.mkdir(exist_ok=True)
import argparse
import case
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--board-step',type=Path,help='Optional externally acquired LILYGO board model; not redistributed')
args=parser.parse_args()
c=vars(case)
box,rr,cyl,shape,iv,normalize=[c[k] for k in ('box','rr','cyl','shape','intersect_volume','normalize')]
body,lid=c['body'],c['lid']
ref=(cq.importers.importStep(str(args.board_step)).rotate((0,0,0),(1,1,0),180).translate(c['BOARD_OFFSET']) if args.board_step else None)
buttons_xy=[(7.11921,12.71162),(7.00201,30.80962)]
pads_xy=[(7.11921,12.71162),(22.,33.5),(62.8,14.5),(62.8,29.5)]
mounts=c['RETAINER_SCREWS']

# Raised 1.2 mm relative to old flat deck: 1.43 mm of plastic remains above
# the two frame-seat strips. One flat printable face replaces separate bezel.
deck=rr(0,0,31.6,74,46,5.6,5)
for x,y in mounts:
    deck=deck.union(cyl(x,y,26.6,3.1,5.2))
# The planar Z35.77 shoulder seats the FRONT FRAME at both short ends.
# Glass opening has 0.65 mm clearance per side and is never a support surface.
deck=deck.cut(rr(4.0,8.4,19.,62.3,27.4,16.77,.5))
deck=deck.cut(rr(10.81221,8.375,19.,51.17,27.25,21.,.3))
for x,y in mounts:
    deck=deck.cut(cyl(x,y,26.5,1.5,6.3))
for x,y in c['DECK_SCREWS']:
    deck=deck.cut(cyl(x,y,30.8,1.2,9))
    deck=deck.cut(cyl(x,y,35.2,2.3,4))
# Wide pocket for the whole native switch; cylindrical pocket for the flange.
buttons=[]
for x,y in buttons_xy:
    # Restore a guide boss inside the large board cavity, stopping short of
    # the adjacent LCD frame. This gives a real flange ceiling at Z35.2.
    boss=cyl(x,y,34.8,3.1,2.4).intersect(box(0,y-4,34.7,9.0,8,3))
    deck=deck.union(boss)
    deck=deck.cut(box(4.0,y-2.65,29.,5.3,5.3,5.8))
    deck=deck.cut(cyl(x,y,29.,2.1,6.2))
    deck=deck.cut(cyl(x,y,29.,1.4,11))
    buttons.append(cyl(x,y,34.32,1.7,.8).union(cyl(x,y,35.07,1.1,4.73)))
deck=deck.cut(box(-1,15.8,27.6,10.3,12.,12))
# A generous reset relief remains open to the underside; no snap rails.
reset_keepout=box(12.7,32.7,24.9,6.7,4.3,7.0)
deck=deck.cut(reset_keepout)
# Open-ended lateral fences locate PCB EDGES, not its faces. About 0.4 mm
# per side; the board lowers vertically and is never slid into a groove.
for guide in [box(28,7.65,29.2,20,1.2,3.0),
              box(28,35.15,29.2,20,1.2,3.0),
              box(65.98,17,29.2,1.2,10,3.0),
              box(3.2,10.9,29.2,1.2,3.6,3.0),
              box(3.2,29,29.2,1.2,3.6,3.0)]:
    deck=deck.union(guide)

def link(x1,y1,x2,y2,width,z,h):
    length=math.hypot(x2-x1,y2-y1)
    return box(0,-width/2,z,length,width,h).rotate((0,0,0),(0,0,1),math.degrees(math.atan2(y2-y1,x2-x1))).translate((x1,y1,0))

# Flat print base, deep rear component clearance and unchanged four mount XYs.
retainer=rr(11.8,3.,20.,54.6,39.4,3.2,2.4)
retainer=retainer.cut(box(10.8,8.8,19.,51.,27.8,6.))
for x,y in mounts:
    retainer=retainer.union(cyl(x,y,20.,3.1,6.6))
for (x,y),(mx,my) in zip(pads_xy,[(18,6),(18,39),(58,6),(58,39)]):
    retainer=retainer.union(link(x,y,mx,my,4.6,20.,3.2))
    retainer=retainer.union(cyl(x,y,20.,3.4,7.2))
for x,y in mounts:
    retainer=retainer.cut(cyl(x,y,19.,1.2,9))
for x,y in pads_xy:
    # Install M2 x 4 mm / OD3.2 heat-set from the flat BACK, flush with Z20.
    # Guide is open from the front. Screw never directly touches the PCB.
    retainer=retainer.cut(cyl(x,y,19.9,1.5,4.1))
    retainer=retainer.cut(cyl(x,y,19.9,1.2,7.5))
    retainer=retainer.cut(cyl(x,y,24.2,1.7,4.0))
retainer=retainer.cut(reset_keepout).clean()

pins=[]
for x,y in pads_xy:
    # 4.2 tall, longer 3 mm guide body, reduced 2 mm insulating PCB tip.
    # At nominal height, 1.98 mm remains inside the guide; at +0.6, 1.38 mm.
    pin=cyl(x,y,25.22,1.5,2.3).union(cyl(x,y,27.42,1.,2.0))
    pins.append(pin)

checks={}
def check(name,a,b):
    if a is None or b is None: return
    value=iv(a,b)
    checks[name]=round(value,7)
    assert value<1e-5,(name,value)
pieces={'deck':deck,'retainer':retainer,**{f'button_{i}':p for i,p in enumerate(buttons)},**{f'pad_{i}':p for i,p in enumerate(pins)}}
for name,obj in pieces.items():
    check(name+'_board',obj,ref)
    check(name+'_body',obj,body)
    check(name+'_lid_closed',obj,lid)
    check(name+'_battery',obj,box(9,3.5,3.4,57,39,8))
items=list(pieces.items())
for i,(n,a) in enumerate(items):
    for m,b in items[i+1:]: check(n+'__'+m,a,b)
for i,(x,y) in enumerate(buttons_xy):
    insertion=cyl(x,y,22.32,1.7,12.8).union(cyl(x,y,23.07,1.1,16.73))
    check(f'button_{i}_continuous_insertion',insertion,deck)
    stroke=cyl(x,y,32.32,1.7,2.8).union(cyl(x,y,33.07,1.1,6.73))
    check(f'button_{i}_two_mm_motion_clearance',stroke,deck)
for i,(x,y) in enumerate(pads_xy):
    # Guide-body movement range; PCB itself limits forward travel in use.
    sweep=cyl(x,y,24.92,1.5,3.2).union(cyl(x,y,27.12,1.,2.9))
    check(f'pad_{i}_adjustment_sweep',sweep,retainer)
    # M2x8 jack screw: tip at pad bottom, head is free behind the base.
    screw=cyl(x,y,17.22,1.,8).union(cyl(x,y,15.62,2.,1.6))
    check(f'adjuster_{i}_board',screw,ref)
    check(f'adjuster_{i}_battery',screw,box(9,3.5,3.4,57,39,8))
    check(f'adjuster_{i}_plastic',screw,retainer)
    check(f'adjuster_{i}_case',screw,body)
for i,(x,y) in enumerate(mounts):
    screw=cyl(x,y,20.,1.,10).union(cyl(x,y,18.4,2.,1.6))
    for n,obj in [('deck',deck),('retainer',retainer),('board',ref),('body',body)]:check(f'mount_{i}_{n}',screw,obj)
for i,(x,y) in enumerate(c['DECK_SCREWS']):
    screw=cyl(x,y,27.2,1,8).union(cyl(x,y,35.2,2,1.6))
    for n,obj in [('deck',deck),('retainer',retainer),('board',ref),('body',body)]:check(f'case_screw_{i}_{n}',screw,obj)

# Verify contact AREA, not merely a non-overlap. The back contacts are bare
# PCB face; the front contacts are frame solid 2, never the glass solid 1.
contacts=[]
front_contact=None
if ref is not None:
    rear=[f for f in shape(ref).Faces() if f.BoundingBox().zlen<1e-4 and abs(f.Center().z-29.42)<1e-4 and f.Area()>500][0]
    for x,y in pads_xy:
        area=rear.intersect(shape(cyl(x,y,29.41,1.,.02))).Area()
        assert area>3.13,('PCB contact not wholly clear',x,y,area)
        contacts.append({'xy':[x,y],'area_mm2':area})
    frame=ref.solids().vals()[2]
    front_contact=shape(deck).intersect(frame.translate((0,0,.01))).Volume()/.01
    assert front_contact>30,('Insufficient frame seat',front_contact)
    
    # Retainer comes directly down onto mounts, without threading PCB into rails.
    for step in (1,3,6,10): check('retainer_approach_'+str(step),retainer.translate((0,0,-step)),ref)
    for step in (0.25,0.5,1,2,4,8,12):
        check('board_approach_'+str(step),ref.translate((0,0,-step)),deck)
    for dx,dy in ((.2,0),(-.2,0),(0,.2),(0,-.2)):
        check(f'board_xy_allowance_{dx}_{dy}',ref.translate((dx,dy,0)),deck)
        for i,pin in enumerate(pins):
            check(f'pad_{i}_board_xy_allowance_{dx}_{dy}',pin,ref.translate((dx,dy,0)))
for i,(x,y) in enumerate(pads_xy):
    fully_retracted=cyl(x,y,16.2,1,8).union(cyl(x,y,14.6,2,1.6))
    check(f'adjuster_{i}_retracted_battery',fully_retracted,box(9,3.5,3.4,57,39,8))
    check(f'adjuster_{i}_retracted_body',fully_retracted,body)
open_lid=lid.rotate((0,0,0),(1,0,0),180).translate((0,221.2,36))
for name,obj in pieces.items(): check(name+'_docked_lid',obj,open_lid)
check('body_lid_closed',body,lid)
check('body_lid_docked',body,open_lid)
parts={'01_body':body,'02_rolling_lid':lid,'03_faceplate_v03':deck,'04_adjustable_retainer_v03':retainer,
       '05_front_button_PRINT_2':buttons[0],'06_insulating_support_PRINT_4':pins[0],
       '08_fit_coupon':c['coupon'],'09_lid_fit_male':c['lip_male'],
       '10_lid_fit_female':c['lip_female'],'11_dock_socket_coupon':c['dock_coupon'],
       '12_dock_collar_coupon':c['dock_male_coupon']}
manifest={}
for name,obj in parts.items():
    assert shape(obj).isValid() and len(obj.solids().vals())==1,name
    oriented=obj.rotate((0,0,0),(1,0,0),180) if name.startswith(('02','03')) else obj
    path=OUT/(name+'.stl')
    cq.exporters.export(normalize(oriented),str(path),tolerance=.015,angularTolerance=.1)
    m=trimesh.load_mesh(path)
    assert m.is_watertight and m.is_winding_consistent and m.volume>0 and len(m.split())==1,name
    assert max(m.extents)<180
    manifest[name]={'bounds_mm':m.extents.round(3).tolist(),'watertight':True,'one_solid':True,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()}
assembly=cq.Assembly(name='Turnkeep_current_case')
for name,obj in {**pieces,'existing_body':body,'existing_lid':lid}.items():assembly.add(obj,name=name)
assembly.export(str(ROOT/'case-assembly.step'))
(ROOT/'preview_meshes').mkdir(exist_ok=True)
for name,obj in {**pieces,'body':body,'lid':lid,'open_docked_lid':open_lid}.items():cq.exporters.export(obj,str(ROOT/'preview_meshes'/(name+'.stl')))

for i,(x,y) in enumerate(pads_xy):
    jack=cyl(x,y,17.22,1.,8).union(cyl(x,y,15.62,2.,1.6))
    cq.exporters.export(jack,str(ROOT/'preview_meshes'/f'jack_{i}.stl'))
(ROOT/'validation.json').write_text(json.dumps({'board_reference_checked':ref is not None,'parts':manifest,'clearance_checks_mm3':checks,'bare_PCB_contacts':contacts,'frame_contact_area_mm2':front_contact,'geometry':{'button_centres':buttons_xy,'rear_pads':pads_xy,'front_frame_seat_z':35.77,'rear_PCB_z':29.42,'faceplate_top_z':37.2,'guide_diameter':2.8,'button_stem_diameter':2.2,'pad_tip_diameter':2.0},'limits':['Board checks run only with --board-step; physical printing tolerances and exact board revision not certified.','Support adjustment is required on the physical board. No FEA or measured button-load test performed.','Switch travel check verifies plastic clearance, not the switch mechanical travel specification.']},indent=2))
print(json.dumps(manifest,indent=2));print('CHECKS PASS',len(checks),'front frame contact area',front_contact,flush=True)
