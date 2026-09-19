"""Optional asset regeneration: pip install qrcode==8.2 (BSD-3-Clause)."""
from pathlib import Path
import qrcode
from qrcode.image.svg import SvgPathImage
url = 'https://tr3v0r86.github.io/turnkeep/setup.html'
qr = qrcode.make(url, image_factory=SvgPathImage, box_size=10, border=4)
qr.save(Path(__file__).resolve().parents[1] / 'site/assets/setup-qr.svg')
print(url)
