from __future__ import annotations

import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, simpledialog, ttk

from .bridge.status import BattleStatusOptions
from .campaign import CampaignEngine
from .models import Faction
from .service import GatesOfCodeXService
from .state_io import load, save


FACTION_COLORS = {
    Faction.NATO: "#2f6fb2",
    Faction.UKRAINE: "#d6b42c",
    Faction.RUSSIA: "#9d3c36",
    Faction.PRC: "#b22f34",
    Faction.NEUTRAL: "#777777",
}


class CampaignMapApp(tk.Tk):
    def __init__(self, state_path: str | Path | None = None) -> None:
        super().__init__()
        self.title("Gates of CodeX")
        self.geometry("1220x760")
        self.minsize(900, 620)
        self.state_path: Path | None = Path(state_path) if state_path else None
        self.campaign = load(self.state_path) if self.state_path else None
        self.selected_battalion: str | None = None
        self.selected_province: str | None = None
        self._build_widgets()
        self.refresh()

    def _build_widgets(self) -> None:
        toolbar = ttk.Frame(self)
        toolbar.pack(fill=tk.X, padx=8, pady=8)
        for label, command in (
            ("Open", self.open_campaign),
            ("Save", self.save_campaign),
            ("Move / Attack", self.move_or_attack),
            ("Auto-resolve", self.auto_resolve),
            ("Export Battle", self.export_battle),
            ("Import Battle", self.import_battle),
            ("End Turn", self.end_turn),
        ):
            ttk.Button(toolbar, text=label, command=command).pack(side=tk.LEFT, padx=3)
        self.summary = ttk.Label(toolbar, text="No campaign loaded")
        self.summary.pack(side=tk.RIGHT, padx=6)

        body = ttk.Panedwindow(self, orient=tk.HORIZONTAL)
        body.pack(fill=tk.BOTH, expand=True, padx=8, pady=(0, 8))
        self.canvas = tk.Canvas(body, background="#15181d", highlightthickness=0)
        self.canvas.bind("<Button-1>", self._select_province_at)
        body.add(self.canvas, weight=4)

        side = ttk.Frame(body, padding=8)
        body.add(side, weight=1)
        ttk.Label(side, text="Battalions", font=("TkDefaultFont", 12, "bold")).pack(anchor=tk.W)
        self.battalion_list = tk.Listbox(side, exportselection=False)
        self.battalion_list.pack(fill=tk.BOTH, expand=True, pady=(6, 10))
        self.battalion_list.bind("<<ListboxSelect>>", self._select_battalion)
        self.details = tk.Text(side, height=14, wrap=tk.WORD, state=tk.DISABLED)
        self.details.pack(fill=tk.X)

    def open_campaign(self) -> None:
        selected = filedialog.askopenfilename(
            title="Open Gates of CodeX campaign",
            filetypes=[("Campaign JSON", "*.json"), ("All files", "*.*")],
        )
        if not selected:
            return
        try:
            self.state_path = Path(selected)
            self.campaign = load(self.state_path)
            self.selected_battalion = None
            self.selected_province = None
            self.refresh()
        except Exception as exc:
            messagebox.showerror("Open failed", str(exc))

    def save_campaign(self) -> None:
        if self.campaign is None:
            return
        if self.state_path is None:
            selected = filedialog.asksaveasfilename(
                defaultextension=".json",
                filetypes=[("Campaign JSON", "*.json")],
            )
            if not selected:
                return
            self.state_path = Path(selected)
        try:
            save(self.campaign, self.state_path)
            self.refresh()
        except Exception as exc:
            messagebox.showerror("Save failed", str(exc))

    def refresh(self) -> None:
        self.canvas.delete("all")
        self.battalion_list.delete(0, tk.END)
        if self.campaign is None:
            self.summary.configure(text="No campaign loaded")
            return
        self.summary.configure(
            text=f"Turn {self.campaign.turn_number} | {self.campaign.current_faction.value.upper()}"
        )
        width = max(self.canvas.winfo_width(), 800)
        height = max(self.canvas.winfo_height(), 560)
        xs = [province.x for province in self.campaign.provinces.values()]
        ys = [province.y for province in self.campaign.provinces.values()]
        min_x, max_x = min(xs), max(xs)
        min_y, max_y = min(ys), max(ys)

        def project(x: float, y: float) -> tuple[float, float]:
            px = 55 + (x - min_x) / max(max_x - min_x, 1) * (width - 110)
            py = 55 + (y - min_y) / max(max_y - min_y, 1) * (height - 110)
            return px, py

        for province in self.campaign.provinces.values():
            x1, y1 = project(province.x, province.y)
            for neighbor_id in province.neighbors:
                if province.province_id < neighbor_id:
                    neighbor = self.campaign.provinces[neighbor_id]
                    x2, y2 = project(neighbor.x, neighbor.y)
                    self.canvas.create_line(x1, y1, x2, y2, fill="#4e5661", width=2)
        for province in self.campaign.provinces.values():
            x, y = project(province.x, province.y)
            radius = 23 if province.province_id != self.selected_province else 29
            self.canvas.create_oval(
                x - radius,
                y - radius,
                x + radius,
                y + radius,
                fill=FACTION_COLORS[province.owner],
                outline="#ffffff" if province.province_id == self.selected_province else "#c4c9d0",
                width=3,
                tags=("province", province.province_id),
            )
            self.canvas.create_text(
                x,
                y + radius + 14,
                text=province.display_name,
                fill="#f1f3f5",
                font=("TkDefaultFont", 9),
                tags=("province", province.province_id),
            )
        for battalion_id, battalion in sorted(self.campaign.battalions.items()):
            self.battalion_list.insert(
                tk.END,
                f"{battalion_id} | {battalion.faction.value.upper()} | {battalion.province_id} | {battalion.unit_count}",
            )
        self._update_details()

    def _select_province_at(self, event: tk.Event) -> None:
        items = self.canvas.find_overlapping(event.x - 3, event.y - 3, event.x + 3, event.y + 3)
        for item in reversed(items):
            tags = self.canvas.gettags(item)
            if "province" in tags:
                self.selected_province = next(tag for tag in tags if tag != "province")
                self.refresh()
                return

    def _select_battalion(self, _event: object = None) -> None:
        selection = self.battalion_list.curselection()
        if not selection or self.campaign is None:
            return
        battalion_id = sorted(self.campaign.battalions)[selection[0]]
        self.selected_battalion = battalion_id
        self.selected_province = self.campaign.battalions[battalion_id].province_id
        self.refresh()
        self.battalion_list.selection_set(selection[0])

    def _update_details(self) -> None:
        self.details.configure(state=tk.NORMAL)
        self.details.delete("1.0", tk.END)
        if self.campaign and self.selected_battalion:
            battalion = self.campaign.battalions.get(self.selected_battalion)
            if battalion:
                lines = [
                    battalion.battalion_id,
                    f"Faction: {battalion.faction.value}",
                    f"Province: {battalion.province_id}",
                    f"Supply: {battalion.supply}",
                    "",
                    "Roster:",
                ]
                lines.extend(
                    f"  {entry.quantity} x {entry.unit_name}" for entry in battalion.roster
                )
                self.details.insert("1.0", "\n".join(lines))
        self.details.configure(state=tk.DISABLED)

    def move_or_attack(self) -> None:
        if not self.campaign or not self.selected_battalion or not self.selected_province:
            messagebox.showinfo("Selection required", "Select a battalion and target province.")
            return
        try:
            result = CampaignEngine(self.campaign).move_or_attack(
                self.selected_battalion, self.selected_province
            )
            self.save_campaign()
            if result.pending_battle:
                messagebox.showinfo("Battle pending", result.pending_battle.battle_id)
        except Exception as exc:
            messagebox.showerror("Move failed", str(exc))

    def auto_resolve(self) -> None:
        if not self.campaign:
            return
        try:
            winner = CampaignEngine(self.campaign).auto_resolve_pending_battle()
            self.save_campaign()
            messagebox.showinfo("Battle resolved", f"Winner: {winner.value}")
        except Exception as exc:
            messagebox.showerror("Auto-resolve failed", str(exc))

    def end_turn(self) -> None:
        if not self.campaign:
            return
        try:
            CampaignEngine(self.campaign).end_turn()
            self.save_campaign()
        except Exception as exc:
            messagebox.showerror("End turn failed", str(exc))

    def export_battle(self) -> None:
        if not self.campaign or not self.state_path:
            return
        codex = self.campaign.code_x_directory or filedialog.askdirectory(title="Select Code:X mod")
        if not codex:
            return
        map_string = simpledialog.askstring("Battle map", "GoH map string:")
        if not map_string:
            return
        destination = filedialog.asksaveasfilename(
            defaultextension=".sav",
            filetypes=[("GoH campaign save", "*.sav")],
        )
        if not destination:
            return
        try:
            GatesOfCodeXService().export_pending_battle(
                state_path=self.state_path,
                code_x_directory=codex,
                save_path=destination,
                options=BattleStatusOptions(map_string=map_string),
            )
            self.campaign = load(self.state_path)
            self.refresh()
            messagebox.showinfo("Battle exported", destination)
        except Exception as exc:
            messagebox.showerror("Export failed", str(exc))

    def import_battle(self) -> None:
        if not self.campaign or not self.state_path:
            return
        selected = filedialog.askopenfilename(
            filetypes=[("GoH campaign save", "*.sav"), ("All files", "*.*")]
        )
        if not selected:
            return
        try:
            result = GatesOfCodeXService().import_completed_battle(
                state_path=self.state_path,
                save_path=selected,
            )
            self.campaign = load(self.state_path)
            self.refresh()
            messagebox.showinfo("Battle imported", f"Winner: {result.winner.value}")
        except Exception as exc:
            messagebox.showerror("Import failed", str(exc))


def main(state_path: str | Path | None = None) -> None:
    CampaignMapApp(state_path).mainloop()


if __name__ == "__main__":
    main()
