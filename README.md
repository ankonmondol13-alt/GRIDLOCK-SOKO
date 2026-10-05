# GRIDLOCK7 - Sokoban

> A 7-level Sokoban puzzle game written in **C** using the **raylib** library.
> Push every crate onto a glowing goal tile. Crates can be pushed, never pulled, and there is no undo.

**Team (BUET CSE):** Ankon Mandal and Sayon Basak

---

## Table of Contents

1. [Overview](#1-overview)
2. [Requirements](#2-requirements)
3. [Project Structure](#3-project-structure)
4. [Setup and Compilation](#4-setup-and-compilation)
5. [Running the Game](#5-running-the-game)
6. [How to Play](#6-how-to-play)
7. [Levels](#7-levels)
8. [Configuration Notes](#8-configuration-notes)
9. [Troubleshooting](#9-troubleshooting)
10. [Credits](#10-credits)

---

## 1. Overview

| Feature | Description |
|---|---|
| Levels | 7 hand-designed levels, each with a hint |
| Menus | Intro screen, main menu, level select, help, leaderboard, About Us |
| Records | Name entry before each run; best time per level is saved |
| Smart checks | Detects an unwinnable state (stuck crate) and asks you to restart |
| Audio / Visuals | Background music, menu background, textured map |

---

## 2. Requirements

| Item | Requirement |
|---|---|
| Operating system | Windows 10 / 11 (64-bit) |
| Compiler | GCC for Windows, 64-bit (MinGW-w64 / w64devkit) |
| Library | [raylib](https://www.raylib.com/) 5.0 or newer |
| Editor (optional) | Visual Studio Code with the raylib template |
| Windows libraries | `opengl32`, `gdi32`, `winmm` (included with Windows / MinGW) |

No other dependencies are needed.

---

## 3. Project Structure

| File | Purpose |
|---|---|
| `main.c` | Game logic, levels, rendering, input, music |
| `menu.c`, `menu.h` | Intro, main menu, level select, name entry, leaderboard, help, About Us |
| `menu_bg.png` | Menu background image |
| `sokoban_intro.png` | Intro screen image |
| `sokoban_map_texture.png` | Map texture |
| `ankon.png`, `sayon.png` | Photos on the About Us screen |
| `soko.mp3.mpeg.wav` | Background music |
| `gridlock7_records.txt` | Saved best times (created automatically if missing) |
| `raylib_template.code-workspace` | VS Code workspace file |

> **Important:** keep all image and audio files in the **same folder as the compiled program** (`main.exe`).

---

## 4. Setup and Compilation

### Step 1 - Install the tools
1. Download and run the **raylib Windows installer** from <https://www.raylib.com/>.
   It installs raylib and the compiler (w64devkit), by default in `C:\raylib`.
2. (Optional) Install Visual Studio Code.

### Step 2 - Prepare the project
1. Extract the submitted ZIP into a folder.
2. Make sure every file listed in Section 3 is in that folder.

### Step 3 - Compile (choose one option)

**Option A - VS Code (easiest)**
1. Open the folder in VS Code (or open `raylib_template.code-workspace`).
2. Open `main.c`.
3. Press **Ctrl + Shift + B** to build, or **F5** to build and run.
4. `main.exe` is created in the project folder.

**Option B - Command line**

Open a terminal in the project folder and run:

```
gcc main.c menu.c -o main.exe -I C:/raylib/raylib/src -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
```

If your raylib has separate `include` and `lib` folders, use:

```
gcc main.c menu.c -o main.exe -I <raylib>/include -L <raylib>/lib -lraylib -lopengl32 -lgdi32 -lwinmm
```

> Both `main.c` **and** `menu.c` must be compiled together.

---

## 5. Running the Game

1. Check that the images and music file are next to `main.exe`.
2. Double-click `main.exe`, or run `main.exe` in a terminal.
3. Click **PLAY**, choose a level, type your name, then press `ENTER` to start.

Window size is fixed at **832 x 714** pixels.

---

## 6. How to Play

**Goal:** push all crates onto the glowing green goal tiles as fast as you can.

**Rules**
- Crates can only be pushed, never pulled.
- There is no undo. Restart with `R` if you get stuck.
- Your time is recorded; beat the record to top the leaderboard.

**Controls**

| Key / Input | Action |
|---|---|
| `W A S D` / Arrow keys | Move and push crates |
| `R` | Restart the current level |
| `ENTER` | Go to the next level (after clearing a level) |
| `ESC` | Return to the menu / go back |
| Mouse click | Select menu and level buttons |
| `ENTER` (menus) | Confirm / start |
| `BACKSPACE` | Delete a letter while typing your name |

---

## 7. Levels

| # | Name |
|---|---|
| 1 | 01 // EASY? LOL. |
| 2 | 02 // THREE'S COMPANY. |
| 3 | 03 // TIGHT CORNERS. |
| 4 | 04 // BOXED IN. |
| 5 | 05 // GOOD LUCK, BESTIE. |
| 6 | 06 // CHAOS ROOM. |
| 7 | 07 // THE OLD TRAP. |

---

## 8. Configuration Notes

- **Music file name:** the code loads exactly `soko.mp3.mpeg.wav`. If renamed, the music will not play (the game still runs).
- **Records file:** best times are stored in `gridlock7_records.txt` in the program folder. Delete it to reset all records.
- **No executable included:** this submission contains only source code and assets. Compile it using Section 4.

---

## 9. Troubleshooting

| Problem | Fix |
|---|---|
| Black or empty background | An image is missing or not in the same folder as `main.exe` |
| No music | Check the file is named exactly `soko.mp3.mpeg.wav` |
| About Us photo frame shows "PHOTO" | `ankon.png` / `sayon.png` is missing |
| `raylib.h` not found when compiling | Fix the `-I` path to your raylib folder |
| Undefined reference errors when linking | Add `-lraylib -lopengl32 -lgdi32 -lwinmm` and compile both `.c` files |
| Windows SmartScreen warning | The exe is unsigned: click *More info -> Run anyway* |

---

## 10. Credits

- Game design and code: **Ankon Mandal** and **Sayon Basak** (BUET CSE)
- Graphics library: [raylib](https://www.raylib.com/) by Ramon Santamaria (zlib/libpng license)
