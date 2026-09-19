"""Boundary check: the release allowlist cannot escape or silently include extras."""
from pathlib import Path
from tempfile import TemporaryDirectory
from release import checked_file, file_list

with TemporaryDirectory() as directory:
    root = Path(directory).resolve()
    (root / 'ok.txt').write_text('public')
    (root / 'unlisted.txt').write_text('not a release input')
    (root / 'RELEASE_FILES.txt').write_text('ok.txt\n')
    assert file_list(root) == ['ok.txt']
    for name in ['../outside', '/tmp/outside', 'x\\y', 'C:/x', './ok.txt', 'a//b', 'missing', '']:
        try:
            checked_file(root, name)
        except ValueError:
            pass
        else:
            raise AssertionError(name)
    (root / 'link').symlink_to(root / 'ok.txt')
    try:
        checked_file(root, 'link')
    except ValueError:
        pass
    else:
        raise AssertionError('symlink accepted')
    (root / 'RELEASE_FILES.txt').write_text('ok.txt\nok.txt\n')
    try:
        file_list(root)
    except ValueError:
        pass
    else:
        raise AssertionError('duplicate accepted')
print('Release boundary checks passed')
