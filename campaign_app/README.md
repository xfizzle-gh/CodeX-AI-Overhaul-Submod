# Gates of CodeX

Gates of CodeX is a clean-room strategic campaign application for **Call to Arms: Gates of Hell** with **Code:X**. It recreates the interoperability behavior observed in the Gates of Europa Workshop project without copying or redistributing that project's executable, decompiled source, or Unity assets.

The repository's existing Code:X data remains the authoritative game-data source. The campaign application scans the installed Code:X mod, creates a campaign battle save, launches Gates of Hell, and imports the post-battle survivors and result.

## Current checkpoint

The campaign core currently provides:

- NATO, Ukraine, Russia, and PRC faction identities
- Province graphs with reciprocal adjacency validation
- Persistent battalions and unit rosters
- Movement, neutral capture, battle creation, retreat, casualties, income, and turn order
- Atomic JSON save/load
- A four-faction playable test theater
- Dependency-free Python 3.11 implementation and tests

The Code:X catalog scanner, GoH save bridge, desktop interface, installer, and release packaging are implemented in subsequent checkpoints.

## Development

```powershell
cd campaign_app
py -3.11 -m unittest discover -s tests -v
```

The application intentionally does not contain the uploaded Gates of Europa binary or its proprietary Unity assets.
