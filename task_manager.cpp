#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <regex>
#include <algorithm> // Added for std::remove

using namespace std;

// User Role Enum
enum class Role { Admin, Member };

// Task Category Enum
enum class Category { Work, Personal, Study, Other };

// Task Class
class Task {
public:
    int id;
    string description;
    bool completed;
    Category category;
    string due_date; // YYYY-MM-DD
    int assigned_to; // User ID
    string priority;

    Task(int id, string desc, bool comp, Category cat, string due, int assigned, string pri)
        : id(id), description(desc), completed(comp), category(cat), due_date(due), assigned_to(assigned), priority(pri) {}
};

// User Class
class User {
public:
    int id;
    string name;
    Role role;

    User(int id, string name, Role role) : id(id), name(name), role(role) {}
};

// TaskManager Class
class TaskManager {
private:
    vector<Task> tasks;
    vector<User> users;
    SQLite::Database db;
    ofstream log_file;

    // Current time for logging
    string currentTime() {
        time_t now = time(0);
        string dt = ctime(&now);
        return dt.substr(0, dt.size() - 1);
    }

    // Load tasks from SQLite
    void loadTasks() {
        tasks.clear();
        SQLite::Statement query(db, "SELECT * FROM tasks");
        while (query.executeStep()) {
            int id = query.getColumn(0).getInt();
            string desc = query.getColumn(1).getText();
            bool comp = query.getColumn(2).getInt();
            string cat_str = query.getColumn(3).getText();
            string due = query.getColumn(4).getText();
            int assigned = query.getColumn(5).getInt();
            string pri = query.getColumn(6).getText();
            Category cat = Category::Other;
            if (cat_str == "Work") cat = Category::Work;
            else if (cat_str == "Personal") cat = Category::Personal;
            else if (cat_str == "Study") cat = Category::Study;
            tasks.emplace_back(id, desc, comp, cat, due, assigned, pri);
        }
    }

    // Load users from SQLite
    void loadUsers() {
        users.clear();
        SQLite::Statement query(db, "SELECT * FROM users");
        while (query.executeStep()) {
            int id = query.getColumn(0).getInt();
            string name = query.getColumn(1).getText();
            string role_str = query.getColumn(2).getText();
            Role role = (role_str == "Admin") ? Role::Admin : Role::Member;
            users.emplace_back(id, name, role);
        }
    }

public:
    TaskManager() : db("tasks.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE), log_file("task_manager.log", ios::app) {
        // Initialize database tables
        db.exec("CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY, description TEXT, completed INTEGER, category TEXT, due_date TEXT, assigned_to INTEGER, priority TEXT)");
        db.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, role TEXT)");
        loadTasks();
        loadUsers();
        // Add default admin user if none exist
        if (users.empty()) {
            addUser("Admin User", Role::Admin);
        }
        log_file << "Database initialized at " << currentTime() << endl;
    }

    // Add user
    void addUser(const string& name, Role role) {
        SQLite::Statement query(db, "INSERT INTO users (name, role) VALUES (?, ?)");
        query.bind(1, name);
        query.bind(2, role == Role::Admin ? "Admin" : "Member");
        query.exec();
        loadUsers();
        log_file << "User added: " << name << " at " << currentTime() << endl;
    }

    // Add task
    void addTask(const string& desc, Category cat, const string& due, int assigned, const string& pri, int current_user_id) {
        if (desc.empty()) throw runtime_error("Description cannot be empty");
        if (!regex_match(due, regex("\\d{4}-\\d{2}-\\d{2}"))) throw runtime_error("Invalid date format");
        bool user_exists = false;
        for (const auto& user : users) {
            if (user.id == assigned) {
                user_exists = true;
                break;
            }
        }
        if (!user_exists) throw runtime_error("Assigned user not found");
        SQLite::Statement query(db, "INSERT INTO tasks (description, completed, category, due_date, assigned_to, priority) VALUES (?, 0, ?, ?, ?, ?)");
        query.bind(1, desc);
        string cat_str = (cat == Category::Work) ? "Work" : (cat == Category::Personal) ? "Personal" : (cat == Category::Study) ? "Study" : "Other";
        query.bind(2, cat_str);
        query.bind(3, due);
        query.bind(4, assigned);
        query.bind(5, pri);
        query.exec();
        loadTasks();
        log_file << "Task added: " << desc << " by user " << current_user_id << " at " << currentTime() << endl;
    }

    // View tasks
    void viewTasks(int current_user_id) {
        if (tasks.empty()) {
            cout << "No tasks found." << endl;
            return;
        }
        cout << left << setw(5) << "ID" << setw(30) << "Description" << setw(10) << "Status"
             << setw(10) << "Category" << setw(15) << "Due Date" << setw(10) << "Assigned" << setw(10) << "Priority" << endl;
        cout << string(90, '-') << endl;
        for (const auto& task : tasks) {
            bool is_admin = false;
            for (const auto& user : users) {
                if (user.id == current_user_id && user.role == Role::Admin) {
                    is_admin = true;
                    break;
                }
            }
            if (is_admin || task.assigned_to == current_user_id) {
                string assignee = "Unknown";
                for (const auto& user : users) {
                    if (user.id == task.assigned_to) {
                        assignee = user.name;
                        break;
                    }
                }
                cout << left << setw(5) << task.id << setw(30) << task.description.substr(0, 29)
                     << setw(10) << (task.completed ? "Completed" : "Pending")
                     << setw(10) << (task.category == Category::Work ? "Work" : task.category == Category::Personal ? "Personal" : task.category == Category::Study ? "Study" : "Other")
                     << setw(15) << task.due_date << setw(10) << assignee << setw(10) << task.priority << endl;
            }
        }
    }

    // Export to iCalendar
    void exportToICalendar() {
        ofstream ics("tasks.ics");
        ics << "BEGIN:VCALENDAR\nVERSION:2.0\nPRODID:-//TaskManagerCLI//EN\n";
        for (const auto& task : tasks) {
            if (!task.completed) {
                string dt = task.due_date;
                // Remove hyphens from YYYY-MM-DD to get YYYYMMDD
                dt.erase(remove(dt.begin(), dt.end(), '-'), dt.end());
                ics << "BEGIN:VEVENT\nUID:" << task.id << "@taskmanagercli\n"
                    << "DTSTART:" << dt << "T000000\n"
                    << "SUMMARY:" << task.description << "\n"
                    << "END:VEVENT\n";
            }
        }
        ics << "END:VCALENDAR\n";
        ics.close();
        log_file << "Tasks exported to tasks.ics at " << currentTime() << endl;
    }

    // Check due tasks (simulated scheduler)
    void checkDueTasks() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char today[11];
        strftime(today, 11, "%Y-%m-%d", ltm);
        for (const auto& task : tasks) {
            if (!task.completed && task.due_date == today) {
                cout << "Reminder: Task '" << task.description << "' is due today!" << endl;
                log_file << "Reminder sent for task " << task.id << " at " << currentTime() << endl;
            }
        }
    }
};

