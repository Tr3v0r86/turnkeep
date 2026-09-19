"""Five CAD views from original meshes; no vendor or miniature models."""
from pathlib import Path
import vtk
from PIL import Image,ImageDraw,ImageFont
ROOT=Path(__file__).resolve().parent
OUT=ROOT.parent.parent/'media';OUT.mkdir(exist_ok=True)
GREEN=(.23,.34,.28);SAGE=(.67,.72,.62);GOLD=(.82,.64,.36)
def render(filename,items,target,position,scale,title,subtitle):
    ren=vtk.vtkRenderer();ren.SetBackground(.922,.894,.843)
    for path,color,offset in items:
        reader=vtk.vtkSTLReader();reader.SetFileName(str(path));reader.Update()
        assert reader.GetOutput().GetNumberOfPoints()>0,path
        mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(reader.GetOutputPort())
        actor=vtk.vtkActor();actor.SetMapper(mapper);actor.SetPosition(*offset)
        actor.GetProperty().SetColor(*color);actor.GetProperty().SetAmbient(.28);actor.GetProperty().SetDiffuse(.72)
        ren.AddActor(actor)
    if filename=='epaper-concept.png':
        # Original colour patches suggest display content, not working UI.
        for x,y,w,h,color in [(18,17,18,14,(.68,.29,.24)),(40,17,18,14,(.86,.63,.18)),(62,17,18,14,(.27,.48,.56)),(18,39,60,3,(.35,.41,.34)),(18,48,47,3,(.35,.41,.34)),(18,57,55,3,(.35,.41,.34))]:
            patch=vtk.vtkCubeSource();patch.SetCenter(x+w/2,y+h/2,41.1);patch.SetXLength(w);patch.SetYLength(h);patch.SetZLength(.1)
            mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(patch.GetOutputPort())
            actor=vtk.vtkActor();actor.SetMapper(mapper);actor.GetProperty().SetColor(*color);actor.GetProperty().SetAmbient(.5);ren.AddActor(actor)
    camera=ren.GetActiveCamera();camera.ParallelProjectionOn();camera.SetViewUp(0,0,1)
    camera.SetFocalPoint(*target);camera.SetPosition(*position);camera.SetParallelScale(scale)
    win=vtk.vtkRenderWindow();win.SetOffScreenRendering(1);win.SetSize(1800,1400);win.SetMultiSamples(8);win.AddRenderer(ren)
    ren.ResetCameraClippingRange();win.Render()
    capture=vtk.vtkWindowToImageFilter();capture.SetInput(win);capture.ReadFrontBufferOff();capture.Update()
    path=OUT/filename;writer=vtk.vtkPNGWriter();writer.SetFileName(str(path));writer.SetInputConnection(capture.GetOutputPort());writer.Write();win.Finalize()
    image=Image.open(path);draw=ImageDraw.Draw(image)
    draw.text((65,45),title,font=ImageFont.load_default(size=43),fill='#263d31')
    draw.text((65,108),subtitle,font=ImageFont.load_default(size=24),fill='#526153')
    image.save(path)
for name,w,d in [('cyd',198,132),('epaper',224,150),('round',190,142)]:
    r=ROOT/name
    color=(.08,.16,.19) if name in ('cyd','round') else (.94,.93,.85)
    items=[(r/'body.stl',GREEN,(0,0,0)),(r/'deck.stl',SAGE,(0,0,0)),(r/'screen_placeholder.stl',color,(0,0,0)),(r/'lid-open.stl',GREEN,(0,0,0))]
    render(name+'-concept.png',items,(w/2,d,15),(-w,-d*1.9,600),d*1.38,
           {'cyd':'CYD / larger display study','epaper':'Colour e-paper / quiet display study','round':'Round screen / compact display study'}[name],
           'CONCEPT ENVELOPE - assumed dimensions, no board-fit or print validation')
r=ROOT.parent/'preview_meshes'
items=[(r/'body.stl',GREEN,(0,0,0)),(r/'lid.stl',GREEN,(0,0,0)),(r/'deck.stl',SAGE,(0,0,0))]
items += [(r/f'button_{i}.stl',GOLD,(0,0,0)) for i in range(2)]
render('closed.png',items,(83,55,16),(-155,-200,390),99,'Turnkeep / closed case','Current v0.3 module with retained v0.2 body and detachable L-shaped lid')
items=[(r/'body.stl',GREEN,(0,0,0)),(r/'lid.stl',GREEN,(180,0,0)),(r/'deck.stl',SAGE,(0,0,70)),(r/'retainer.stl',GREEN,(0,0,35))]
items += [(r/f'button_{i}.stl',GOLD,(0,0,70)) for i in range(2)]
items += [(r/f'pad_{i}.stl',GOLD,(0,0,45)) for i in range(4)]
render('exploded.png',items,(170,55,43),(-150,-400,480),170,'Turnkeep / original printed parts','Current geometry - lid set aside, screen module separated; electronics omitted')
print('Rendered 5 original-geometry views')
