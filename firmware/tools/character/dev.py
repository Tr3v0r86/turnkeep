#!/usr/bin/env python3
import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / "fixtures" / "character" / "synthetic-mara-vale.json"
ID_RE = re.compile(r"^[a-z0-9][a-z0-9-]{0,63}$")
ENTRY_GROUPS = ("actions", "spells", "checks", "sheet")


def _required(obj, key, path, errors):
    if key not in obj:
        errors.append(f"{path}.{key}: required" if path else f"{key}: required")
        return None
    return obj[key]


def _bounded_string(value, path, errors, limit):
    if not isinstance(value, str) or not value or len(value) > limit:
        errors.append(f"{path}: expected non-empty string <= {limit} characters")
    elif any(not (" " <= char <= "~" or char == "\u00b7") for char in value):
        errors.append(f"{path}: supported text is printable ASCII and middle dot only")


def _unknown(obj, allowed, path, errors):
    for key in sorted(set(obj) - set(allowed)):
        errors.append(f"{path + '.' if path else ''}{key}: unknown field")


def validate_pack(pack):
    errors = []
    if not isinstance(pack, dict):
        return ["pack: expected object"]
    _unknown(pack, ("format_version", "character_id", "identity", "combat", "resources",
                    *ENTRY_GROUPS), "", errors)

    version = _required(pack, "format_version", "", errors)
    if not isinstance(version, str) or not version.startswith("1."):
        errors.append("format_version: expected 1.x")
    character_id = _required(pack, "character_id", "", errors)
    if not isinstance(character_id, str) or not ID_RE.fullmatch(character_id):
        errors.append("character_id: expected lowercase slug")

    identity = _required(pack, "identity", "", errors)
    if isinstance(identity, dict):
        _unknown(identity, ("name", "ancestry", "class_name", "level"), "identity", errors)
        for key in ("name", "ancestry", "class_name"):
            value = _required(identity, key, "identity", errors)
            _bounded_string(value, f"identity.{key}", errors, 40)
        level = _required(identity, "level", "identity", errors)
        if not isinstance(level, int) or isinstance(level, bool) or not 1 <= level <= 20:
            errors.append("identity.level: expected integer 1..20")
    else:
        errors.append("identity: expected object")

    combat = _required(pack, "combat", "", errors)
    if isinstance(combat, dict):
        _unknown(combat, ("max_hp", "armor_class"), "combat", errors)
        for key, low, high in (("max_hp", 1, 9999), ("armor_class", 0, 99)):
            value = _required(combat, key, "combat", errors)
            if not isinstance(value, int) or isinstance(value, bool) or not low <= value <= high:
                errors.append(f"combat.{key}: expected integer {low}..{high}")
    else:
        errors.append("combat: expected object")

    resources = _required(pack, "resources", "", errors)
    if isinstance(resources, dict):
        _unknown(resources, ("spell_slots_max", "class_resource_name", "class_resource_max"),
                 "resources", errors)
        for key in ("spell_slots_max", "class_resource_max"):
            value = _required(resources, key, "resources", errors)
            if not isinstance(value, int) or isinstance(value, bool) or not 0 <= value <= 99:
                errors.append(f"resources.{key}: expected integer 0..99")
        name = _required(resources, "class_resource_name", "resources", errors)
        _bounded_string(name, "resources.class_resource_name", errors, 24)
    else:
        errors.append("resources: expected object")

    for group in ENTRY_GROUPS:
        entries = _required(pack, group, "", errors)
        if not isinstance(entries, list):
            errors.append(f"{group}: expected array")
            continue
        if len(entries) > 64:
            errors.append(f"{group}: maximum 64 entries")
        seen = set()
        for index, entry in enumerate(entries):
            path = f"{group}[{index}]"
            if not isinstance(entry, dict):
                errors.append(f"{path}: expected object")
                continue
            _unknown(entry, ("id", "name", "detail"), path, errors)
            entry_id = _required(entry, "id", path, errors)
            if not isinstance(entry_id, str) or not ID_RE.fullmatch(entry_id):
                errors.append(f"{path}.id: expected lowercase slug")
            elif entry_id in seen:
                errors.append(f"{group}: duplicate id {entry_id}")
            else:
                seen.add(entry_id)
            for key, limit in (("name", 48), ("detail", 512)):
                value = _required(entry, key, path, errors)
                _bounded_string(value, f"{path}.{key}", errors, limit)
    return errors


def _c(value):
    return json.dumps(value, ensure_ascii=True)


def _generation(character_id):
    value = 2166136261
    for byte in character_id.encode("utf-8"):
        value = ((value ^ byte) * 16777619) & 0xFFFFFFFF
    return value


def generate_header(pack, output):
    errors = validate_pack(pack)
    if errors:
        raise ValueError("\n".join(errors))
    identity = pack["identity"]
    combat = pack["combat"]
    resources = pack["resources"]
    lines = [
        "// Generated by tools/character/dev.py; edit the JSON fixture, not this file.",
        "#pragma once",
        "",
        "typedef struct { const char *name; const char *detail; } character_pack_entry_t;",
        f"#define CHARACTER_PACK_ID {_c(pack['character_id'])}",
        f"#define CHARACTER_PACK_GENERATION 0x{_generation(pack['character_id']):08X}u",
        f"#define CHARACTER_PACK_NAME {_c(identity['name'])}",
        f"#define CHARACTER_PACK_ANCESTRY {_c(identity['ancestry'])}",
        f"#define CHARACTER_PACK_CLASS {_c(identity['class_name'])}",
        f"#define CHARACTER_PACK_LEVEL {identity['level']}",
        f"#define CHARACTER_PACK_MAX_HP {combat['max_hp']}",
        f"#define CHARACTER_PACK_AC {combat['armor_class']}",
        f"#define CHARACTER_PACK_SLOTS_MAX {resources['spell_slots_max']}",
        f"#define CHARACTER_PACK_RESOURCE_NAME {_c(resources['class_resource_name'])}",
        f"#define CHARACTER_PACK_RESOURCE_MAX {resources['class_resource_max']}",
        "",
    ]
    for group in ENTRY_GROUPS:
        symbol = f"CHARACTER_PACK_{group.upper()}"
        lines.append(f"static const character_pack_entry_t {symbol}[] = {{")
        for entry in pack[group]:
            lines.append(f"    {{ {_c(entry['name'])}, {_c(entry['detail'])} }},")
        lines.append("};")
        lines.append(f"#define {symbol}_COUNT {len(pack[group])}")
        lines.append("")
    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text("\n".join(lines), encoding="utf-8")


def load(path):
    return json.loads(Path(path).read_text(encoding="utf-8"))


def main():
    parser = argparse.ArgumentParser(description="CharacterPack validation and generation")
    sub = parser.add_subparsers(dest="command", required=True)
    validate = sub.add_parser("validate")
    validate.add_argument("pack", nargs="?", default=FIXTURE)
    generate = sub.add_parser("generate")
    generate.add_argument("pack", nargs="?", default=FIXTURE)
    generate.add_argument("output")
    sub.add_parser("doctor")
    args = parser.parse_args()

    if args.command == "doctor":
        errors = validate_pack(load(FIXTURE))
        print(f"python {sys.version_info.major}.{sys.version_info.minor}: ok")
        print(f"fixture: {'ok' if not errors else 'invalid'}")
        return 1 if errors else 0
    pack = load(args.pack)
    errors = validate_pack(pack)
    if errors:
        print("\n".join(errors), file=sys.stderr)
        return 1
    if args.command == "generate":
        generate_header(pack, args.output)
    print(f"{args.pack}: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
