Task Manager CLI

Description

A sophisticated C++17 command-line application for collaborative task management, featuring task creation, viewing, assignment, and iCalendar export, with persistent storage in a SQLite database. Includes user roles (Admin/Member), task categories, due dates, priorities, and a cron-like scheduler for reminders. Designed for real-world productivity, integrating with calendar apps and mimicking enterprise tools like Trello. Showcases advanced C++ skills, database management, and system design.

Features





Manage tasks with descriptions, categories (Work, Personal, Study), due dates, priorities, and user assignments.



Support multiple users with Admin/Member roles for collaborative workflows.



Store data in a SQLite database for scalability.



Export tasks to iCalendar (.ics) for integration with Google Calendar.



Automated due task reminders via a cron-like scheduler.



Robust error handling with file-based logging.



Table-based CLI display for enhanced usability.

Prerequisites





C++ Compiler: g++ (GCC) with C++17 support.





Mac: xcode-select --install or brew install gcc.



Linux: sudo apt install g++.



Windows: MinGW via MSYS2 (pacman -S mingw-w64-x86_64-gcc).



CMake: brew install cmake (Mac), sudo apt install cmake (Linux), or download from cmake.org (Windows).



SQLite: brew install sqlite (Mac), sudo apt install libsqlite3-dev (Linux), or download from sqlite.org (Windows).



Git: git-scm.com.

Setup Instructions





Create project folder:

mkdir Task-Manager-CLI
cd Task-Manager-CLI



Initialize Git:

git init
git remote add origin https://github.com/kwesi-owusuofori/Task-Manager-CLI.git



Install dependencies:

mkdir include
curl -L https://www.sqlite.org/2023/sqlite-amalgamation-3410200.zip -o sqlite.zip
unzip sqlite.zip
mv sqlite-amalgamation-3410200/sqlite3.c include/
mv sqlite-amalgamation-3410200/sqlite3.h include/
rm -rf sqlite.zip sqlite-amalgamation-3410200



Add source files:





Save task_manager.cpp, CMakeLists.txt, and README.md.



Ensure include/sqlite3.c and include/sqlite3.h are present.



Build:

mkdir build
cd build
cmake ..
cmake --build .



Run:

./task_manager  # Mac/Linux
task_manager.exe  # Windows

Sample Usage

Task Manager CLI
1. Add Task
2. View Tasks
3. Add User
4. Export to iCalendar
5. Exit
Enter choice: 3
Enter user name: Alice
Select role (1=Admin, 2=Member): 2
Enter choice: 1
Enter description: Team meeting
Select category (1=Work, 2=Personal, 3=Study, 4=Other): 1
Enter due date (YYYY-MM-DD): 2025-06-05
Enter priority (High/Medium/Low): High
Assign to user ID: 2

Output (tasks.ics):

BEGIN:VCALENDAR
VERSION:2.0
PRODID:-//TaskManagerCLI//EN
BEGIN:VEVENT
UID:1@taskmanagercli
DTSTART:20250605T000000
SUMMARY:Team meeting
END:VEVENT
END:VCALENDAR

Troubleshooting





SQLite Errors:





Verify include/sqlite3.c and include/sqlite3.h.



Install SQLite: brew install sqlite (Mac).



Compilation Issues:





Ensure C++17: g++ --version.



Rebuild: rm -rf build && mkdir build && cd build && cmake .. && cmake --build ..



No tasks.db:





Run the program to create the database.



Check permissions: chmod +w tasks.db.

Notes





Real-World Use: Ideal for team projects, personal productivity, or small businesses.



Portfolio: Demonstrates C++17, SQLite, OOP, and system design.



Portfolio: Add this project to your CV to highlight C++ skills.

If you need help or encounter errors, share the error message or output, and I’ll provide a tailored fix immediately.
