from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "tools" / "apply_allied_fow_vision_owner.py"

spec = importlib.util.spec_from_file_location("allied_fow_owner", MODULE_PATH)
module = importlib.util.module_from_spec(spec)
assert spec and spec.loader
sys.modules[spec.name] = module
spec.loader.exec_module(module)


PREFIX = '''local function positiveId(primary, fallback)
\tprimary = tonumber(primary or 0) or 0
\tfallback = tonumber(fallback or 0) or 0
\tif primary > 0 then return primary end
\tif fallback > 0 then return fallback end
\treturn 0
end

local function publishIdentity(id)
\tif id.attacking ~= true then return end
\tlocal sc = scene()
\tif not sc or not sc.SetVar then
\t\tlog("identity_publish_skipped", "Scene.SetVar_missing")
\t\treturn
\tend
'''

SUFFIX = '''end

local function nextFunction()
end
'''


class AlliedFowOwnerTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.path = self.root / module.RUNTIME_PATH
        self.path.parent.mkdir(parents=True)
        self.path.write_text(PREFIX + module.OLD_BLOCK + SUFFIX, encoding="utf-8")

    def tearDown(self) -> None:
        self.temp.cleanup()

    def test_routes_to_real_ai_teammate_without_human_ownership(self) -> None:
        self.assertTrue(module.apply(self.root))
        text = self.path.read_text(encoding="utf-8")
        module.validate_text(text)
        self.assertIn(
            "local ownerId = positiveId(id.defenderBotId, id.playerId)", text
        )
        self.assertIn('sc:SetVar("id_attack_support", ownerId)', text)
        self.assertNotIn('sc:SetVar("id_attack_support", id.playerId)', text)
        self.assertNotIn("id.firstPlayerId", text)
        self.assertIn('sc:SetVar("attack_support_ready", 1)', text)
        self.assertIn('sc:SetVar("attack_support_use_mi", 1)', text)

    def test_is_idempotent_and_check_mode_accepts_valid_file(self) -> None:
        self.assertTrue(module.apply(self.root))
        first = self.path.read_bytes()
        self.assertFalse(module.apply(self.root))
        self.assertFalse(module.apply(self.root, check_only=True))
        self.assertEqual(first, self.path.read_bytes())

    def test_check_mode_rejects_unpatched_file(self) -> None:
        with self.assertRaisesRegex(RuntimeError, "has not been applied"):
            module.apply(self.root, check_only=True)

    def test_preserves_utf8_bom(self) -> None:
        original = self.path.read_text(encoding="utf-8")
        self.path.write_text(original, encoding="utf-8-sig")
        self.assertTrue(module.apply(self.root))
        self.assertTrue(self.path.read_bytes().startswith(b"\xef\xbb\xbf"))


if __name__ == "__main__":
    unittest.main()
