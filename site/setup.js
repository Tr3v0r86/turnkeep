"use strict";
// Static prototype: no network requests, persistence, authentication or device APIs.
const SAMPLE = {
  "format_version": "1.0",
  "character_id": "synthetic-mara-vale",
  "identity": {
    "name": "Mara Vale",
    "ancestry": "Elf",
    "class_name": "Eldritch Knight",
    "level": 4
  },
  "combat": {
    "max_hp": 40,
    "armor_class": 18
  },
  "resources": {
    "spell_slots_max": 3,
    "class_resource_name": "Surge",
    "class_resource_max": 1
  },
  "actions": [
    {
      "id": "longsword",
      "name": "Longsword",
      "detail": "+6 hit | 1d8+4 slashing | 5 ft"
    },
    {
      "id": "handaxe",
      "name": "Handaxe",
      "detail": "+6 hit | 1d6+4 slashing | 20/60"
    },
    {
      "id": "second-wind",
      "name": "Second Wind",
      "detail": "Bonus action | regain 1d10+4 HP"
    }
  ],
  "spells": [
    {
      "id": "shield",
      "name": "Shield",
      "detail": "Reaction | self | +5 AC until next turn"
    },
    {
      "id": "magic-missile",
      "name": "Magic Missile",
      "detail": "Action | 120 ft | three 1d4+1 darts"
    },
    {
      "id": "fire-bolt",
      "name": "Fire Bolt",
      "detail": "Action | 120 ft | +5 hit | 1d10 fire"
    }
  ],
  "checks": [
    {
      "id": "athletics",
      "name": "Athletics",
      "detail": "+6"
    },
    {
      "id": "perception",
      "name": "Perception",
      "detail": "+3"
    },
    {
      "id": "stealth",
      "name": "Stealth",
      "detail": "+2"
    },
    {
      "id": "arcana",
      "name": "Arcana",
      "detail": "+5"
    }
  ],
  "sheet": [
    {
      "id": "defenses",
      "name": "Defenses",
      "detail": "AC 18 | speed 30 | passive perception 13"
    },
    {
      "id": "saves",
      "name": "Saves",
      "detail": "STR +6 | CON +5 | WIS +1"
    },
    {
      "id": "equipment",
      "name": "Equipment",
      "detail": "Longsword | shield | handaxes x2"
    },
    {
      "id": "features",
      "name": "Features",
      "detail": "Action Surge | Second Wind | Weapon Bond"
    }
  ]
};

function validBeyondLink(value) {
  try {
    const url = new URL(value.trim());
    return url.protocol === "https:" && ["www.dndbeyond.com", "dndbeyond.com"].includes(url.hostname)
      && !url.username && !url.password && !url.port && !url.search && !url.hash
      && /^\/characters\/[0-9]+\/?$/.test(url.pathname);
  } catch (_) { return false; }
}

function reviewedPack(values, base = SAMPLE) {
  const name = values.name.trim();
  if (!/^[\x20-\x7e\u00b7]{1,40}$/.test(name)) throw new Error("Enter a name of 1–40 printable ASCII characters or middle dots.");
  const fields = [["max_hp", "Maximum HP", 1, 9999], ["armor_class", "Armor class", 0, 99], ["level", "Level", 1, 20]];
  const numbers = {};
  for (const [key, label, min, max] of fields) {
    const raw = String(values[key]).trim();
    const number = Number(raw);
    if (!/^\d+$/.test(raw) || !Number.isInteger(number) || number < min || number > max) throw new Error(`${label} must be a whole number from ${min} to ${max}.`);
    numbers[key] = number;
  }
  const pack = JSON.parse(JSON.stringify(base));
  pack.identity.name = name;
  pack.identity.level = numbers.level;
  pack.combat = {max_hp: numbers.max_hp, armor_class: numbers.armor_class};
  const slug = name.toLowerCase().replace(/[^a-z0-9]+/g, "-").replace(/^-|-$/g, "").slice(0, 40) || "character";
  pack.character_id = `demo-${slug}-l${numbers.level}`;
  const defenses = pack.sheet.find(entry => entry.id === "defenses");
  if (defenses) defenses.detail = `AC ${numbers.armor_class} | speed 30 | passive perception 13`;
  return pack;
}

function sessionState(pack, hp) {
  if (!Number.isInteger(hp) || hp < 0 || hp > pack.combat.max_hp) throw new Error(`Current HP must be a whole number from 0 to ${pack.combat.max_hp}.`);
  return {format: "turnkeep-demo-session-v1", prototype: true, character_id: pack.character_id,
    counters: {hp, max_hp: pack.combat.max_hp, temp_hp: 0, spell_slots: pack.resources.spell_slots_max, resource: pack.resources.class_resource_max}};
}

