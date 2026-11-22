# Hospital Emergency and Resource Management

Level 2 prototype for CSC241OOP (BEE-3A, Fall 2026). A classroom simulation, not a clinical system. Use fictional aliases and manually selected demonstration priorities only.

**Repository placeholder:** `https://github.com/YOUR-USERNAME/hospital-emergency-resource-management`

- [Level 2 report](docs/Level-2-Report.docx)
- [UML class diagram](docs/class-diagram.png) and [editable Mermaid source](docs/class-diagram.mmd)

## Build and run on Windows

Install a C++17 compiler, CMake 3.20 or later, and Ninja. When using MSVC, open an **x64 Native Tools Command Prompt** with the Visual Studio C++ desktop workload installed. From the project folder:

```bat
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
build\hospital_demo.exe
```

The console front end uses the C++ standard library. No SFML, separate runtime library, database, font, or network connection is needed. With Ninja and a C++17 compiler on Linux/macOS, use the same CMake commands and run `./build/hospital_demo`.

## Walkthrough

1. Register `Demo-A` with priority `1`, then `Demo-B` and `Demo-C` with priority `3`.
2. Choose Status. Patient `2` is first: higher priority wins, and ties favor earlier registration.
3. Assign Bed `101` and Equipment `201` to patient `2`. Status now lists patient `3` first. Attempting to reassign occupied Bed `101` is rejected.
4. Release `101` and check that patient `3` remains first. Release `201`; patient `2` is eligible again because they now hold no resources.
5. Export to `snapshot.txt` in the current working directory; the file is a display export, not a reloadable save. Exit via `0`.

The operator manually chooses the patient and resource. The waiting recommendation does not allocate anything. This milestone does not implement discharge, sign-in, persistent sessions, or a graphical interface.

## Source map

| Path | Role |
|---|---|
| `include/Hospital.hpp`, `src/Hospital.cpp` | Model, assignment rules, report, and file export |
| `include/ConsoleApp.hpp`, `src/ConsoleApp.cpp`, `src/main.cpp` | Menu, validation messages, and entry point |
| `tests/hospital_tests.cpp` | Automated model checks registered with CTest |
| `docs/` | Submission report and UML artifacts |

## Publish your own copy

Copy only this project folder to a standalone location outside any unrelated Git repository. Fill in the team details and replace the repository placeholder in this README and the report before submitting. Create an empty GitHub repository (no generated README, license, or `.gitignore`), then from the standalone project root run:

```bat
git init -b main
git add .
git status
git commit -m "Add Level 2 hospital management prototype"
git remote add origin https://github.com/YOUR-USERNAME/hospital-emergency-resource-management.git
git push -u origin main
```

Check the repository link from the instructor's access context. Team members should commit their own actual work and follow the course's attribution requirements. GitHub hosts the code; run the desktop prototype locally.
