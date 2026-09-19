"""Render an example kit: original CAD + explicitly illustrative purchased parts.
Run in the hardware environment: python hardware/render_kit.py.
No supplier models, battery capacity or final kit quantities are asserted.
"""
import hashlib
import json
import vtk
from PIL import Image
from render_product import mesh_actor, BG, INTERIOR, DECK, BRASS, ROOT, OUT, SCREEN, source_hashes

ren=vtk.vtkRenderer(); ren.SetBackground(*BG); ren.SetUseFXAA(True)

def add(actor):
    if actor.GetTexture() is None: actor.GetProperty().SetInterpolationToPBR(); actor.GetProperty().SetRoughness(.72)
    ren.AddActor(actor); return actor

def box(x,y,z,w,h,d,color):
    src=vtk.vtkCubeSource(); src.SetCenter(x+w/2,y+h/2,z+d/2); src.SetXLength(w);src.SetYLength(h);src.SetZLength(d)
    mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(src.GetOutputPort())
    actor=vtk.vtkActor();actor.SetMapper(mapper);actor.GetProperty().SetColor(*color); return add(actor)

def disc(x,y,z,r,h,color):
    src=vtk.vtkCylinderSource();src.SetRadius(r);src.SetHeight(h);src.SetResolution(32)
    mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(src.GetOutputPort())
    actor=vtk.vtkActor();actor.SetMapper(mapper);actor.RotateX(90);actor.SetPosition(x,y,z+h/2);actor.GetProperty().SetColor(*color);return add(actor)

def label(text,x,y):
    actor=vtk.vtkTextActor();actor.SetInput(text);actor.SetPosition(x,y)
    prop=actor.GetTextProperty();prop.SetFontFamilyToArial();prop.SetFontSize(25);prop.SetColor(.83,.87,.82)
    ren.AddViewProp(actor)

# All original printed pieces; only rigid translations/rotations are applied.
add(mesh_actor('body',INTERIOR,(0,135,0)))
add(mesh_actor('lid',INTERIOR,(0,-2,0),docked=False)).RotateX(180)
# Flip lid about its actual axis and place at y=110: open interior upward.
ren.GetActors().GetLastActor().SetPosition(0,110,36)
add(mesh_actor('deck',DECK,(187,177,-27)))
add(mesh_actor('retainer',DECK,(187,93,0)))
for i in range(2):
    a=mesh_actor(f'button_{i}',BRASS); b=a.GetBounds();a.SetPosition(198+i*20-b[0],87-b[2],-b[4]);add(a)
for i in range(4):
    a=mesh_actor(f'pad_{i}',BRASS);b=a.GetBounds();a.SetPosition(198+i*17-b[0],63-b[2],-b[4]);add(a)

# Purchased components are simple visual proxies, not dimensional vendor CAD.
box(300,193,0,68,31,2,(.04,.09,.08))
box(308,197,2,51.2,27.2,2,(.015,.02,.025))
plane=vtk.vtkPlaneSource();plane.SetOrigin(308,197,4.1);plane.SetPoint1(359.2,197,4.1);plane.SetPoint2(308,224.2,4.1)
mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(plane.GetOutputPort())
reader=vtk.vtkPNGReader();reader.SetFileName(str(SCREEN))
texture=vtk.vtkTexture();texture.SetInputConnection(reader.GetOutputPort());texture.InterpolateOn()
a=vtk.vtkActor();a.SetMapper(mapper);a.SetTexture(texture);a.GetProperty().LightingOff();add(a)
box(298,201,1,6,11,4,(.62,.65,.65))
for y in (198,219):disc(364,y,2,2,2,BRASS)
# Silver pouch, yellow insulation, disconnected two-wire lead.
box(310,137,0,48,34,5,(.65,.68,.68));box(310,164,5,48,7,.6,(.77,.54,.12))
for x,c in ((326,(.72,.12,.07)),(330,(.03,.04,.04))):
    line=vtk.vtkLineSource();line.SetPoint1(x,171,3);line.SetPoint2(x,182,3)
    tube=vtk.vtkTubeFilter();tube.SetInputConnection(line.GetOutputPort());tube.SetRadius(.65);tube.SetNumberOfSides(12)
    m=vtk.vtkPolyDataMapper();m.SetInputConnection(tube.GetOutputPort());a=vtk.vtkActor();a.SetMapper(m);a.GetProperty().SetColor(*c);add(a)
box(323,181,1,10,5,4,(.85,.84,.75))
for i in range(12):
    x=302+(i%6)*11;y=76+(i//6)*17
    disc(x,y,0,1,8 if i<8 else 10,(.47,.51,.53));disc(x,y,8 if i<8 else 10,2,1.5,(.6,.64,.66))
for i in range(12):disc(302+(i%6)*11,37+(i//6)*13,0,2,4,BRASS)
for i in range(8):disc(199+(i%4)*17,17+(i//4)*15,0,3,2,(.57,.62,.64))
# Material blanks, intentionally not presented as cut pattern outlines.
box(397,145,0,101,88,2,(.38,.19,.10));box(402,151,2,91,76,1,(.44,.245,.14))
box(397,38,0,101,83,2,(.11,.17,.16));box(402,44,2,90,70,2,(.16,.23,.21))
# Fine ruled bench, studio lighting and readable category labels.
box(-20,-20,-2,545,282,1,BG)
ren.AutomaticLightCreationOff()
for pos,power in [((-70,-120,450),1.5),((410,260,400),1.2)]:
    light=vtk.vtkLight();light.SetPosition(*pos);light.SetFocalPoint(240,120,0);light.SetIntensity(power);ren.AddLight(light)
camera=ren.GetActiveCamera();camera.ParallelProjectionOn();camera.SetPosition(245,-45,720);camera.SetFocalPoint(245,125,0);camera.SetViewUp(0,1,0);camera.SetParallelScale(163)
for text,x,y in [('Printed body + rolling lid',90,67),('Faceplate + carrier',780,1010),('Buttons + supports',780,340),('Magnets',780,110),('ESP32 + actual UI',1190,1010),('LiPo + lead',1230,605),('M2 screws',1190,375),('Heat-set inserts',1190,185),('Leather blanks',1560,630),('Felt blanks',1560,220)]:label(text,x,y)
win=vtk.vtkRenderWindow();win.SetOffScreenRendering(1);win.SetSize(2000,1200);win.SetMultiSamples(0);win.AddRenderer(ren);ren.ResetCameraClippingRange();win.Render()
capture=vtk.vtkWindowToImageFilter();capture.SetInput(win);capture.ReadFrontBufferOff();capture.Update()
path=OUT/'kit-layout.png';writer=vtk.vtkPNGWriter();writer.SetFileName(str(path));writer.SetInputConnection(capture.GetOutputPort());writer.Write();win.Finalize()
assert Image.open(path).size==(2000,1200)
for name,digest in source_hashes.items():assert hashlib.sha256((ROOT/'preview_meshes'/(name+'.stl')).read_bytes()).hexdigest()==digest
(OUT/'kit-layout-validation.json').write_text(json.dumps({'source_STL_sha256':source_hashes,'geometry_unchanged':True,'screen_sha256':hashlib.sha256(SCREEN.read_bytes()).hexdigest(),'illustrative_only':['board housing','battery and lead','fasteners','magnets','uncut felt and leather blanks'],'status':'Example kit layout; contents, quantities, battery selection and finishes not finalized'},indent=2)+'\n')
print(path)
