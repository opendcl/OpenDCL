# OpenDCL

OpenDCL is a modern dialog and UI toolkit for AutoLISP programmers. It replaces AutoCAD’s limited DCL (Dialog Control Language) with Windows forms and controls for use in AutoCAD and compatible platforms (BricsCAD, ZWCAD, GstarCAD, and others as supported by each release).

This repository is the **source code** for OpenDCL Studio and Runtime (product
version **10.1.1.1** — see `Runtime/ARX.rc` FILEVERSION / PRODUCTVERSION).

## Links

| Resource | URL |
| --- | --- |
| Project site / downloads | [https://opendcl.github.io/](https://opendcl.github.io/) (GitHub Pages) · [https://www.opendcl.com/](https://www.opendcl.com/) |
| Community (new Q&A) | [GitHub Discussions](https://github.com/opendcl/community/discussions) |
| Forum archive (history) | [https://opendcl.github.io/forum-archive/](https://opendcl.github.io/forum-archive/) |
| Online help | [https://opendcl.github.io/HelpFiles/](https://opendcl.github.io/HelpFiles/) or [https://www.opendcl.com/HelpFiles/](https://www.opendcl.com/HelpFiles/) |
| Releases (installers) | [GitHub Releases](https://github.com/opendcl/OpenDCL/releases) (when published) |
| Historical project | [SourceForge OpenDCL](https://sourceforge.net/projects/opendcl/) |

## License

OpenDCL is free software under the **GNU General Public License version 2 or later**. See [LICENSE](LICENSE).

Copyright © 2007–2026 OpenDCL Consortium. OpenDCL is a trademark of OpenDCL Consortium.

OpenDCL is based on ObjectDCL (3rd Day Software), released as open source under the GPL in 2006 by Chad Wanless.

## Repository layout

```text
OpenDCL.sln          Visual Studio solution
Common/              Shared dialog/control core
Library/             Bundled third-party code (LibPNG, ZLib)
Runtime/             CAD runtime modules (ARX / BRX / GRX / ZRX variants)
Studio/              OpenDCL Studio (dialog editor) + localized help content
```

## Building

**Supported path:** CMake presets (Visual Studio 2022 generator). Step-by-step for
a first build: **[docs/BUILD-QUICKSTART.md](docs/BUILD-QUICKSTART.md)**. Framework
reference: **[CMAKE.md](CMAKE.md)**.

**Typical requirements:**

- CMake ≥ 3.24
- Visual Studio 2022 with Windows desktop C++ and **MFC**
- ObjectARX / BRX / ZRX / GRX SDKs for any CAD Runtime modules you build (not in this repository); Studio-only builds need no CAD SDK

```powershell
# Studio only (no CAD SDK):
cmake --preset vs2022-x86-studio
cmake --build --preset vs2022-x86-studio-debug

# Dev default: latest modern SDK per family (set any ARX*/BRX*/GRX*/ZRX* roots;
# missing families omitted; Studio still builds with zero SDKs):
cmake --preset vs2022-x64-dev
cmake --build --preset vs2022-x64-dev-debug
```

Open the generated solution under `build/<preset>/<preset>.sln` (for example
`build/vs2022-x64-dev/vs2022-x64-dev.sln`).

Classic `OpenDCL.sln` remains for historical / transitional builds.

**Older Platform Toolsets:** historic matrix rows use pre-VS2015 toolsets
(`v70`–`v120`, and related XP variants). Install
**[Daffodil](https://github.com/manusoft-gh/manusoft-daffodil)** so MSBuild can
resolve those toolsets under a current VS host. Modern presets (`v141`+) do not
need Daffodil.

There is no public CI build that produces official installers from this tree alone.

## Contributing

- Discuss product topics in [OpenDCL community Discussions](https://github.com/opendcl/community/discussions).
- Historic forum threads: [forum archive](https://opendcl.github.io/forum-archive/).
- Prefer pull requests against `main` once the GitHub project is active.

## History

This Git repository was imported from the SourceForge Subversion trunk  
`https://svn.code.sf.net/p/opendcl/code/Trunk` (full history through r411).
