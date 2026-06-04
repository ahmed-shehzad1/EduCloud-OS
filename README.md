<div align="center">
  <br />
  <h1>☁️ EduCloud OS</h1>
  <p><b>A C++17 & Qt-powered Mock Operating System Shell Environment</b></p>

  [![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
  [![Framework](https://img.shields.io/badge/Framework-Qt5%20%2F%20Qt6-green.svg)](https://www.qt.io/)
  [![Build](https://img.shields.io/badge/Build-CMake-orange.svg)](https://cmake.org/)
  [![License](https://img.shields.io/badge/License-MIT-purple.svg)](https://opensource.org/licenses/MIT)
</div>

---

## 📝 Project Overview

**EduCloud OS** is a comprehensive full-screen desktop environment and operating system simulation. Built strictly with modern **C++17** and the **Qt Framework**, it maps abstract theoretical computer science concepts—such as process control blocks, CPU scheduling, execution loops, and virtual memory maps—into a highly interactive, graphically responsive, and beautifully styled software ecosystem.

---

## 🚀 Exhaustive Core Feature breakdown

### 🖥️ 1. Modern Desktop Shell & UX Environment
* **MDI Windowing Infrastructure:** Powered by `QMdiArea`, allowing multiple application instances to open concurrently. Windows can be moved, resized, maximized, or layered seamlessly over an event-driven system architecture.
* **Persistent Taskbar Component:** Houses an integrated global application menu controller, tracking hooks for open application cycles, and a localized hardware power control mechanism.
* **Asynchronous Threaded Clock:** Updates system diagnostics and time/date values dynamically every 1,000 milliseconds using an isolated `QTimer` event looping loop.
* **Decoupled Context Menu Engine:** Implements a standalone mouse listener tracking user right-clicks across the central workspace, clearing out stale UI buffer cache allocations through localized repaint updates (`workspace->update()`).
* **Catppuccin-Inspired Theme Engine:** Handcrafted, global CSS stylesheet injections mapping high-contrast text inputs, uniform dark layout backdrops, and modern border-radius rounded elements.

### ⚙️ 2. Task Orchestrator (CPU Scheduler Simulation)
* **Algorithmic Simulation Matrices:** Models real-world operating system scheduling paradigms (including First-Come First-Served, Shortest Job First, and Priority Scheduling profiles).
* **Dynamic PCB Instantiation:** Allows direct generation of custom mock tasks via specific parameters including Burst Time, Execution Priorities, and Arrival sequences.
* **Visual State Progression:** Renders real-time execution status arrays mapping out color-coded process progressions through ready, running, and terminated states.

### 📊 3. System Task Manager
* **Process Event Logging Arrays:** Provides a live diagnostics table displaying active thread IDs, resource footprint representations, and parent window lifecycles.
* **Asynchronous Thread Destruction:** Integrates safe process termination hooks (`Qt::WA_DeleteOnClose`), enabling clean heap deallocations of software applications directly from the monitoring grid.

### 📁 4. Sandboxed File Explorer (Virtual FS)
* **In-Memory Storage Sandbox:** Structures system storage using optimized key-value map caches (`QMap<QString, QString>`) to prevent hazardous mutations on the host computer's hard drive.
* **Virtual Data Persistence APIs:** Features full operational integrity for creating, saving, opening, reading, and indexing text documents across simulated storage directories.

### 🌐 5. Virtual Web Browser Explorer
* **Localized Network Routing Logic:** Circumvents compilation restrictions of Chromium engines under standard MinGW environments via custom string-parsing protocols.
* **On-the-Fly HTML Streaming:** Synthesizes custom-styled HTML layouts dynamically based on incoming URLs, mimicking real-world website pathways.
* **Dynamic Google Search Generator:** Intercepts unrecognized queries inputted into the address bar to dynamically assemble customized, mock search results tailored to whatever the user types.
* **Interactive Browsing State Control:** Includes active toolbar utilities for back-tracking, front-tracking, homepage redirection, and frame clearing.

### 🧮 6. Integrated Utilities (Calculator & Snake Game)
* **Mathematical Parser Core:** Offers a localized grid input calculator processing execution sequences for standard arithmetic arithmetic contexts.
* **Snake Game App Block:** Implements an interactive recreation of the iconic retro game, demonstrating custom keyboard event filters (`keyPressEvent`), collision matrix grids, and high-frequency game ticking clocks.

---

## 🛠️ Technology Stack & Dependencies

```yaml
Programming Language: C++17 Standard
GUI Framework:        Qt Framework (Qt6 or Qt5)
Build Automater:      CMake (Minimum Version 3.16)
Compiler Engine:      MinGW / GCC Core Execution Pipelines
Operating Systems:    Windows, Linux, macOS
