# OpenDCL

OpenDCL is a modern dialog and UI toolkit for AutoLISP programmers. It replaces AutoCAD's limited DCL (Dialog Control Language) with Windows forms and controls for use in AutoCAD and compatible platforms (BricsCAD, ZWCAD, GstarCAD, and others as supported by each release).

This repository is the **source code** for OpenDCL Studio and Runtime (product
version **10.1.2.2** - see `Runtime/ARX.rc` FILEVERSION / PRODUCTVERSION).

## Links

| Resource | URL |
| --- | --- |
| Project site / downloads | [https://opendcl.github.io/](https://opendcl.github.io/) (GitHub Pages) - [https://www.opendcl.com/](https://www.opendcl.com/) |
| Ask a question | [GitHub Discussions Q&A](https://github.com/opendcl/community/discussions/new?category=q-a) |
| Wishlist | [GitHub Discussions Wishlist](https://github.com/opendcl/community/discussions/new?category=wishlist) |
| Report a bug | [OpenDCL Issues](https://github.com/opendcl/OpenDCL/issues/new?template=bug.yml) |
| Forum archive (history) | [https://opendcl.github.io/forum-archive/](https://opendcl.github.io/forum-archive/) |
| Online help | [https://opendcl.github.io/HelpFiles/](https://opendcl.github.io/HelpFiles/) or [https://www.opendcl.com/HelpFiles/](https://www.opendcl.com/HelpFiles/) |
| Releases (installers) | [GitHub Releases](https://github.com/opendcl/OpenDCL/releases) (when published) |
| Historical project | [SourceForge OpenDCL](https://sourceforge.net/projects/opendcl/) |

## License

OpenDCL is free software under the **GNU General Public License version 2 or later**. See [LICENSE](LICENSE).

Copyright (c) 2007-2026 OpenDCL Consortium. OpenDCL is a trademark of OpenDCL Consortium.

OpenDCL is based on ObjectDCL (3rd Day Software), released as open source under the GPL in 2006 by Chad Wanless.

## Repository layout

```text
CMakeLists.txt       CMake multi-runtime + Studio + RxInstall (supported build)
CMakePresets.json    Dev / full / no-SDK presets (see CMAKE.md)
Common/              Shared dialog/control core
Library/             Bundled third-party code (LibPNG, ZLib)
Runtime/             CAD runtime modules (ARX / BRX / GRX / ZRX) + VI props
Studio/              OpenDCL Studio (dialog editor) + localized help content
```

## Building

**Supported path:** CMake presets. Step-by-step for a first build:
**[docs/BUILD-QUICKSTART.md](docs/BUILD-QUICKSTART.md)**. Framework
reference: **[CMAKE.md](CMAKE.md)**.

**Typical requirements:**

- CMake >= 3.24 (`vs2026-*` needs **CMake 4.2+**)
- Visual Studio 2022 with Windows desktop C++ and **MFC**
- **Visual Studio 2026** (same workloads) whenever you compile **`ARX.26.x64`** or later **v145+** hosts â€” including a Full product catalog. Preferred Full preset is **`vs2026-full`** (`vs2022-full` also works with v145 installed)
- ObjectARX / BRX / ZRX / GRX SDKs for any CAD Runtime modules you build (not in this repository); Studio-only builds need no CAD SDK

```powershell
# Studio only (no CAD SDK):
cmake --preset vs2022-x86-studio
cmake --build --preset vs2022-x86-studio-debug

# Dev default: latest modern SDK per family (set any ARX*/BRX*/GRX*/ZRX* roots;
# missing families omitted; Studio still builds with zero SDKs):
cmake --preset vs2022-dev
cmake --build --preset vs2022-dev-debug
```

Open the generated solution under `build/<preset>/` (`.sln` on VS2022,
`.slnx` on VS2026). There is no checked-in `OpenDCL.sln`.

**Older Platform Toolsets:** historic matrix rows use pre-VS2015 toolsets
(`v70`-`v120`, and related XP variants). Install
**[Daffodil](https://github.com/manusoft-gh/manusoft-daffodil)** so MSBuild can
resolve those toolsets under a current VS host. Modern presets (`v141`+) do not
need Daffodil.

There is no public CI build that produces official installers from this tree alone.

## Contributing

- **Ask a question** in [Discussions Q&A](https://github.com/opendcl/community/discussions/new?category=q-a).
- **Wishlist** (new capabilities) in [Discussions Wishlist](https://github.com/opendcl/community/discussions/new?category=wishlist).
- **Report a bug** with [OpenDCL Issues](https://github.com/opendcl/OpenDCL/issues/new?template=bug.yml).
- Historic forum threads: [forum archive](https://opendcl.github.io/forum-archive/).
- Prefer pull requests against `main` once the GitHub project is active.
- **Squash-merge** PRs unless there is a reason to keep commits separated.

## History

This Git repository was imported from the SourceForge Subversion trunk  
`https://svn.code.sf.net/p/opendcl/code/Trunk` (full history through r411).
