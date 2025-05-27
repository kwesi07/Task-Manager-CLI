#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <json.hpp>

using json = nlohmann::json;
using namespace std;

// Task structure
struct Task {
    int id;
    string description;
    bool completed;
};

// Function to load tasks from JSON file
vector<Task> loadTasks() {
    vector<Task> tasks;
    try {
        ifstream file("tasks.json");
        if (file.is_open()) {
            json j;
            file >> j;
            for (const auto& item : j) {
                Task task;
                task.id = item["id"];
                task.description = item["description"];
                task.completed = item["completed"];
                tasks.push_back(task);
            }
            file.close();
        }
    } catch (const exception& e) {
        cout << "Error loading tasks: " << e.what() << endl;
    }
    return tasks;
}

// Function to save tasks to JSON file
void saveTasks(const vector<Task>& tasks) {
    try {
        json j = json::array();
        for (const auto& task : tasks) {
            j.push_back({
                {"id", task.id},
                {"description", task.description},
                {"completed", task.completed}
            });
        }
        ofstream file("tasks.json");
        file << j.dump(4);
        file.close();
    } catch (const exception& e) {
        cout << "Error saving tasks: " << e.what() << endl;
    }
}

// Function to add a task
void addTask(vector<Task>& tasks, const string& description) {
    int id = tasks.empty() ? 1 : tasks.back().id + 1;
    tasks.push_back({id, description, false});
    saveTasks(tasks);
    cout << "Task added successfully!" << endl;
}

// Function to view tasks
void viewTasks(const vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "No tasks found." << endl;
        return;
    }
    for (const auto& task : tasks) {
        cout << "ID: " << task.id << ", Description: " << task.description
             << ", Status: " << (task.completed ? "Completed" : "Pending") << endl;
    }
}

// Function to update a task
void updateTask(vector<Task>& tasks, int id, const string& description) {
    for (auto& task : tasks) {
        if (task.id == id) {
            task.description = description;
            saveTasks(tasks);
            cout << "Task updated successfully!" << endl;
            return;
        }
    }
    cout << "Task ID not found." << endl;
}

// Function to delete a task
void deleteTask(vector<Task>& tasks, int id) {
    auto it = remove_if(tasks.begin(), tasks.end(),
                        [id](const Task& task) { return task.id == id; });
    if (it != tasks.end()) {
        tasks.erase(it, tasks.end());
        saveTasks(tasks);
        cout << "Task deleted successfully!" << endl;
    } else {
        cout << "Task ID not found." << endl;
    }
}

// Function to mark a task as complete
void markTaskComplete(vector<Task>& tasks, int id) {
    for (auto& task : tasks) {
        if (task.id == id) {
            task.completed = true;
            saveTasks(tasks);
            cout << "Task marked as complete!" << endl;
            return;
        }
    }
    cout << "Task ID not found." << endl;
}

int main() {
    vector<Task> tasks = loadTasks();
    int choice;

    while (true) {
        cout << "\nTask Manager CLI\n";
        cout << "1. Add Task\n";
        cout << "2. View Tasks\n";
        cout << "3. Update Task\n";
        cout << "4. Delete Task\n";
        cout << "5. Mark Task Complete\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline

        if (choice == 6) break;

        switch (choice) {
            case 1: {
                string description;
                cout << "Enter task description: ";
                getline(cin, description);
                addTask(tasks, description);
                break;
            }
            case 2:
                viewTasks(tasks);
                break;
            case 3: {
                int id;
                string description;
                cout << "Enter task ID to update: ";
                cin >> id;
                cin.ignore();
                cout << "Enter new description: ";
                getline(cin, description);
                updateTask(tasks, id, description);
                break;
            }
            case 4: {
                int id;
                cout << "Enter task ID to delete: ";
                cin >> id;
                deleteTask(tasks, id);
                break;
            }
            case 5: {
                int id;
                cout << "Enter task ID to mark complete: ";
                cin >> id;
                markTaskComplete(tasks, id);
                break;
            }
            default:
                cout << "Invalid choice. Try again." << endl;
        }
    }

    return 0;
}
