# Timetable Generator Portal

A C++ timetable generation project with a lightweight web frontend for viewing schedules section-wise, teacher-wise, and room-wise.

## Overview

This project has two main parts:

- `C++ backend`: generates the timetable from CSV input files
- `web frontend`: displays the generated timetable in a login-based portal

The backend reads teachers, subjects, and rooms from CSV files, generates a schedule, checks constraints, and exports timetable CSV files. The frontend reads those exported CSV files and renders them in interactive pages.

## Project Structure

```text
timetable_generator/
├── data/                 # Input CSV files
├── include/              # Header files
├── src/                  # C++ source files
├── web/                  # Frontend HTML, CSS, JS
├── public/assets/        # Images and static assets
├── main.cpp              # Backend entry point
├── Makefile              # Build and run commands
├── timetable_cse2yr      # Compiled backend executable
└── output_*.csv          # Generated timetable exports
```

## Backend Features

- Reads input from:
  - `data/teachers.csv`
  - `data/subjects.csv`
  - `data/rooms.csv`
- Builds a conflict graph
- Assigns time slots
- Assigns rooms
- Checks hard and soft constraints
- Optimizes the generated timetable
- Exports:
  - full schedule
  - section-wise files
  - teacher-wise file

## Frontend Features

- Login page with background campus image
- Faded team name at the bottom
- Home dashboard with:
  - Section Wise Timetable
  - Teacher Wise Timetable
  - Room Wise Timetable
- Subject-wise color boxes across timetable views
- Static-file-compatible frontend for VS Code Go Live

## Login Credentials

### Admin

- Username: `Geu`
- Password: `Geu-admin`

### Students

Registered student IDs:

- `240211785`
- `240211789`
- `240260491`

Password for all student IDs:

- `Timer@123`

## Current Sections

- `A`
- `B`
- `C`
- `D`
- `E`
- `F`
- `ML1`
- `ML2`

## How To Build And Run

### 1. Build the backend

```bash
make all
```

### 2. Run the backend

```bash
make run
```

Inside the C++ program:

- Press `5` to export the timetable CSV files
- Press `Q` to quit

This generates files such as:

- `output_latest_schedule.csv`
- `output_latest_teachers.csv`
- `output_latest_section_A.csv`
- `output_latest_section_ML1.csv`
- `output_latest_section_ML2.csv`

## Frontend Usage

### Option 1: VS Code Go Live

Open this file:

- `web/index.html`

Then start Go Live. The frontend pages are connected with relative paths and should work in static mode.

### Option 2: Local Node server

```bash
make web
```

Then open:

- `http://127.0.0.1:3000`

## Frontend Data Flow

The frontend reads timetable data from exported CSV files:

- `output_latest_schedule.csv`
- `output_latest_teachers.csv`
- `output_latest_section_*.csv`

Fallback support also exists for:

- `output_v1_schedule.csv`
- `output_v1_teachers.csv`
- `output_v1_section_*.csv`

If timetable data is missing in the website:

1. Run `make run`
2. Export with option `5`
3. Refresh the browser

## Important Files

- `main.cpp`: backend entry point
- `src/scheduler.cpp`: slot and room assignment
- `src/checker.cpp`: constraint validation
- `src/optimizer.cpp`: timetable improvement
- `src/output_module.cpp`: CSV export logic
- `web/index.html`: login page
- `web/dashboard.html`: main dashboard
- `web/viewer.html`: timetable viewer
- `web/app.js`: frontend logic and timetable rendering
- `web/auth.js`: login/authentication logic
- `web/styles.css`: UI styling

## Notes

- The current authentication is frontend-based and suitable for demo/prototype use.
- For a production version, authentication should move to a secure backend API.
- Generated CSV files are part of the current frontend workflow.

## Clean Build Outputs

```bash
make clean
```

This removes:

- `timetable_cse2yr`
- `output_v*.csv`
- `output_latest*.csv`
