#ifndef REPORTS_H
#define REPORTS_H

#include <string>
using namespace std;

// Print merit list of active students sorted by CGPA descending
void printMeritList();

// Print all students with attendance below 75% across all courses
void printAttendanceDefaulters();

// Print all fee defaulters sorted by outstanding amount
void printFeeDefaulters();

// Print full semester result sheet for a student
void printSemesterResult(const string& roll, const string& semester);

// Print department-wise summary (count, avg CGPA, pass rate)
void printDepartmentSummary();

// Export a chosen report to a text file
void exportReportToFile(int reportChoice, const string& filename);

#endif