// Main function with simulated scheduler
int main() {
    try {
        TaskManager tm;
        int current_user_id = 1; // Default admin user
        thread scheduler([&tm]() {
            while (true) {
                tm.checkDueTasks();
                this_thread::sleep_for(chrono::seconds(60));
            }
        });
        scheduler.detach();

        int choice;
        while (true) {
            cout << "\nTask Manager CLI\n";
            cout << "1. Add Task\n2. View Tasks\n3. Add User\n4. Export to iCalendar\n5. Exit\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) {
                cout << "Invalid input." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore();

            if (choice == 5) break;

            switch (choice) {
                case 1: {
                    string desc, due, pri;
                    int cat_choice, assigned;
                    cout << "Enter description: ";
                    getline(cin, desc);
                    cout << "Select category (1=Work, 2=Personal, 3=Study, 4=Other): ";
                    cin >> cat_choice;
                    cin.ignore();
                    Category cat = (cat_choice == 1) ? Category::Work : (cat_choice == 2) ? Category::Personal : (cat_choice == 3) ? Category::Study : Category::Other;
                    cout << "Enter due date (YYYY-MM-DD): ";
                    getline(cin, due);
                    cout << "Enter priority (High/Medium/Low): ";
                    getline(cin, pri);
                    cout << "Assign to user ID: ";
                    cin >> assigned;
                    cin.ignore();
                    tm.addTask(desc, cat, due, assigned, pri, current_user_id);
                    break;
                }
                case 2:
                    tm.viewTasks(current_user_id);
                    break;
                case 3: {
                    string name;
                    int role_choice;
                    cout << "Enter user name: ";
                    getline(cin, name);
                    cout << "Select role (1=Admin, 2=Member): ";
                    cin >> role_choice;
                    cin.ignore();
                    tm.addUser(name, role_choice == 1 ? Role::Admin : Role::Member);
                    break;
                }
                case 4:
                    tm.exportToICalendar();
                    cout << "Tasks exported to tasks.ics" << endl;
                    break;
                default:
                    cout << "Invalid choice." << endl;
            }
        }
    } catch (const SQLite::Exception& e) {
        cerr << "SQLite error: " << e.what() << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}