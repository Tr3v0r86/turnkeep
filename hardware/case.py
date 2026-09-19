"""Retained compact body and removable L-shaped lid; dimensions in mm."""
from pathlib import Path
import math
import cadquery as cq
ROOT=Path(__file__).resolve().parent
OUT=ROOT/'stl'; OUT.mkdir(exist_ok=True)
W,D=166.,110.
TOP,DECK_UNDER,SEAM,RIM=36.,31.6,20.,30.2
FLOOR,FELT,LEATHER=2.4,2.,1.
CW,BAND=74.,46.
MAG_D,MAG_DEPTH=6.3,2.2
INSERT_D,INSERT_DEPTH,SCREW_CLEAR=3.,4.,2.4
DECK_SCREWS=[(4.,4.),(70.,4.),(4.,42.),(70.,42.)]
RETAINER_SCREWS=[(18,6),(58,6),(18,39),(58,39)]
BOARD_OFFSET=(4.8,22.,29.42)
BUTTONS=[(6.,12.712),(6.,30.81)]
MAGNETS=[(82,4.8,3),(154,4.8,3),(83,77,6),(154,77,6),(48.4,82.6,3)]
DOCK_X=[28,86,144]; DOCK_Z=9.
POCKETS=[('d20',79,9,27,28,2.4),('d12',108.4,9,25,28,4.4),
 ('d10',135.8,9,25,28,7.4),('d100',79,39.4,27,26,7.4),
 ('d8',108.4,39.4,25,26,7.4),('d6',135.8,39.4,25,26,11.4),
 ('d4',50,51.2,24.5,25,8.4),('accessory',4.4,48,41,35.2,2.4),
 ('pen',4.4,85.8,155,16,14.4)]
def box(x,y,z,w,d,h):
    return cq.Workplane('XY').box(w,d,h,centered=(False,False,False)).translate((x,y,z))
def rr(x,y,z,w,d,h,r=3):
    return box(x,y,z,w,d,h).edges('|Z').fillet(r)
def cyl(x,y,z,r,h):
    return cq.Workplane('XY').circle(r).extrude(h).translate((x,y,z))
def cyl_y(x,y,z,r,h):
    return cq.Workplane('XZ').circle(r).extrude(-h).translate((x,y,z))
def shape(obj):
    return obj.val() if isinstance(obj,cq.Workplane) else obj
def intersect_volume(a,b):
    return shape(a).intersect(shape(b)).Volume()
def flip_print(obj):
    return obj.rotate((0,0,0),(1,0,0),180)
def normalize(obj):
    b=shape(obj).BoundingBox()
    return obj.translate((-b.xmin,-b.ymin,-b.zmin))


print('Building compact body and L lid',flush=True)
# Rounded outside shell; electronics occupies only the upper-left corner.
body=rr(0,0,0,W,D,SEAM,5).union(rr(2.8,2.8,SEAM,W-5.6,D-5.6,RIM-SEAM,2.5))
head=rr(0,0,0,CW,BAND,DECK_UNDER,5)
body=body.union(head)
body=body.cut(rr(2.4,2.4,FLOOR,CW-4.8,BAND-4.8,50,2))
for x,y in DECK_SCREWS:
    body=body.union(cyl(x,y,FLOOR,3.,DECK_UNDER-FLOOR).intersect(head))
    body=body.cut(cyl(x,y,DECK_UNDER-6.2,INSERT_D/2,6.5))
    body=body.cut(cq.Workplane('XY').circle(1.5).workplane(offset=.45).circle(1.65).loft().translate((x,y,DECK_UNDER-.45)))
# Short-end locator stops; bay walls locate the long edges without thin extra walls.
locator=box(7,10,FLOOR,1.4,26,2).union(box(66.6,10,FLOOR,1.4,26,2))
locator=locator.cut(box(6.8,17,FLOOR-.1,3,12,3))
body=body.union(locator)
for name,x,y,w,d,z in POCKETS:
    body=body.cut(rr(x,y,z,w,d,50,1 if name=='accessory' else (4 if name=='pen' else 2)))
