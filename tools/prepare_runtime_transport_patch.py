from pathlib import Path

path = Path("tools/patch_runtime_transport_proof.py")
text = path.read_text(encoding="utf-8")

# The distance-band helper is telemetry only. Keep its established selector spelling;
# vehicle-only scoping is required on actual hull orders, not on the probe.
start = text.index("    units_hull = f'{units".replace("{units", "{{units"))
end = text.index("    stage_two =", start)
text = text[:start] + text[end:]

# Stage 9 is the invalid-package witness. Extra drive/band zero writes are redundant
# and would change the pinned telemetry write counts.
lines = []
for line in text.splitlines(keepends=True):
    if "{engine.drive_var}" in line or "{engine.band_var}" in line:
        continue
    lines.append(line)
text = "".join(lines)

# Keep exactly one terminal newline in the generated regression-test file.
eof_old = '    path.write_text(text.rstrip() + addition + "\\n", encoding="utf-8")'
eof_new = '    path.write_text(text.rstrip() + addition.rstrip() + "\\n", encoding="utf-8")'
if text.count(eof_old) != 1:
    raise RuntimeError("preflight could not find generated-test newline writer")
text = text.replace(eof_old, eof_new)

path.write_text(text, encoding="utf-8")
print("Prepared runtime transport patcher")
