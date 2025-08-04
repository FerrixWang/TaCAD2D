# Tacad2D
 
[![CMake on Windows](https://github.com/FerrixWang/Tacad2D/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/FerrixWang/Tacad2D/actions/workflows/cmake-windows.yml)

**Tacad2D: Cross-Platform 2D CAD Powered by OCCT OCAF**

> 🛠️ *Project Name Origin*:
> The name **Tacad2D** stands for **"TAG CAD in 2D"**, where "**TA**" refers to "**TAG**" (Tag-based Architecture), reflecting its use of OCAF’s label/tag-driven data structure.
> The name and architectural structure are **inspired by [MaCAD3D](https://github.com/Macad3D/Macad3D)** — an open-source 3D CAD system based on OCCT.

**Tacad2D** is a cross-platform 2D CAD application built on the [Open CASCADE Technology (OCCT)](https://dev.opencascade.org/) OCAF framework. By leveraging OCCT's label-based data model, Tacad2D provides structured, efficient, and precise 2D design tools for engineers, architects, and developers. It supports the creation of mechanical schematics, architectural layouts, circuit diagrams, and other technical drawings.

![Tacad2D](./doc/screenshot.png)

## Key Features

* **OCAF Label-Driven Design**: Uses OCCT's OCAF for organizing geometry, attributes, and relationships through hierarchical labels, enabling intuitive and scalable data management.
* **Cross-Platform Support**: Seamlessly runs on Windows, Linux, and macOS.
* **Precision 2D Modeling**: Create 2D sketches with geometric constraints, powered by a custom constraint solver (sketchSolve).
* **Modern UI**: Built with a Ribbon-based interface (SARibbonBar) and advanced docking panels (Qt Advanced Docking System) using Qt.
* **High Performance**: Optimized with OCCT’s powerful geometric kernel for fast and reliable operation.
* **Open Source & Extensible**: 100% open-source and designed for customization and community-driven improvements.

## Installation

### Prerequisites

* **OCCT**: Version 7.5.0 or later (with OCAF).
* **Qt 6** (compatible with Qt 5 via Core5Compat), with the following modules:

  * Widgets
  * OpenGLWidgets
  * Test
  * Core5Compat
* **SARibbonBar**: [SARibbonBar](https://github.com/czyt1988/SARibbon)
* **cxxopts**: [cxxopts](https://github.com/jarro2783/cxxopts)
* **Qt Advanced Docking System**: [qtadvanceddocking](https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System)
* **RTTR**: [RTTR](https://www.rttr.org/)
* **enumFlags**: Utility for enum operations.
* **sketchSolve**: Custom 2D constraint solver (bundled or external).
* **OpenGL**: System OpenGL libraries.
* **CMake**: Version 3.12 or higher.
* **C++ Compiler**: GCC, Clang, or MSVC.

### Build Instructions

```bash
git clone https://github.com/farleyrunkel/tacad2d.git
cd tacad2d
mkdir build && cd build
cmake ..
cmake --build .
./tacad2d
```

> 💡 *Note*: Make sure dependencies like OCCT and Qt are installed. You may need to set `OpenCASCADE_INCLUDE_DIR` and `OpenCASCADE_LIBRARY_DIR` in CMake manually.

## Usage

1. **Create a Project**: Launch Tacad2D and start a new 2D project.
2. **Draw Geometry**: Use intuitive tools to draw lines, arcs, and polygons with constraint support.
3. **Organize with OCAF**: Manage elements like layers, dimensions, and annotations via OCAF's label-based system.
4. **Export**: Export to standard CAD formats (e.g., DXF, STEP).

Documentation is available in the `docs/` directory.

## Contributing

We welcome contributions!

1. Fork the repo
2. Create a branch:

   ```bash
   git checkout -b feature/your-feature
   ```
3. Commit and push:

   ```bash
   git commit -m "Add your feature"
   git push origin feature/your-feature
   ```
4. Open a Pull Request.

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## Community

* **Issues**: Report bugs or request features via [GitHub Issues](https://github.com/farleyrunkel/tacad2d/issues)
* **Discussions**: Engage via [X](https://x.com/tagcad) or the OCCT forums
* **Contact**: [support@tagcad.com](mailto:support@tagcad.com)

## License

Tacad2D is licensed under the [MIT License](LICENSE). You are free to use, modify, and distribute it.

## Acknowledgments

* Built on [Open CASCADE Technology](https://dev.opencascade.org/) and its OCAF framework.
* **Inspired by [MaCAD3D](https://github.com/Macad3D/Macad3D)** — Tacad2D’s architecture closely follows MaCAD3D, which provided key design patterns and structural ideas.
* Thanks to the developers of SARibbonBar, cxxopts, Qt Advanced Docking System, RTTR, and the open-source CAD community.

