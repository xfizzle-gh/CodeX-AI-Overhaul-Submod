from pathlib import Path

path = Path("tools/patch_runtime_transport_proof.py")
text = path.read_text(encoding="utf-8")
old = '''    units_hull = f'{units {ignore_captured_by_user 0} {tag {engine.hull}}}'
    typed_units_hull = f'{units {ignore_captured_by_user 0} {tag {engine.hull}} {type vehicle}}'
    text = text.replace(units_hull, typed_units_hull)

    stage_two = f'{"set_i" {var "{engine.stage_var}"} {op "="} {value 2}}'
'''
# Braces in the patcher are doubled inside its source-level f-strings.
old = old.replace("{units", "{{units").replace(" 0}", " 0}}").replace("{tag ", "{{tag ").replace("}}'", "}}}}'")
# Use literal slices instead of depending on the transformation above when Python's
# source formatting changes.
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
path.write_text(text, encoding="utf-8")
print("Prepared runtime transport patcher")
