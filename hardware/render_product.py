"""Deterministic product photography-style views of the unchanged case CAD.

Original STL triangles, per-face material colours and native depth-based AO.
The screen uses the real 320x170 simulator image. Its rectangle is placed in
our nominal glass opening; this is a visual display plane, not vendor CAD.
Run with the pinned hardware environment in a graphics-capable session.
"""
from pathlib import Path
import hashlib,json
import vtk
from vtk.util.numpy_support import vtk_to_numpy,numpy_to_vtk
import numpy as np
from PIL import Image
ROOT=Path(__file__).resolve().parent
MESH=ROOT/'preview_meshes';OUT=ROOT.parent/'media'
SCREEN=ROOT.parent/'site/assets/screen.png'
OUT.mkdir(exist_ok=True)
assert Image.open(SCREEN).size==(320,170),'Use the actual 320x170 simulator capture'
BG=(.035,.084,.103)
INTERIOR=(.085,.125,.123);COPPER=(.43,.235,.135);DECK=(.15,.235,.245);BRASS=(.72,.48,.235)
source_hashes={}

def mesh_actor(name,color,offset=(0,0,0),docked=False):
    path=MESH/(name+'.stl');source_hashes[name]=hashlib.sha256(path.read_bytes()).hexdigest()
    reader=vtk.vtkSTLReader();reader.SetFileName(str(path));reader.Update()
    poly=reader.GetOutput();assert poly.GetNumberOfPoints()>0
    # Material only: no remeshing, scaling, warping or vertex displacement.
    pts=vtk_to_numpy(poly.GetPoints().GetData())
    cells=vtk_to_numpy(poly.GetPolys().GetConnectivityArray()).reshape(-1,3)
    centers=pts[cells].mean(axis=1)
    if name in ('body','lid'):
        colors=np.tile(np.array(INTERIOR),(len(cells),1))
        # Outer walls/bottom of body and outside top/skirt of lid.
        outer=(centers[:,0]<2.35)|(centers[:,0]>163.65)|(centers[:,1]<2.35)|(centers[:,1]>107.65)
        outer |= centers[:,2]<.05 if name=='body' else centers[:,2]>35.95
        colors[outer]=COPPER
        arr=numpy_to_vtk((colors*255).astype(np.uint8),deep=True,array_type=vtk.VTK_UNSIGNED_CHAR)
        arr.SetName('Original part material colours');poly.GetCellData().SetScalars(arr)
    normals=vtk.vtkPolyDataNormals();normals.SetInputData(poly);normals.SetFeatureAngle(35);normals.SplittingOn();normals.ComputePointNormalsOn();normals.Update()
    mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(normals.GetOutputPort())
    if name in ('body','lid'):mapper.SetScalarModeToUseCellData();mapper.SetColorModeToDirectScalars()
    else:mapper.ScalarVisibilityOff()
    actor=vtk.vtkActor();actor.SetMapper(mapper)
    if docked:actor.RotateX(180);actor.SetPosition(0,221.2,36)
    else:actor.SetPosition(*offset)
    prop=actor.GetProperty();prop.SetColor(*color);prop.SetInterpolationToPBR();prop.SetMetallic(.12);prop.SetRoughness(.68)
    return actor

def display_actor(offset=(0,0,0)):
    # Full nominal opening: x=10.81221..61.98221, y=8.375..35.625.
    # 35.5 leaves the actual 37.2-mm faceplate 1.7 mm proud of the screen.
    plane=vtk.vtkPlaneSource();plane.SetOrigin(10.81221,8.375,35.5);plane.SetPoint1(61.98221,8.375,35.5);plane.SetPoint2(10.81221,35.625,35.5)
    mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(plane.GetOutputPort())
    read=vtk.vtkPNGReader();read.SetFileName(str(SCREEN));read.Update()
    texture=vtk.vtkTexture();texture.SetInputConnection(read.GetOutputPort());texture.InterpolateOn();texture.RepeatOff()
    actor=vtk.vtkActor();actor.SetMapper(mapper);actor.SetTexture(texture);actor.SetPosition(*offset);actor.GetProperty().LightingOff()
    return actor

