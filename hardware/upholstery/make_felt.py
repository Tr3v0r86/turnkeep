"""Regenerate the retained floor/lid felt SVG from current case dimensions."""
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parent.parent))
import case
R=Path(__file__).resolve().parent
par={'pockets':case.POCKETS,'closure_magnets':case.MAGNETS,'dock_x':case.DOCK_X}
# Separate floor templates, then an L-shaped lid blank with trim guides.
svg=['<svg xmlns="http://www.w3.org/2000/svg" width="210mm" height="297mm" viewBox="0 0 210 297">',
'<rect width="210" height="297" fill="white"/><g font-family="sans-serif" font-size="3.2">',
'<text x="10" y="10">Turnkeep retained body/lid — 2 mm felt, print at 100%</text>',
'<text x="10" y="16">Dry-trim to fit. Red = trim away for magnet lands / docking pads.</text>']
layout={'pen':(10,25),'d20':(10,55),'d12':(49,55),'d100':(87,55),'d10':(125,55),
'd8':(10,97),'d6':(49,97),'d4':(87,97),'accessory':(125,97)}
for name,x,y,w,d,z in par['pockets']:
    tx,ty=layout[name];radius=.7 if name=='accessory' else (3.7 if name=='pen' else 1.7)
    svg.extend([f'<text x="{tx}" y="{ty-2}">{name}</text>',f'<rect x="{tx}" y="{ty}" width="{w-.6}" height="{d-.6}" rx="{radius}" fill="none" stroke="black" stroke-width=".2"/>'])
svg+=['<text x="10" y="146">Lid blank (flip over to match the inside of the lid)</text>',
'<g transform="translate(7.3,152.3)"><path d="M77.1 2.7 H163.3 V107.3 H2.7 V49.1 H77.1 Z" fill="none" stroke="black" stroke-width=".25"/>']
for x,y,diam in par['closure_magnets']:
    svg.append(f'<circle cx="{x}" cy="{y}" r="{diam/2+2}" fill="none" stroke="red" stroke-width=".2"/>')
for x in par['dock_x']:
    svg.append(f'<rect x="{x-7.3}" y="103.3" width="14.6" height="6.7" fill="none" stroke="red" stroke-width=".2"/>')
svg+=['</g><path d="M10 277 H110 M10 275 V279 M110 275 V279" stroke="black" fill="none" stroke-width=".3"/>',
'<text x="10" y="286">This line must measure exactly 100 mm.</text></g></svg>']
(R/'felt-pocket-templates.svg').write_text('\n'.join(svg),encoding='utf-8')
