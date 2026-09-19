from pathlib import Path
import json,hashlib
import trimesh
from PIL import Image,ImageDraw,ImageFont
ROOT=Path(__file__).resolve().parent
layout=[('03_faceplate_v03',5,5),('04_adjustable_retainer_v03',85,5),
        ('05_front_button_PRINT_2',15,62),('05_front_button_PRINT_2',25,62)]
layout += [('06_insulating_support_PRINT_4',x,62) for x in (35,45,55,65)]
meshes=[]
for name,x,y in layout:
    m=trimesh.load_mesh(ROOT/'stl'/(name+'.stl'));m.apply_translation((x,y,0));meshes.append(m)
plate=trimesh.util.concatenate(meshes)
plate.export(ROOT/'REPAIR_PRINT_PLATE.stl')
reload=trimesh.load_mesh(ROOT/'REPAIR_PRINT_PLATE.stl')
assert reload.is_watertight and reload.is_winding_consistent and len(reload.split())==8
assert max(reload.extents)<180 and abs(reload.bounds[0,2])<.0001
validation=json.loads((ROOT/'validation.json').read_text())
validation['combined_print_plate']={'pieces':8,'watertight':True,'bounds_mm':reload.extents.round(3).tolist(),'sha256':hashlib.sha256((ROOT/'REPAIR_PRINT_PLATE.stl').read_bytes()).hexdigest()}
(ROOT/'validation.json').write_text(json.dumps(validation,indent=2))

im=Image.new('RGB',(1500,1000),'#f5f7f9');d=ImageDraw.Draw(im)
f=lambda n:ImageFont.load_default(size=n)
d.text((40,25),'Rear carrier hardware map | PCB-facing side',font=f(37),fill='#20343b')
d.text((40,85),'USB end LEFT. Turn the adjustment screws from the opposite (back) side.',font=f(25),fill='#52646a')
def xy(x,y):return (130+x*17,190+y*13)
d.rounded_rectangle([xy(2,0),xy(70,45)],radius=30,outline='#b7c4c8',width=3)
mounts=[(18,6),(58,6),(18,39),(58,39)]
pads=[(7.11921,12.71162),(22,33.5),(62.8,14.5),(62.8,29.5)]
for prefix,points,color in [('M',mounts,'#287480'),('J',pads,'#b47716')]:
    for i,(x,y) in enumerate(points):
        px,py=xy(x,y);d.ellipse((px-22,py-22,px+22,py+22),fill=color)
        d.text((px+30,py-20),f'{prefix}{i+1}',font=f(29),fill=color)
d.text((40,810),'M1-M4: M2 x 10 mm mounting screws - gently seat on the posts.',font=f(27),fill='#287480')
d.text((40,860),'J1-J4: M2 x 8 mm adjustment screws + inserts + plastic support pins.',font=f(27),fill='#b47716')
d.text((40,915),'J screws stop at light PCB contact. Their heads MUST NOT be tightened against the carrier.',font=f(25),fill='#842c20')
im.save(ROOT/'hardware-map.png')
