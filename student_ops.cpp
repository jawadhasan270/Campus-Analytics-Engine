#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <iomanip>
using namespace std;

const string STUDENTS_FILE = "students.txt";
const string STUDENTS_HEADER = "roll_no,name,department,semester,cgpa,status";

// Validate BSAI-YY-XXX format using substr and char checks (no regex)
bool isValidRoll(const string& roll) {
    if (roll.length() != 12) return false;
    if (roll.substr(0, 5) != "BSAI-") return false;
    if (!isdigit(roll[5]) || !isdigit(roll[6])) return false;
    if (roll[7] != '-') return false;
    if (!isdigit(roll[8]) || !isdigit(roll[9]) || !isdigit(roll[10])) return false;
    return true;
}

void printStudent(const vector<string>& s) {
    if (s.size() < 6) { cout << "  [Invalid record]\n"; return; }
    cout << "  Roll   : " << s[0] << "\n";
    cout << "  Name   : " << s[1] << "\n";
    cout << "  Dept   : " << s[2] << "\n";
    cout << "  Sem    : " << s[3] << "\n";
    cout << "  CGPA   : " << s[4] << "\n";
    cout << "  Status : " << s[5] << "\n";
}

void addStudent() {
    string roll, name, dept, semStr, cgpaStr;

    cout << "\n--- Add New Student ---\n";

    while (true) {
        cout << "Enter Roll No (format BSAI-YY-XXX): ";
        cin >> roll;
        if (!isValidRoll(roll)) {
            cout << "  [Error] Invalid format. Use BSAI-YY-XXX (e.g. BSAI-23-031)\n";
            continue;
        }
        if (rowExists(STUDENTS_FILE, 0, roll)) {
            cout << "  [Error] Roll number already exists.\n";
            continue;
        }
        break;
    }

    cin.ignore();
    while (true) {
        cout << "Enter Name: ";
        getline(cin, name);
        bool hasDigit = false;
        for (int i = 0; i < (int)name.size(); i++) {
            if (isdigit(name[i])) { hasDigit = true; break; }
        }
        if (hasDigit || name.empty()) {
            cout << "  [Error] Name must not contain digits.\n";
            continue;
        }
        break;
    }

    cout << "Enter Department: ";
    getline(cin, dept);

    cout << "Enter Semester (1-8): ";
    cin >> semStr;

    while (true) {
        cout << "Enter CGPA (0.0 - 4.0): ";
        cin >> cgpaStr;
        double cgpa = 0.0;
        bool valid = true;
        bool dotSeen = false;
        for (int i = 0; i < (int)cgpaStr.size(); i++) {
            if (cgpaStr[i] == '.') { if (dotSeen) { valid = false; break; } dotSeen = true; }
            else if (!isdigit(cgpaStr[i])) { valid = false; break; }
        }
        if (valid) {
            double intPart = 0, fracPart = 0, fracDiv = 1;
            bool inFrac = false;
            for (int i = 0; i < (int)cgpaStr.size(); i++) {
                if (cgpaStr[i] == '.') { inFrac = true; }
                else if (!inFrac) { intPart = intPart * 10 + (cgpaStr[i] - '0'); }
                else { fracPart = fracPart * 10 + (cgpaStr[i] - '0'); fracDiv *= 10; }
            }
            cgpa = intPart + fracPart / fracDiv;
            if (cgpa >= 0.0 && cgpa <= 4.0) break;
        }
        cout << "  [Error] CGPA must be between 0.0 and 4.0.\n";
    }

    vector<string> newRow = {roll, name, dept, semStr, cgpaStr, "active"};
    appendTXT(STUDENTS_FILE, newRow);
    cout << "  [Success] Student added successfully.\n";
}

vector<string> searchByRoll(const string& roll) {
    return findRow(STUDENTS_FILE, 0, roll);
}

vector<vector<string>> searchByName(const string& namePart) {
    vector<vector<string>> rows = readTXT(STUDENTS_FILE);
    vector<vector<string>> results;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 2) continue;
        string haystack = rows[i][1];
        string needle = namePart;
        string h2 = "", n2 = "";
        for (int j = 0; j < (int)haystack.size(); j++) h2 += tolower(haystack[j]);
        for (int j = 0; j < (int)needle.size(); j++) n2 += tolower(needle[j]);
        bool found = false;
        if (n2.size() <= h2.size()) {
            for (int j = 0; j <= (int)h2.size() - (int)n2.size(); j++) {
                if (h2.substr(j, n2.size()) == n2) { found = true; break; }
            }
        }
        if (found) results.push_back(rows[i]);
    }
    return results;
}

void updateStudent(const string& roll, const string& field, const string& newValue) {
    vector<vector<string>> rows = readTXT(STUDENTS_FILE);
    int colIdx = -1;
    if (field == "name") colIdx = 1;
    else if (field == "department") colIdx = 2;
    else if (field == "semester") colIdx = 3;
    else if (field == "cgpa") colIdx = 4;
    else if (field == "status") colIdx = 5;

    if (colIdx == -1) {
        cout << "  [Error] Cannot update field '" << field << "'. Roll number cannot be changed.\n";
        return;
    }
    bool found = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (!rows[i].empty() && rows[i][0] == roll) {
            rows[i][colIdx] = newValue;
            found = true;
            break;
        }
    }
    if (!found) { cout << "  [Error] Student not found.\n"; return; }
    writeTXT(STUDENTS_FILE, STUDENTS_HEADER, rows);
    cout << "  [Success] Student updated.\n";
}

void softDelete(const string& roll) {
    updateStudent(roll, "status", "inactive");
    cout << "  [Info] Student marked as inactive (soft deleted).\n";
}

vector<vector<string>> listActiveStudents() {
    vector<vector<string>> rows = readTXT(STUDENTS_FILE);
    vector<vector<string>> active;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 6 && rows[i][5] == "active") {
            active.push_back(rows[i]);
        }
    }
    for (int i = 0; i < (int)active.size() - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < (int)active.size(); j++) {
            if (active[j][0] < active[minIdx][0]) minIdx = j;
        }
        if (minIdx != i) {
            vector<string> tmp = active[i];
            active[i] = active[minIdx];
            active[minIdx] = tmp;
        }
    }
    return active;
}