// The same small validation boundary is runnable with Node, without a DOM.
if (typeof module !== "undefined") module.exports = {SAMPLE, validBeyondLink, reviewedPack, sessionState};

if (typeof document !== "undefined") {
  const $ = id => document.getElementById(id);
  let pack = JSON.parse(JSON.stringify(SAMPLE));
  let hp = pack.combat.max_hp;
  let photoURL = null;
  const announce = message => { $("status").textContent = message; };

  function show(step) {
    ["choose", "review", "play"].forEach((id, index) => {
      $(id).hidden = id !== step;
      const item = document.querySelectorAll(".progress li")[index];
      if (id === step) item.setAttribute("aria-current", "step");
      else item.removeAttribute("aria-current");
    });
    $(`${step}-heading`).focus({preventScroll: true});
    if (window.matchMedia("(max-width: 720px)").matches) $(step).scrollIntoView({block: "start"});
  }

  function render() {
    $("device-start").hidden = true;
    $("device-character").hidden = false;
    $("device-name").textContent = pack.identity.name;
    $("device-class").textContent = `${pack.identity.ancestry} / ${pack.identity.class_name} ${pack.identity.level}`;
    $("device-hp").textContent = hp;
    $("device-max").textContent = `of ${pack.combat.max_hp} HP`;
    $("device-ac").textContent = `AC ${pack.combat.armor_class}`;
    $("session-hp").max = pack.combat.max_hp;
    $("session-hp").value = hp;
  }

  function populateReview(origin) {
    $("character-name").value = pack.identity.name;
    $("max-hp").value = pack.combat.max_hp;
    $("armor-class").value = pack.combat.armor_class;
    $("character-level").value = pack.identity.level;
    $("review-origin").textContent = origin;
    $("review-error").textContent = "";
    show("review");
    announce("Review sample data. Nothing has been imported or sent to a device.");
  }

  function useSample(origin) {
    pack = JSON.parse(JSON.stringify(SAMPLE));
    hp = pack.combat.max_hp;
    populateReview(origin);
  }

  function collect() {
    try {
      const next = reviewedPack({name: $("character-name").value, max_hp: $("max-hp").value,
        armor_class: $("armor-class").value, level: $("character-level").value}, pack);
      $("review-error").textContent = "";
      return next;
    } catch (error) {
      $("review-error").textContent = error.message;
      $("review-error").scrollIntoView({block: "nearest"});
      return null;
    }
  }

  function download(value, filename) {
    const url = URL.createObjectURL(new Blob([JSON.stringify(value, null, 2) + "\n"], {type: "application/json"}));
    const a = document.createElement("a");
    a.href = url;
    a.download = filename;
    document.body.append(a);
    a.click();
    a.remove();
    setTimeout(() => URL.revokeObjectURL(url), 1000);
  }

  document.querySelectorAll('input[name="source"]').forEach(input => input.addEventListener("change", () => {
    const photo = input.value === "photo";
    $("beyond-form").hidden = photo;
    $("photo-source").hidden = !photo;
    announce(photo ? "Your image stays local. Sample extraction is simulated, not OCR." : "Only the URL format is checked. No character data is fetched.");
  }));

  $("beyond-form").addEventListener("submit", event => {
    event.preventDefault();
    if (!validBeyondLink($("beyond-url").value)) {
      $("beyond-error").textContent = "Use an HTTPS dndbeyond.com/characters/12345678 link with a numeric character ID and no extra query or fragment.";
      $("beyond-url").setAttribute("aria-invalid", "true");
      $("beyond-url").focus();
      return;
    }
    $("beyond-error").textContent = "";
    $("beyond-url").removeAttribute("aria-invalid");
    useSample("Link format checked. No fetch occurred. Reviewing synthetic Mara Vale data.");
  });
  $("sample").addEventListener("click", () => useSample("Synthetic example · Mara Vale. No source was imported."));

  function clearPhoto() {
    if (photoURL) URL.revokeObjectURL(photoURL);
    photoURL = null;
    $("photo-preview").hidden = true;
    $("photo-preview").removeAttribute("src");
    $("extract").disabled = true;
  }
  $("sheet-photo").addEventListener("change", () => {
    clearPhoto();
    $("photo-error").textContent = "";
    const file = $("sheet-photo").files[0];
    if (!file) return;
    if (!["image/jpeg", "image/png", "image/webp"].includes(file.type) || file.size > 10 * 1024 * 1024) {
      $("photo-error").textContent = "Choose a JPEG, PNG or WebP image no larger than 10 MB.";
      return;
    }
    photoURL = URL.createObjectURL(file);
    $("photo-preview").onload = () => { $("photo-preview").hidden = false; $("extract").disabled = false; announce("Local preview ready. No upload or OCR occurred."); };
    $("photo-preview").onerror = () => { clearPhoto(); $("photo-error").textContent = "This image could not be opened. Try a different JPEG, PNG or WebP."; };
    $("photo-preview").src = photoURL;
  });
  $("extract").addEventListener("click", () => useSample("Sample extraction only · Mara Vale. Your image was not read or uploaded."));

  $("review-form").addEventListener("submit", event => {
    event.preventDefault();
    const next = collect();
    if (!next) return;
    pack = next;
    hp = pack.combat.max_hp;
    $("session-error").textContent = "";
    render();
    show("play");
    announce("Simulated transfer complete. This tab’s preview is updated; no device was connected.");
  });
  $("download-pack").addEventListener("click", () => {
    const next = collect();
    if (!next) return;
    download(next, `${next.character_id}.json`);
    announce("CharacterPack JSON download requested. Validate and compile it with the firmware tools to use it on a real board.");
  });
  $("back").addEventListener("click", () => { show("choose"); announce("Choose a source. Loading a sample replaces this review draft."); });

  function applyHP() {
    const raw = $("session-hp").value;
    try {
      if (!/^\d+$/.test(raw)) throw new Error(`Current HP must be a whole number from 0 to ${pack.combat.max_hp}.`);
      const next = Number(raw);
      sessionState(pack, next);
      hp = next;
      render();
      $("session-error").textContent = "";
      return true;
    } catch (error) { $("session-error").textContent = error.message; return false; }
  }
  $("apply-hp").addEventListener("click", () => { if (applyHP()) announce("Preview HP updated in this tab only."); });
  $("export-state").addEventListener("click", () => {
    if (!applyHP()) return;
    download(sessionState(pack, hp), `${pack.character_id}-demo-session.json`);
    announce("Demo session export requested. This is a local counter snapshot, not a device or D&D Beyond sync.");
  });
  $("level-up").addEventListener("click", () => {
    if (!applyHP()) return;
    populateReview("Level-up review · Sample features remain unchanged. Simulated replacement resets HP to full.");
    $("character-level").value = Math.min(20, pack.identity.level + 1);
    announce("Review the new level, maximum HP and AC. Nothing is recalculated automatically.");
  });
  $("restart").addEventListener("click", () => {
    clearPhoto();
    $("sheet-photo").value = "";
    $("beyond-url").value = "";
    $("beyond-error").textContent = "";
    $("beyond-url").removeAttribute("aria-invalid");
    $("photo-error").textContent = "";
    $("device-start").hidden = false;
    $("device-character").hidden = true;
    pack = JSON.parse(JSON.stringify(SAMPLE));
    hp = pack.combat.max_hp;
    show("choose");
    announce("Fresh setup. Previous demo counters were cleared; downloaded files remain on your device.");
  });
  window.addEventListener("pagehide", clearPhoto);
}

// Run `node site/setup.js` for the smallest regression check of the import boundary.
if (typeof module !== "undefined" && require.main === module) {
  const assert = require("node:assert/strict");
  assert(validBeyondLink("https://www.dndbeyond.com/characters/12345"));
  for (const url of ["http://dndbeyond.com/characters/1", "https://dndbeyond.com.evil.test/characters/1", "https://user:pass@dndbeyond.com/characters/1", "https://dndbeyond.com/characters/no", "https://dndbeyond.com/characters/1?token=x"]) assert(!validBeyondLink(url));
  const pack = reviewedPack({name: 'Mara "Vale"', max_hp: "50", armor_class: "19", level: "5"});
  assert.equal(pack.identity.level, 5);
  assert.equal(pack.combat.max_hp, 50);
  assert.throws(() => reviewedPack({name: "A\u0000B", max_hp: 1, armor_class: 1, level: 1}));
  assert.throws(() => reviewedPack({name: "Mara", max_hp: "", armor_class: 1, level: 1}));
  assert.throws(() => sessionState(pack, 51));
  assert.equal(sessionState(pack, 15).counters.hp, 15);
  console.log("Turnkeep setup URL, pack and session checks: PASS");
}
