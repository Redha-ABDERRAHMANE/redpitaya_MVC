# redpitaya_GUI

## Dependencies Overview

This project relies on several third-party libraries. Some are provided directly in the repository, others must be sourced and installed by the user.

---

## Qt6

**No manual DLL setup required if using Visual Studio with the Qt VS Tools extension.**

If you open this project in Visual Studio with the Qt Visual Studio Tools extension installed and a Qt6 kit configured, Qt will automatically locate and use the correct DLLs at runtime. You do not need to copy any Qt DLLs into the project manually.

If you do not use the extension, you will need to add the Qt `bin` directory to your system PATH or copy the required DLLs next to the executable manually.

Download Qt: https://www.qt.io/download-open-source  
Qt VS Tools extension: https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools2022

---

## SDL3 — Provided in this Repository

SDL3 binaries are included directly in this repository under the `external/SDL3/` folder.

> **Note:** I did not write SDL3. It is provided here unmodified, solely for convenience, under the terms of the **zlib license**. Full license text is available in `LICENSE.txt` and at https://www.libsdl.org.

No additional setup is needed for SDL3.

---

## Thorlabs SDK — Must Be Sourced by the User

The Thorlabs SDK is **not included** in this repository due to its proprietary license.

You must download the appropriate SDK from Thorlabs and place the required files as follows:

- Import libraries (`.lib`) → `external/Thorlabs_dependencies/lib/`
- Runtime libraries (`.dll`) → project root folder (next to the `.vcxproj`)

Download: https://www.thorlabs.com (navigate to your device's software page)

---

## Vimba X SDK — Must Be Sourced by the User

The Vimba X SDK is **not included** in this repository due to its proprietary license.

You must install the Vimba X SDK from Allied Vision. The include directories are picked up automatically from the default Vimba X installation path — no manual header setup is needed.

Place the required files as follows:

- Import libraries (`.lib`) → `external/VimbaX_dependencies/lib/`
- Runtime libraries (`.dll`) → project root folder (next to the `.vcxproj`)

Download: https://www.alliedvision.com/en/products/software/vimba-x-sdk/

---

## Adding New Dependencies

If you want to add additional libraries to the project, follow this convention:

1. Place the dependency files as follows:
   - Headers → `external/YourLibrary/include/`
   - Import libraries → `external/YourLibrary/lib/`
   - DLLs → project root folder (next to the `.vcxproj`)

2. In Visual Studio, open the project properties and add the paths:

**Linker → General → Additional Library Directories**
```
$(SolutionDir)external\YourLibrary\lib
```

**Linker → Input → Additional Dependencies**
```
YourLibrary.lib
```

**C/C++ → General → Additional Include Directories** (if needed for headers)
```
$(SolutionDir)external\YourLibrary\include
```

Always use `$(SolutionDir)` as the base path instead of absolute paths so the project works on any machine that clones the repository.

---

## Repository Structure

```
redpitaya_GUI/
├── external/
│   ├── SDL3/                        ← provided, unmodified (zlib license)
│   ├── Thorlabs_dependencies/
│   │   └── lib/                     ← must be sourced by user
│   └── VimbaX_dependencies/
│       └── lib/                     ← must be sourced by user
├── *.dll                            ← Thorlabs and Vimba X runtime DLLs go here
├── redpitaya_GUI.vcxproj
├── redpitaya_GUI.sln
├── LICENSE.txt
└── README.md
```

---

## License

See `LICENSE.txt` for full third-party license information.