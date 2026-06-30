# 💎 Bejeweled Match-3 Game

A classic **match-3+ puzzle game** built with **C++** and **Qt**.  
Swap adjacent colored gems to create lines of three or more and score points!

---

## 🎮 Features

- **Dynamic grid generation** — random colored gems on a square board
- **Click-based swapping** — select two adjacent gems to swap them
- **Match detection** — automatically detects horizontal and vertical lines of 3+ gems
- **Score tracking** — points awarded for each match
- **Cascade mechanics** — matched gems are replaced with new random colors
- **Game over detection** — alerts when no more moves are possible
- **Customizable board size** — choose your grid size at startup

---

## 🛠️ Technologies Used

- **C++** — core game logic
- **Qt6** — GUI framework (QGraphicsScene, QGraphicsView, event handling)

---

## 🚀 How to Build

### Requirements

Install Qt6 development libraries:

```bash
# Ubuntu / Debian
sudo apt install qt6-base-dev qt6-base-dev-tools

# Or use the Qt online installer for other platforms

### Compile

g++ -o bejeweled bejeweled.cpp \
    -I/usr/include/x86_64-linux-gnu/qt6 \
    -I/usr/include/x86_64-linux-gnu/qt6/QtCore \
    -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt6/QtGui \
    -lQt6Core -lQt6Widgets -lQt6Gui

### Run

./bejeweled
