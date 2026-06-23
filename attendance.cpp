#include "attendance.h"
#include "filehandler.h"
#include "course_ops.h"
#include <iostream>
#include <iomanip>
using namespace std;

const string ATTENDANCE_FILE = "attendance_log.txt";
const string ATTENDANCE_HEADER = "roll_no,course_code,date,status";

// Backup stored in memory per session (global for simplicity)
static vector<vector<string>> attendanceBackup;
static bool backupExists = false;

void markAttendance(const string& courseCode, const string& date, const string& semester) {
    // Get enrolled students
    vector<vector<string>> enrolled = listEnrolledStudents(courseCode);
    if (enrolled.empty()) {
        cout << "  [Info] No students enrolled in " << courseCode << "\n";
        return;
    }

    // Save backup of current attendance file
    attendanceBackup = readTXT(ATTENDANCE_FILE);
    backupExists = true;

    cout << "\n--- Mark Attendance for " << courseCode << " on " << date << " ---\n";
    cout << "  Enter P (Present), A (Absent), L (Late) for each student:\n\n";

    for (int i = 0; i < (int)enrolled.size(); i++) {
        string roll = enrolled[i][1];
        string status = "";
        while (true) {
            cout << "  [" << (i+1) << "] " << roll << " -> ";
            cin >> status;
            // convert to uppercase
            for (int j = 0; j < (int)status.size(); j++) status[j] = toupper(status[j]);
            if (status == "P" || status == "A" || status == "L") break;
            cout << "  [Error] Enter P, A, or L only.\n";
        }
        vector<string> row = {roll, courseCode, date, status};
        appendTXT(ATTENDANCE_FILE, row);
    }
    cout << "  [Success] Attendance marked for " << enrolled.size() << " students.\n";
}

double getAttendancePct(const string& roll, const string& courseCode) {
    vector<vector<string>> rows = readTXT(ATTENDANCE_FILE);
    int present = 0, late = 0, total = 0;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 4) continue;
        if (rows[i][0] != roll || rows[i][1] != courseCode) continue;
        total++;
        string s = rows[i][3];
        if (s == "P") present++;
        else if (s == "L") late++;
    }
    if (total == 0) return 0.0;
    return (present + 0.5 * late) / total * 100.0;
}

vector<vector<string>> getShortageList(const string& courseCode) {
    vector<vector<string>> enrolled = listEnrolledStudents(courseCode);
    vector<vector<string>> shortage;
    for (int i = 0; i < (int)enrolled.size(); i++) {
        string roll = enrolled[i][1];
        double pct = getAttendancePct(roll, courseCode);
        if (pct < 75.0) {
            // Store roll, courseCode, pct as string
            vector<string> row;
            row.push_back(roll);
            row.push_back(courseCode);
            // double to string manually (2 decimals)
            int intPart = (int)pct;
            int fracPart = (int)((pct - intPart) * 100);
            string pctStr = "";
            if (intPart == 0) pctStr = "0";
            else { int tmp = intPart; while (tmp > 0) { pctStr = char('0'+tmp%10) + pctStr; tmp /= 10; } }
            pctStr += ".";
            if (fracPart < 10) pctStr += "0";
            int t2 = fracPart; string f = "";
            if (t2 == 0) f = "00";
            else { while (t2 > 0) { f = char('0'+t2%10) + f; t2 /= 10; } }
            pctStr += f;
            row.push_back(pctStr);
            shortage.push_back(row);
        }
    }
    return shortage;
}

bool undoLastSession(const string& courseCode, const string& date) {
    if (!backupExists) {
        cout << "  [Error] No backup available to undo.\n";
        return false;
    }
    writeTXT(ATTENDANCE_FILE, ATTENDANCE_HEADER, attendanceBackup);
    backupExists = false;
    cout << "  [Success] Last attendance session undone.\n";
    return true;
}

void printDailySheet(const string& courseCode, const string& date) {
    vector<vector<string>> rows = readTXT(ATTENDANCE_FILE);
    cout << "\n--- Daily Attendance Sheet: " << courseCode << " | Date: " << date << " ---\n";
    cout << setw(15) << left << "Roll No"
         << setw(8) << left << "Status" << "\n";
    cout << string(25, '-') << "\n";
    bool any = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 4) continue;
        if (rows[i][1] != courseCode || rows[i][2] != date) continue;
        cout << setw(15) << left << rows[i][0]
             << setw(8) << left << rows[i][3] << "\n";
        any = true;
    }
    if (!any) cout << "  [Info] No attendance records found for this date.\n";
}