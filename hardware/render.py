"""Render actual current original-part STLs. No external reference models/fonts."""
from pathlib import Path
import vtk
ROOT=Path(__file__).resolve().parent
OUT=ROOT.parent/'media';OUT.mkdir(exist_ok=True)
def render(module=False):
    ren=vtk.vtkRenderer();ren.SetBackground(.922,.894,.843)
    if module:
        items=[('deck',(.65,.70,.59),14),('retainer',(.24,.33,.27),-4)]
        items += [(f'button_{i}',(.8,.63,.37),14) for i in range(2)]
        items += [(f'pad_{i}',(.8,.63,.37),4) for i in range(4)]
    else:
        items=[('body',(.24,.33,.27),0),('open_docked_lid',(.24,.33,.27),0),('deck',(.65,.70,.59),0)]
        items += [(f'button_{i}',(.8,.63,.37),0) for i in range(2)]
    for name,color,z in items:
        read=vtk.vtkSTLReader();read.SetFileName(str(ROOT/'preview_meshes'/f'{name}.stl'));read.Update()
        mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(read.GetOutputPort())
        actor=vtk.vtkActor();actor.SetMapper(mapper);actor.SetPosition(0,0,z)
        actor.GetProperty().SetColor(*color);actor.GetProperty().SetAmbient(.28);actor.GetProperty().SetDiffuse(.72)
        ren.AddActor(actor)
    camera=ren.GetActiveCamera();camera.ParallelProjectionOn();camera.SetViewUp(0,0,1)
    if module:
        camera.SetFocalPoint(37,23,34);camera.SetPosition(-60,-90,105);camera.SetParallelScale(55)
    else:
        camera.SetFocalPoint(83,110,15);camera.SetPosition(-155,-200,490);camera.SetParallelScale(143)
    win=vtk.vtkRenderWindow();win.SetOffScreenRendering(1);win.SetSize(1800,1400);win.SetMultiSamples(8);win.AddRenderer(ren)
    ren.ResetCameraClippingRange();win.Render()
    capture=vtk.vtkWindowToImageFilter();capture.SetInput(win);capture.ReadFrontBufferOff();capture.Update()
    writer=vtk.vtkPNGWriter();writer.SetFileName(str(OUT/('module.png' if module else 'hero.png')));writer.SetInputConnection(capture.GetOutputPort());writer.Write();win.Finalize()
render();render(True)
print('Rendered hero.png and module.png')
