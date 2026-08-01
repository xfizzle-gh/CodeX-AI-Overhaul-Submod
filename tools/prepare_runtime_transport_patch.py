from pathlib import Path

path = Path("tools/patch_runtime_transport_proof.py")
text = path.read_text(encoding="utf-8")
# Relocate the global motor-band selector edit until after the finisher block is
# replaced. Doing it earlier changes source offsets captured for that block.
start = text.index("    units_hull = f'{units".replace("{units", "{{units"))
end = text.index("    stage_two =", start)
chunk = text[start:end]
if "text = text.replace(units_hull, typed_units_hull)" not in chunk:
    raise RuntimeError("preflight did not find the premature global replacement")
chunk = chunk.replace("    text = text.replace(units_hull, typed_units_hull)\n", "")
text = text[:start] + chunk + text[end:]
needle = "    text = text[:start] + new_body + text[end:]\n    return text\n"
replacement = "    text = text[:start] + new_body + text[end:]\n    text = text.replace(units_hull, typed_units_hull)\n    return text\n"
if text.count(needle) != 1:
    raise RuntimeError("preflight could not relocate the global units-selector replacement")
text = text.replace(needle, replacement)

# Keep exactly one terminal newline in the generated regression-test file.
eof_old = '    path.write_text(text.rstrip() + addition + "\\n", encoding="utf-8")'
eof_new = '    path.write_text(text.rstrip() + addition.rstrip() + "\\n", encoding="utf-8")'
if text.count(eof_old) != 1:
    raise RuntimeError("preflight could not find generated-test newline writer")
text = text.replace(eof_old, eof_new)

path.write_text(text, encoding="utf-8")
print("Prepared runtime transport patcher")
