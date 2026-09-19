"""Two connected exterior nets for 1 mm PVC. ReportLab, Shapely, pypdf.

Retained v0.2 body/lid, current v0.3 deck left painted. Units mm.
Fold lines are nominal edge datums, not scores. The 0.5 mm bend/trim
allowance is provisional: fit paper, then a scrap of the actual PVC.
"""
from pathlib import Path
import json, math
from shapely.geometry import box, Point
from shapely.ops import unary_union
from shapely.affinity import affine_transform
from reportlab.pdfgen import canvas
from reportlab.lib.units import mm
from reportlab.lib.colors import HexColor
from pypdf import PdfReader

ROOT=Path(__file__).resolve().parent
OUT=ROOT/'output/pdf';OUT.mkdir(parents=True,exist_ok=True)
DEST=OUT/'Turnkeep-TWO-PIECE-wrap-nets-A4.pdf'
W,D,R=166.,110.,5.
ARC=math.pi/2*5.5
BEND=.5
LOW=20-.3+BEND
HIGH=31.6-.3+BEND
LID=16-.3+BEND
INK=HexColor('#23332e'); GREY=HexColor('#777777'); BLUE=HexColor('#245aa1')
cv=canvas.Canvas(str(DEST),pagesize=(297*mm,210*mm),pageCompression=1)
cv.setTitle('Turnkeep - exactly two connected upholstery pieces - A4 100 percent')
specs=[]

def rr():
    return box(R,R,W-R,D-R).buffer(R,quad_segs=48)

def make_net(lid):
    panel=rr()
    if lid: panel=panel.difference(box(-1,-1,74.4,46.4))
    # Four wall flaps share full hinge edges with the center panel.
    # A single attached wing at each corner wraps the vertical R5 arc.
    # Wings start 2 mm away from the hinge to relieve compound curvature.
    h=LID if lid else LOW
    topstart=74.8 if lid else 5
    leftstart=46.8 if lid else 5
    parts=[panel, box(topstart,-h,161,0),box(166,5,166+h,105),
           box(5,110,161,110+h),box(-h,leftstart,0,105),
           box(161,-h,161+ARC,-2),box(168,105,166+h,105+ARC),
           box(5-ARC,112,5,110+h)]
    if not lid:
        parts += [box(5,-HIGH,69,0),box(-HIGH,5,0,41),
                  box(-HIGH,5-ARC,-2,5)]
    net=unary_union(parts)
    folds=[((topstart,0),(161,0)),((166,5),(166,105)),
           ((5,110),(161,110)),((0,leftstart),(0,105))]
    guides=[((161,-h),(161,-2)),((168,105),(166+h,105)),
            ((5,112),(5,110+h))]
    if not lid: guides += [((-HIGH,5),(-2,5))]
    holes=[Point(x,110+9+BEND).buffer(6 if lid else 6.3,quad_segs=48)
           for x in (28,86,144)]
    if lid:
        holes += [box(72.2,110+12.5+BEND,91.8,110+h+1)]
    else:
        holes += [box(-HIGH-1,15.2,-27.2-BEND,28.4)]
    net=net.difference(unary_union(holes))
    assert net.geom_type=='Polygon' and net.is_valid
    assert net.area>18000
    # Mirror the underside net to show the outward-facing material correctly.
    if not lid:
        net=affine_transform(net,[-1,0,0,1,W,0])
        folds=[((W-a[0],a[1]),(W-b[0],b[1])) for a,b in folds]
        guides=[((W-a[0],a[1]),(W-b[0],b[1])) for a,b in guides]
    return net,folds,guides

def text(x,y,s,size=8,color=INK):
    cv.setFillColor(color);cv.setFont('Helvetica',size)
    cv.drawString(x*mm,(210-y)*mm,s)

