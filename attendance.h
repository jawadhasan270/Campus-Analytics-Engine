#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>
using namespace std;

extern const string ATTENDANCE_FILE;

// Mark attendance for all enrolled students in a course on a given date
void markAttendance(const string& courseCode, const string& date, const string& semester);

// Compute attendance percentage: (P + 0.5*L) / total * 100
double getAttendancePct(const string& roll, const string& courseCode);

// Get list of students with attendance < 75% in a course
vector<vector<string>> getShortageList(const string& courseCode);

// Undo last session (restore from backup)
bool undoLastSession(const string& courseCode, const string& date);

// Print daily attendance sheet for a course and date
void printDailySheet(const string& courseCode, const string& date);

#endif
