from __future__ import annotations

import hashlib
import json
import re
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Iterable


SUPPORTED_SIDES = {"nato", "ukr", "rusa", "prc"}


@dataclass(slots=True)
class UnitDefinition:
    name: str
    side: str
    template: str = ""
    period: str = ""
    doctrine: str = ""
    members: dict[str, int] = field(default_factory=dict)
    vehicles: list[str] = field(default_factory=list)
    actions: list[str] = field(default_factory=list)
    type_tags: list[str] = field(default_factory=list)
    category: str = "unknown"
    manpower_cost: float = 0.0
    doctrine_cost: float = 0.0
    source_files: list[str] = field(default_factory=list)

    @property
    def raw_name(self) -> str:
        return self.name

    @property
    def is_doctrine_unit(self) -> bool:
        return "Doctrine" in self.type_tags or self.doctrine_cost > 0


@dataclass(slots=True)
class CodeXCatalog:
    units: dict[str, UnitDefinition] = field(default_factory=dict)
    source_signature: str = ""
    scanned_files: int = 0
    warnings: list[str] = field(default_factory=list)

    def by_side(self, side: str) -> list[UnitDefinition]:
        normalized = side.lower()
        return sorted(
            (unit for unit in self.units.values() if unit.side == normalized),
            key=lambda unit: unit.name,
        )

    def to_dict(self) -> dict[str, object]:
        return {
            "source_signature": self.source_signature,
            "scanned_files": self.scanned_files,
            "warnings": list(self.warnings),
            "units": {name: asdict(unit) for name, unit in sorted(self.units.items())},
        }

    def save(self, path: str | Path) -> Path:
        destination = Path(path)
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(
            json.dumps(self.to_dict(), indent=2, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )
        return destination

    @classmethod
    def load(cls, path: str | Path) -> "CodeXCatalog":
        data = json.loads(Path(path).read_text(encoding="utf-8"))
        units = {
            name: UnitDefinition(**payload) for name, payload in data.get("units", {}).items()
        }
        return cls(
            units=units,
            source_signature=data.get("source_signature", ""),
            scanned_files=int(data.get("scanned_files", 0)),
            warnings=list(data.get("warnings", [])),
        )


class CodeXCatalogScanner:
    _definition_name = re.compile(r'^\s*\{\s*"([^"]+)"', re.MULTILINE)
    _template = re.compile(r'\(\s*"([^"]+)"')
    _side = re.compile(r'\bside\(([^)]+)\)')
    _period = re.compile(r'\bperiod\(([^)]+)\)')
    _doctrine = re.compile(r'\bd\(([^)]+)\)')
    _member = re.compile(r'\b(?:c\d+|crew\d*|crew)\(([^():\s]+):(\d+)\)')
    _vehicle = re.compile(r'\bvehicle\d*\(([^)]+)\)')
    _action = re.compile(r'(?:\{\s*action\s+"([^"]+)"\}|\baction\(([^)]+)\))')
    _cost = re.compile(r'\bcost\(([-+]?\d+(?:\.\d+)?)\)')
    _purchase = re.compile(
        r'type\s*=\s*\{(?P<types>[^}]*)\}[^\n\r]*?unit\s*=\s*"(?P<unit>[^"]+)"'
    )
    _quoted = re.compile(r'"([^"]+)"')

    def scan(self, code_x_directory: str | Path) -> CodeXCatalog:
        root = Path(code_x_directory).resolve()
        resource = root / "resource"
        if not resource.is_dir():
            raise FileNotFoundError(f"Code:X resource directory was not found: {resource}")

        catalog = CodeXCatalog()
        signature = hashlib.sha256()
        files = sorted(
            path
            for path in resource.rglob("*")
            if path.is_file() and path.suffix.lower() in {".set", ".lua"}
        )
        for path in files:
            relative = path.relative_to(root).as_posix()
            try:
                raw = path.read_bytes()
                text = raw.decode("utf-8-sig", errors="replace")
            except OSError as exc:
                catalog.warnings.append(f"Could not read {relative}: {exc}")
                continue
            signature.update(relative.encode("utf-8"))
            signature.update(b"\0")
            signature.update(hashlib.sha256(raw).digest())
            catalog.scanned_files += 1
            if path.suffix.lower() == ".set":
                self._scan_set_file(text, relative, catalog)
            else:
                self._scan_lua_file(text, relative, catalog)

        catalog.source_signature = signature.hexdigest()
        self._finalize(catalog)
        if not catalog.units:
            catalog.warnings.append("No Code:X conquest or multiplayer unit definitions were found.")
        return catalog

    def _scan_set_file(self, text: str, source: str, catalog: CodeXCatalog) -> None:
        for block in self._extract_named_blocks(text):
            match = self._definition_name.match(block)
            if not match:
                continue
            name = match.group(1).strip()
            side = self._infer_side(name, block, source)
            if side not in SUPPORTED_SIDES:
                continue

            unit = catalog.units.get(name)
            if unit is None:
                unit = UnitDefinition(name=name, side=side)
                catalog.units[name] = unit
            elif unit.side != side:
                catalog.warnings.append(
                    f"Conflicting side for {name}: {unit.side} vs {side} in {source}"
                )

            template = self._template.search(block)
            period = self._period.search(block)
            doctrine = self._doctrine.search(block)
            if template and not unit.template:
                unit.template = template.group(1).strip()
            if period and not unit.period:
                unit.period = period.group(1).strip()
            if doctrine and not unit.doctrine:
                unit.doctrine = doctrine.group(1).strip()

            for member_name, count in self._member.findall(block):
                unit.members[member_name] = unit.members.get(member_name, 0) + int(count)
            for vehicle in self._vehicle.findall(block):
                if vehicle not in unit.vehicles:
                    unit.vehicles.append(vehicle)
            for left, right in self._action.findall(block):
                action = (left or right).strip()
                if action and action not in unit.actions:
                    unit.actions.append(action)
            costs = [float(value) for value in self._cost.findall(block)]
            if costs:
                unit.doctrine_cost = max(unit.doctrine_cost, max(costs))
            if source not in unit.source_files:
                unit.source_files.append(source)

    def _scan_lua_file(self, text: str, source: str, catalog: CodeXCatalog) -> None:
        for match in self._purchase.finditer(text):
            name = match.group("unit").strip()
            side = self._infer_side(name, "", source)
            if side not in SUPPORTED_SIDES:
                continue
            unit = catalog.units.setdefault(name, UnitDefinition(name=name, side=side))
            for tag in self._quoted.findall(match.group("types")):
                if tag not in unit.type_tags:
                    unit.type_tags.append(tag)
            if source not in unit.source_files:
                unit.source_files.append(source)

    def _finalize(self, catalog: CodeXCatalog) -> None:
        for unit in catalog.units.values():
            unit.type_tags.sort()
            unit.vehicles.sort()
            unit.actions.sort()
            unit.source_files.sort()
            unit.category = self._classify(unit)
            unit.manpower_cost = self._estimate_manpower(unit)

    @staticmethod
    def _estimate_manpower(unit: UnitDefinition) -> float:
        if unit.vehicles:
            base = 150.0 + 35.0 * len(unit.members)
            if unit.category == "tank":
                base += 350.0
            elif unit.category in {"ifv", "artillery", "air_defense"}:
                base += 225.0
            return base
        member_count = sum(unit.members.values())
        if member_count:
            return float(max(50, member_count * 45))
        return 100.0

    @staticmethod
    def _classify(unit: UnitDefinition) -> str:
        tags = {tag.lower() for tag in unit.type_tags}
        haystack = " ".join(
            [unit.name, unit.template, *unit.vehicles, *unit.actions, *unit.type_tags]
        ).lower()
        if "tank" in tags or any(
            token in haystack
            for token in ("m1a", "t72", "t80", "t90", "ztz", "leopard", "challenger", "leclerc")
        ):
            return "tank"
        if "ifv" in tags or any(
            token in haystack for token in ("bmp", "bradley", "marder", "puma", "zbd", "cv90")
        ):
            return "ifv"
        if "artillery" in tags or any(
            token in haystack for token in ("art_", "m109", "pzh", "plz", "howitzer", "mortar")
        ):
            return "artillery"
        if "aa" in tags or any(
            token in haystack
            for token in ("manpads", "stinger", "igla", "pgz", "tunguska", "pantsir", "air_def")
        ):
            return "air_defense"
        if "recon" in tags or any(
            token in haystack for token in ("recon", "scout", "sniper", "lrs")
        ):
            return "recon"
        if "vehicle" in tags or "armored" in tags or unit.vehicles:
            return "vehicle"
        if "infantry" in tags or unit.members or unit.name.startswith("squad_"):
            return "infantry"
        if "air" in tags or any(
            token in haystack for token in ("heli", "helicopter", "sortie")
        ):
            return "air"
        return "unknown"

    @classmethod
    def _extract_named_blocks(cls, text: str) -> Iterable[str]:
        index = 0
        length = len(text)
        while index < length:
            match = cls._definition_name.search(text, index)
            if not match:
                return
            start = match.start()
            depth = 0
            in_quote = False
            escaped = False
            position = start
            while position < length:
                char = text[position]
                if in_quote:
                    if escaped:
                        escaped = False
                    elif char == "\\":
                        escaped = True
                    elif char == '"':
                        in_quote = False
                else:
                    if char == '"':
                        in_quote = True
                    elif char == "{":
                        depth += 1
                    elif char == "}":
                        depth -= 1
                        if depth == 0:
                            yield text[start : position + 1]
                            index = position + 1
                            break
                position += 1
            else:
                return

    @classmethod
    def _infer_side(cls, name: str, block: str, source: str) -> str:
        match = cls._side.search(block)
        if match:
            return cls._normalize_side(match.group(1))
        suffix = re.search(r"\(([^)]+)\)$", name)
        if suffix:
            return cls._normalize_side(suffix.group(1))
        lowered = source.lower().replace("\\", "/")
        for side in SUPPORTED_SIDES:
            if f"/{side}/" in lowered or f".{side}." in lowered or f"_{side}." in lowered:
                return side
        return ""

    @staticmethod
    def _normalize_side(side: str) -> str:
        aliases = {
            "rus": "rusa",
            "russia": "rusa",
            "ukraine": "ukr",
            "china": "prc",
        }
        normalized = side.strip().lower()
        return aliases.get(normalized, normalized)
