# OpenDCL

OpenDCL is a modern dialog and UI toolkit for AutoLISP programmers. It replaces AutoCAD’s limited DCL (Dialog Control Language) with Windows forms and controls for use in AutoCAD and compatible platforms (BricsCAD, ZWCAD, GstarCAD, and others as supported by each release).

This repository is the **source code** for OpenDCL Studio and Runtime (product
version **10.1.1.1** — see `Runtime/ARX.rc` FILEVERSION / PRODUCTVERSION).

## Links

| Resource | URL |
| --- | --- |
| Project site / downloads | [https://opendcl.github.io/](https://opendcl.github.io/) (GitHub Pages) · [https://www.opendcl.com/](https://www.opendcl.com/) |
| Forum | [https://www.opendcl.com/forum/](https://www.opendcl.com/forum/) |
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

**Requirements (typical):**

- Microsoft Visual Studio with the Windows desktop C++ / MFC workload (the solution uses a mix of Platform Toolsets; a current VS host is fine when older toolsets are available—see Daffodil below)
- ObjectARX / BRX / ZRX / GRX SDKs for the CAD platforms you intend to target (not included in this repository)
- **[Daffodil](https://github.com/manusoft-gh/manusoft-daffodil)** when building **pre-VS2015** Platform Toolsets (`v70`–`v120`, and related XP variants). Those toolsets are required by older AutoCAD / BricsCAD / GstarCAD / ZWCAD runtime modules and are **not** shipped with modern Visual Studio alone. Install Daffodil so MSBuild can resolve those Platform Toolsets under a current VS host. Targets that use VS2015+ toolsets (`v140` and later) do not need Daffodil.

Build the solution in **Release** configuration for the platform(s) you need. Exact project matrix and SDK paths depend on your local CAD SDK install layout.

There is no public CI build that produces official installers from this tree alone.

## Contributing

- Discuss product and localization topics on the [OpenDCL forums](https://www.opendcl.com/forum/).
- Source discussion board: [Source Code](https://www.opendcl.com/forum/index.php?board=4.0).
- Prefer pull requests against `main` once the GitHub project is active.

## History

This Git repository was imported from the SourceForge Subversion trunk  
`https://svn.code.sf.net/p/opendcl/code/Trunk` (full history through r411).
