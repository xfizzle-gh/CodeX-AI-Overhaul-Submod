from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONQUEST = ROOT / "resource/script/multiplayer/modes/conquest.lua"
WORKFLOW = ROOT / ".github/workflows/apply-dcg-identity-retry-fix.yml"
SELF = Path(__file__).resolve()

source = CONQUEST.read_text(encoding="utf-8")
old = "\telseif firstEnemyId <= 0 then\n\t\tmissionIdentityRetryPending = true\n"
new = "\telseif firstEnemyId <= 0 or defenderBotId <= 0 or firstPlayerId <= 0 then\n\t\t-- Retry once on the first quant: new Conquest IDs may settle after GameStart.\n\t\tmissionIdentityRetryPending = true\n"
if source.count(old) != 1:
    raise RuntimeError(f"expected one retry condition, found {source.count(old)}")
source = source.replace(old, new, 1)
CONQUEST.write_text(source, encoding="utf-8", newline="\n")

if WORKFLOW.exists():
    WORKFLOW.unlink()
if SELF.exists():
    SELF.unlink()
