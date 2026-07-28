from pathlib import Path

path = Path("tools/expand_cwa_ownership_probe.py")
text = path.read_text(encoding="utf-8")
old = "            self.assertIn(f'{{{{value {{player_id}}}}}', self.probe)"
new = "            self.assertIn(f'{{{{value {{player_id}}}}}}', self.probe)"
if text.count(old) != 1:
    raise RuntimeError(f"expected one nested f-string marker, found {text.count(old)}")
path.write_text(text.replace(old, new, 1), encoding="utf-8")
