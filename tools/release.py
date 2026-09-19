"""Package only explicitly reviewed files. Run from any directory with Python 3.10+."""
from pathlib import Path, PurePosixPath
import hashlib
import json
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[1]
VERSION = '0.1.0-prototype.4'


def checked_file(root, name):
    path = PurePosixPath(name)
    if not name or '\\' in name or ':' in name or path.is_absolute() or '..' in path.parts or str(path) != name:
        raise ValueError(f'Unsafe or noncanonical path: {name!r}')
    target = root / name
    if any(p.is_symlink() for p in [target, *target.parents] if p != root.parent):
        raise ValueError(f'Symlink forbidden: {name}')
    if not target.is_file() or target.stat().st_size == 0:
        raise ValueError(f'Missing or empty release file: {name}')
    target.resolve().relative_to(root.resolve())
    return target


def file_list(root):
    names = [s for s in (root / 'RELEASE_FILES.txt').read_text().splitlines() if s and not s.startswith('#')]
    if len(set(names)) != len(names):
        raise ValueError('Duplicate release paths')
    for name in names:
        checked_file(root, name)
    return sorted(names)


def license_for(name):
    if '/ui/fonts/' in name or '/tools/fonts/' in name:
        return 'OFL-1.1'
    if name.startswith('LICENSES/') or 'LICENSE' in Path(name).name or name.endswith(('.ttf', '.woff2')):
        return 'See applicable license and THIRD_PARTY_NOTICES.md'
    if name.endswith(('.md', '.png', '.jpg')):
        return 'CC-BY-4.0; third-party notices retained where applicable'
    if name.startswith('hardware/'):
        return 'CERN-OHL-P-2.0'
    return 'MIT; third-party notices retained where applicable'


def build(root=ROOT):
    names = file_list(root)
    git = subprocess.run(['git', 'rev-parse', '--verify', 'HEAD'], cwd=root, capture_output=True, text=True)
    revision = git.stdout.strip() if git.returncode == 0 else 'uncommitted-preview'
    dirty = bool(subprocess.run(['git', 'status', '--porcelain'], cwd=root, capture_output=True, text=True).stdout.strip()) if git.returncode == 0 else True
    output = root / 'site/downloads'
    output.mkdir(parents=True, exist_ok=True)
    entries = []
    archive = output / 'turnkeep-source.zip'
    with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as bundle:
        for name in names:
            data = checked_file(root, name).read_bytes()
            info = zipfile.ZipInfo('turnkeep/' + name, date_time=(1980, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o100644 << 16
            bundle.writestr(info, data)
            entries.append(dict(path=name, bytes=len(data), sha256=hashlib.sha256(data).hexdigest(), license=license_for(name)))
    data = archive.read_bytes()
    manifest = dict(version=VERSION, source_commit=revision, working_tree_dirty=dirty, status='experimental; physical fit and endurance acceptance pending', components=dict(body='0.2', lid='0.2', screen_module='0.3'), archive=dict(path=archive.name, bytes=len(data), sha256=hashlib.sha256(data).hexdigest()), files=entries)
    (output / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    (output / 'SHA256SUMS.txt').write_text(f'{manifest["archive"]["sha256"]}  {archive.name}\n')
    print(f'{len(entries)} reviewed files; {len(data):,} byte archive; source {revision}')
    return manifest


if __name__ == '__main__':
    build()
