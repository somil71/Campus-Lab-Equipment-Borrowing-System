# Campus Lab Equipment Borrowing System

A C++17 terminal application for a lab assistant. Add equipment, check stock, issue one unit to a student, record its return, and view active and overdue loans. SQLite is bundled as source. Building requires a C and C++ compiler. Running the built application only needs the executable and its compiler runtime in a writable working directory.

## Build and launch

Extract the ZIP first. Open a terminal inside the extracted CampusLab folder. Use the commands for your platform.

### Windows

Use the **x64 Native Tools Command Prompt for Visual Studio 2022** with the *Desktop development with C++* workload installed, or a terminal with a recent MinGW-w64 toolchain (gcc and g++) on PATH. From the project folder, run:

```powershell
.\build.bat
.\build\campus_lab.exe
```

The batch file selects MSVC when cl is available and otherwise uses MinGW-w64. After a successful build, use the same terminal environment to launch the program.

### Linux

Install GCC, G++ and the standard C development headers if needed. For Debian/Ubuntu the package is build-essential. From the project folder, run:

```sh
sh build.sh
./build/campus_lab
```

The script compiles the bundled SQLite C source first. This can take a little time. Rerun the script after editing the source. The database appears as lab_equipment.db in the directory from which you launch the program. Open the same directory to continue a previous session.

## Three-minute demonstration

Use a fresh working directory for an empty demo. Use a real due date of today or later. Follow the IDs printed by the program.

| Menu action | What the audience should see |
|---|---|
| 1. Add Arduino Kit, quantity 2. Then choose 2. | 2 total, 2 available |
| 3. Issue to roll 001, then 002. Record both loan IDs. | 2 distinct loans |
| 3. Try to issue to 003. Then choose 2. | All units are on loan. Available is 0. |
| 4. Return the first loan. Try the same return again. | First return succeeds. The second displays the earlier return date. |
| 3. Issue to 003. Choose 6, restart, then choose 2 and 5. | 0 available. Active borrowers are 002 and 003. The earlier return is kept in SQLite. |

For an overdue example, run `python demo/prepare_overdue.py` on Windows or `python3 demo/prepare_overdue.py` on Linux after building. It creates a separate demo folder and prints the absolute executable path. From that folder, run the printed executable (in PowerShell, type `& "full path to executable"`), then choose 5. The three active loans show yesterday, today, and tomorrow. Recreate this fixture on the day of the demo.

## Tests

With Python 3.8+ installed, run `python tests/acceptance.py` on Windows or `python3 tests/acceptance.py` on Linux. Tests launch the compiled C++ executable as a subprocess, feed it menu input, open its database to check results, and discard temporary databases. They do not change the main demo. Python is used only by the tests and optional demo helper.

## Source map

| Path | Purpose |
|---|---|
| src/main.cpp | Menu, prompts, input parsing, report formatting, startup and exit |
| src/lending.hpp and src/lending.cpp | Equipment and loan result types, schema, business rules, SQL and transactions |
| src/database.hpp and src/database.cpp | Connection and statement owners, parameter binding and result access |
| vendor/sqlite/ | Upstream SQLite 3.50.4 amalgamation and its source reference |
| build.sh and build.bat | Linux and Windows build entry points |
| tests/acceptance.py | Eight automated end-to-end checks |
| demo/ | Isolated overdue example |
| docs/Campus_Lab_Project_Guide.docx | Technical walkthrough, decisions, annotated queries, troubleshooting and viva preparation |
| docs/Lab_Equipment_PRD.md and docs/Lab_Equipment_SRS.md | Original product and software requirements |

## Verification

Built and tested on Debian Linux x86-64 with GCC/G++ 14.2 and the bundled SQLite. All eight checks passed there. The Windows build script is supplied for MSVC or MinGW-w64 and still needs a run on the presentation computer.
