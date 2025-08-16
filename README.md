# Markdown & CSV Editor (Qt/C++)

**Auteur: Gertjan Debusschere**

_Licentie: GPL-3.0-or-later_

Een eenvoudige Qt-applicatie voor het bewerken van Markdown- en CSV-bestanden.

## Functies

- Markdown-editor met live preview (optioneel)
- CSV-editor met tabelweergave en cell-level editing
- Bestandsbewerkingen: openen, opslaan, opslaan als
- Printen via QPrinter
- Zoeken/vervangen
- Sneltoetsen voor veelgebruikte acties

## Opbouw

- `src/` bevat alle bronbestanden
- CMake-project, afhankelijk van Qt6 (Widgets, PrintSupport)

## Build-instructies

```sh
cmake -B build -S .
cmake --build build
```
