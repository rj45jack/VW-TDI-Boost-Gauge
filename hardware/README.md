# Hardware

The Eagle projects from the original 2010 build. These are the fabrication
sources and the generated Gerbers, not a current design review.

## What is here

| Folder | Board file | Contents |
|---|---|---|
| `display_board/` | `boost gauge V1.1.brd` / `.sch` | Gerbers, drill file, CAM support |
| `cpu_sensor_board/` | `boost gauge (2) V1.2.brd` / `.sch` | Gerbers, drill file, CAM job, CAM support |

File types:

| Extension | Meaning |
|---|---|
| `.sch`, `.brd` | Eagle schematic and board sources |
| `.GTL`, `.GTO`, `.GTP`, `.GTS` | top copper, silkscreen, paste, soldermask |
| `.GBL`, `.GBO`, `.GBS` | bottom copper, silkscreen, soldermask |
| `.TXT` | Excellon drill file |
| `.dri` | drill rack |
| `.gpi` | photoplot job |
| `.pro` | Eagle CAM job |

The `b#1` and `b#2` files in the original commit were Eagle autosave backups,
not fabrication data. They have been removed, and `.gitignore` now excludes
future autosaves.

## Warning: the folder names do not match the contents

Strings inside the binary board files show the ATmega8 and MPX4250A library
parts in `display_board`, and the voltage regulator, trimmer and connectors in
`cpu_sensor_board`. That is the opposite of what the folder names suggest and
of the original README's description. The original author also wrote that he
was "not 100% sure" the files were correct.

Treat the schematics as the source of truth and verify the board you are
fabricating before you spend money on it.

## Opening the files

These are **binary Eagle 5** files, not the XML format used by Eagle 6 and
later. Open them with Eagle 7–9 or Fusion 360 Electronics. KiCad's Eagle
importer expects Eagle 6+ XML, so re-save the schematic and board from Eagle
first if you want to migrate.

Autodesk discontinued the standalone Eagle editor, but existing installs
still open version 5 files. There is no text netlist in this repository; the
connectivity lives in the binary `.sch`/`.brd` files.

## Fabricating

Use the Gerbers plus the Excellon drill file. The `.pro` file records the CAM
job that produced the outputs; it is useful if you need to regenerate them.
Check the board outline, drill sizes and soldermask against the schematic
before ordering.

## Pinout

The firmware pin map and the LCD header pinout are in
[`../docs/pinout.md`](../docs/pinout.md). The inter-board RJ45 assignment is
only in the Eagle schematic.