# Finger scallops are above contents and never invade the electronics bay.
# Dice sit near the rim; omit scallops that leave fragile slivers between rows.
body=body.cut(cyl(81,85.8,RIM-7,5,8))
body=body.cut(box(-1,15.8,27.6,10,12,8.1))
# The lid's notch clears the service module. The inner offset also clears its wall.
lid=rr(0,0,SEAM,W,D,TOP-SEAM,5)
lid=lid.cut(box(-1,-1,SEAM-.1,CW+1.4,BAND+1.4,20))
inner=rr(2.4,2.4,SEAM-.1,W-4.8,D-4.8,TOP-3.6-SEAM+.1,2.6)
inner=inner.cut(box(-1,-1,SEAM-.2,CW+3.8,BAND+3.8,25))
lid=lid.cut(inner)
# Remove the raised body lip under the two internal edges of the L-shaped skirt.
body=body.cut(box(CW,0,SEAM,CW*0+3.2,BAND+3.2,20))
body=body.cut(box(0,BAND,SEAM,CW+3.2,3.2,20))
# Recut accessory and d20 starts below the lip as intended; inner skirt overlaps these
# pocket edges only if packing is wrong, checked with content envelopes below.
for x,y,diam in MAGNETS:
    pr=diam/2+1.4
    # Flush tray-floor magnets; mating body lands project through felt cutouts.
    body=body.union(cyl(x,y,SEAM-.1,pr,32.2-SEAM+.1))
    body=body.cut(cyl(x,y,30.,(diam+.3)/2,2.21))
    lid=lid.cut(cyl(x,y,SEAM-.1,pr+.4,32.4-SEAM+.1))
    lid=lid.cut(cyl(x,y,32.39,(diam+.3)/2,2.21))
lid=lid.cut(rr(73,D-3,SEAM-.1,18,3.5,2.8,1.2))
# Full front edge docks to the body's front after a 180 degree X flip.
for x in DOCK_X:
    collar=cq.Solid.makeCone(5.3,4.3,2,cq.Vector(x,D,DOCK_Z),cq.Vector(0,1,0))
    body=body.union(cq.Workplane('XY').newObject([collar]))
    body=body.cut(cyl_y(x,D-.2,DOCK_Z,MAG_D/2,2.21))
    lid=lid.union(rr(x-7,D-6.4,SEAM,14,6.4,TOP-3.5-SEAM,1))
    lid=lid.cut(cyl_y(x,D-1.2,TOP-DOCK_Z,5.1,1.21))
    lid=lid.cut(cyl_y(x,D-3.4,TOP-DOCK_Z,MAG_D/2,2.2))
    body=body.cut(box(x-7.4,D-6.8,SEAM-.4,14.8,8,15))

print('Building fit coupons',flush=True)
coupon=rr(0,0,0,80,65,7,3)
for i,dia in enumerate([2.9,3.0,3.1,3.2]):
    coupon=coupon.cut(cyl(11+i*18,12,1,dia/2,7))
for i,dia in enumerate([6.1,6.2,6.3,6.4]):
    coupon=coupon.cut(cyl(11+i*18,33,4.8,dia/2,3))
for i,dia in enumerate([3.1,3.2,3.3,3.4]):
    coupon=coupon.cut(cyl(11+i*18,52,4.8,dia/2,3))
# Counts identify rows without relying on tiny printable text: 1..4 ticks left to right.
for i in range(4):
    for tick in range(i+1):
        coupon=coupon.cut(box(7.5+i*18+tick*2,21,6.5,.8,2,1))
        coupon=coupon.cut(box(7.5+i*18+tick*2,41,6.5,.8,2,1))
dock_coupon=rr(0,0,0,22,16,16,2)
dock_coupon=dock_coupon.cut(cyl_y(11,-.01,8,5.1,1.21))
dock_coupon=dock_coupon.cut(cyl_y(11,1.2,8,MAG_D/2,2.2))
dock_male_coupon=rr(0,0,0,22,16,16,2)
dock_male_coupon=dock_male_coupon.union(cq.Workplane('XY').newObject([
    cq.Solid.makeCone(5.3,4.3,2,cq.Vector(11,16,8),cq.Vector(0,1,0))]))
dock_male_coupon=dock_male_coupon.cut(cyl_y(11,15.8,8,MAG_D/2,2.21))
# First print can verify the entire actual electronics deck and retainer independently.
lip_male=rr(0,0,0,42,20,3,2).union(rr(2.8,2.8,3,36.4,14.4,6,1.5))
lip_female=rr(0,0,0,42,20,9,2).cut(rr(2.4,2.4,2.4,37.2,15.2,7,1.6))