def page(lid,num):
    net,folds,guides=make_net(lid)
    minx,miny,maxx,maxy=net.bounds
    ox=(297-(maxx-minx))/2-minx
    oy=30-miny
    assert ox+minx>=12 and ox+maxx<=285 and oy+maxy<=193
    def pt(p):return ((p[0]+ox)*mm,(210-p[1]-oy)*mm)
    title='1 - LID: outside top + attached sides' if lid else '2 - BASE: underside + attached sides'
    text(12,12,title,15)
    text(12,19,'ONE connected PVC piece on this page. Solid = CUT. Blue dashed = FOLD gently; do not cut or score.',8.5)
    text(12,25,'Grey dotted = corner-wing guide. The corner wings stay attached. No separate strips. No inward turn-ins.',8)
    path=cv.beginPath()
    for ring in [net.exterior,*net.interiors]:
        coords=list(ring.coords);path.moveTo(*pt(coords[0]))
        for xy in coords[1:]:path.lineTo(*pt(xy))
        path.close()
    cv.setStrokeColor(INK);cv.setLineWidth(.22*mm);cv.drawPath(path,stroke=1,fill=0)
    for segments,color,dash in [(folds,BLUE,[2*mm,1.1*mm]),(guides,GREY,[.5*mm,1*mm])]:
        cv.setStrokeColor(color);cv.setDash(dash)
        for a,b in segments:cv.line(*pt(a),*pt(b))
    cv.setDash()
    tx=ox+20;ty=oy+59
    text(tx,ty,'LID OUTSIDE FACE' if lid else 'BASE UNDERSIDE',12)
    rows=['Print at 100% / Actual size on A4 LANDSCAPE.',
          'Check the 100 mm line below before cutting.',
          'Cut and tape this PAPER net onto the case first.',
          'Fold side flaps; curve attached wings around corners.',
          'Trim to fit. Leave tiny corner reliefs painted.',
          'Trace with this printed face DOWN onto PVC backing.',
          'Fold PVC gently over the case edge, then glue sides.',
          'Keep docking holes, USB and mating edges clear.']
    for i,t in enumerate(rows):text(tx,ty+7+i*4.4,t,8)
    # Keep text entirely on the center panel.
    if lid:
        text(ox+84,oy+20,'SCREEN NOTCH',9)
        text(ox+84,oy+26,'No flap on either inside edge.',7.5)
        text(ox+84,oy+32,'Paint the screen surround.',7.5)
    else:
        text(ox+29,oy+15,'OUTSIDE VIEW - USB cutout on RIGHT',9)
        text(ox+29,oy+22,'Taller flaps cover the fixed screen-corner walls.',8)
    text(ox+54,oy+108,'FRONT / MAGNETIC DOCKING EDGE',7)
    text(12,201,'100% / NO FIT TO PAGE | 1 mm PVC | Paper + scrap trial required',8)
    cv.setStrokeColor(INK);cv.setLineWidth(.3*mm)
    cv.line(181*mm,10*mm,281*mm,10*mm)
    for x in (181,281):cv.line(x*mm,8.5*mm,x*mm,11.5*mm)
    text(218,196,'100 mm',8)
    specs.append({'piece':'lid' if lid else 'base','connected_components':1,
                  'valid_polygon':True,'cut_bounds_mm':[maxx-minx,maxy-miny],
                  'area_mm2':net.area,'interior_cutouts':len(net.interiors),
                  'page_orientation':'landscape','outside_view_mirrored':not lid})
    cv.showPage()

page(True,1);page(False,2);cv.save()
pdf=PdfReader(DEST)
assert len(pdf.pages)==2
for p in pdf.pages:
    assert abs(float(p.mediabox.width)-297*mm)<.01
    assert abs(float(p.mediabox.height)-210*mm)<.01
    assert '100 mm' in p.extract_text()
(OUT/'validation.json').write_text(json.dumps({'total_material_pieces':2,
    'units':'mm','scale':'1:1','material_mm':1,'corner_neutral_radius_mm':5.5,
    'provisional_bend_trim_allowance_mm':BEND,'pieces':specs,
    'physical_fit':'Unverified; paper then material scrap required.'},indent=2)+'\n')
print(DEST)