def render(mode):
    ren=vtk.vtkRenderer();ren.SetBackground(*BG);ren.SetUseFXAA(True)
    ren.AutomaticLightCreationOff()
    body=mesh_actor('body',INTERIOR);ren.AddActor(body)
    if mode=='hero':ren.AddActor(mesh_actor('lid',INTERIOR,docked=True));module_z=0
    elif mode=='closed':ren.AddActor(mesh_actor('lid',INTERIOR));module_z=0
    else:
        ren.AddActor(mesh_actor('lid',INTERIOR,(180,0,0)));module_z=68
        ren.AddActor(mesh_actor('retainer',DECK,(0,0,32)))
        for i in range(4):ren.AddActor(mesh_actor(f'pad_{i}',BRASS,(0,0,42)))
    ren.AddActor(mesh_actor('deck',DECK,(0,0,module_z)))
    for i in range(2):ren.AddActor(mesh_actor(f'button_{i}',BRASS,(0,0,module_z)))
    ren.AddActor(display_actor((0,0,module_z)))
    floor=vtk.vtkPlaneSource();floor.SetOrigin(-800,-800,-.15);floor.SetPoint1(1000,-800,-.15);floor.SetPoint2(-800,1000,-.15)
    mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(floor.GetOutputPort())
    ground=vtk.vtkActor();ground.SetMapper(mapper);ground.GetProperty().SetColor(*BG);ground.GetProperty().SetInterpolationToPBR();ground.GetProperty().SetRoughness(1)
    ren.AddActor(ground)
    for pos,color,intensity in [((-180,-230,430),(1,.89,.75),2.4),((280,-60,260),(.57,.77,1),1.1),((70,340,300),(.85,1,.95),2.5)]:
        light=vtk.vtkLight();light.SetLightTypeToSceneLight();light.SetPosition(*pos);light.SetFocalPoint(85,80,10);light.SetColor(*color);light.SetIntensity(intensity);ren.AddLight(light)
    camera=ren.GetActiveCamera();camera.ParallelProjectionOn();camera.SetViewUp(0,0,1)
    if mode=='hero':target=(83,110,12);position=(270,-255,455);scale=133
    elif mode=='closed':target=(83,55,16);position=(200,-235,355);scale=91
    else:target=(170,55,39);position=(345,-400,450);scale=158
    camera.SetFocalPoint(*target);camera.SetPosition(*position);camera.SetParallelScale(scale)
    steps=vtk.vtkRenderStepsPass();ao=vtk.vtkSSAOPass();ao.SetDelegatePass(steps);ao.SetRadius(13);ao.SetBias(.04);ao.SetKernelSize(128);ao.BlurOn();ren.SetPass(ao)
    win=vtk.vtkRenderWindow();win.SetOffScreenRendering(1);win.SetSize(2100,1600);win.SetMultiSamples(0);win.AddRenderer(ren)
    ren.ResetCameraClippingRange();win.Render()
    capture=vtk.vtkWindowToImageFilter();capture.SetInput(win);capture.ReadFrontBufferOff();capture.Update()
    path=OUT/f'product-{mode}.png';writer=vtk.vtkPNGWriter();writer.SetFileName(str(path));writer.SetInputConnection(capture.GetOutputPort());writer.Write();win.Finalize()
    image=Image.open(path);assert image.size==(2100,1600) and np.asarray(image).std()>8
    print(path.name)
for mode in ('hero','closed','exploded'):render(mode)
# Verify rendering never mutated CAD inputs.
for name,digest in source_hashes.items():assert hashlib.sha256((MESH/(name+'.stl')).read_bytes()).hexdigest()==digest
(OUT/'product-render-validation.json').write_text(json.dumps({'source_STL_sha256':source_hashes,'screen_sha256':hashlib.sha256(SCREEN.read_bytes()).hexdigest(),'screen_pixels':[320,170],'screen_placement':'Nominal opening; original flat visual panel at z=35.5 mm, not vendor board geometry','geometry_unchanged':True,'output_pixels':[2100,1600],'materials':'Per-face visual colour study; not a claim about a fabricated finish','lighting':'Three native VTK lights and depth-based ambient occlusion'},indent=2)+'\n')
