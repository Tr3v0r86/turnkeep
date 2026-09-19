import json
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools" / "character"))

import dev


class CharacterPackTests(unittest.TestCase):
    def setUp(self):
        self.pack = json.loads(
            (ROOT / "fixtures" / "character" / "synthetic-mara-vale.json").read_text()
        )

    def test_synthetic_pack_validates(self):
        self.assertEqual(dev.validate_pack(self.pack), [])

    def test_unknown_major_version_is_rejected(self):
        self.pack["format_version"] = "2.0"
        self.assertIn("format_version: expected 1.x", dev.validate_pack(self.pack))

    def test_duplicate_entry_ids_are_rejected(self):
        self.pack["actions"][1]["id"] = self.pack["actions"][0]["id"]
        self.assertIn("actions: duplicate id longsword", dev.validate_pack(self.pack))

    def test_missing_combat_field_is_rejected(self):
        del self.pack["combat"]["armor_class"]
        self.assertIn("combat.armor_class: required", dev.validate_pack(self.pack))

    def test_unknown_fields_are_rejected(self):
        self.pack["combat"]["mystery"] = 7
        self.assertIn("combat.mystery: unknown field", dev.validate_pack(self.pack))

    def test_generator_is_deterministic_and_escapes_c_strings(self):
        self.pack["identity"]["name"] = 'Mara "Vale"'
        with tempfile.TemporaryDirectory() as tmp:
            first = Path(tmp) / "first.h"
            second = Path(tmp) / "second.h"
            dev.generate_header(self.pack, first)
            dev.generate_header(self.pack, second)
            self.assertEqual(first.read_bytes(), second.read_bytes())
            self.assertIn('Mara \\"Vale\\"', first.read_text())

    def test_required_values_cannot_be_null(self):
        paths = [(key,) for key in self.pack]
        for group in ("identity", "combat", "resources"):
            paths.extend((group, key) for key in self.pack[group])
        paths.extend(("actions", 0, key) for key in self.pack["actions"][0])
        for path in paths:
            with self.subTest(path=path):
                pack = json.loads(json.dumps(self.pack))
                parent = pack
                for key in path[:-1]:
                    parent = parent[key]
                parent[path[-1]] = None
                self.assertTrue(dev.validate_pack(pack))
                with tempfile.TemporaryDirectory() as tmp:
                    with self.assertRaises(ValueError):
                        dev.generate_header(pack, Path(tmp) / "invalid.h")

    def test_text_outside_font_repertoire_is_rejected(self):
        for text in ("A\0B", "A\nB", "A\x7fB", "A\x85B", "snowman \u2603"):
            with self.subTest(text=repr(text)):
                self.pack["identity"]["name"] = text
                self.assertTrue(dev.validate_pack(self.pack))
        self.pack["identity"]["name"] = "Mara \u00b7 Vale"
        self.assertEqual(dev.validate_pack(self.pack), [])

    def test_tracked_device_header_matches_fixture(self):
        with tempfile.TemporaryDirectory() as tmp:
            generated = Path(tmp) / "pack.h"
            dev.generate_header(dev.load(dev.FIXTURE), generated)
            tracked = ROOT / "apps" / "character" / "include" / "character_pack_generated.h"
            self.assertEqual(generated.read_bytes(), tracked.read_bytes())


if __name__ == "__main__":
    unittest.main()
