#include "grades.h"
#include "filehandler.h"
#include "student_ops.h"
#include "attendance.h"
#include "course_ops.h"
#include <iostream>
#include <iomanip>
using namespace std;

const string GRADES_FILE = "grades.txt";
const string GRADES_HEADER = "roll_no,course_code,semester,quiz_avg,asgn_avg,mid_marks,final_marks,total,letter_grade";

// Convert string to double manually
static double strToDouble(const string& s) {
    double intP = 0, fracP = 0, fracD = 1;
    bool inFrac = false;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '.') inFrac = true;
        else if (isdigit(s[i])) {
            if (!inFrac) intP = intP * 10 + (s[i] - '0');
            else { fracP = fracP * 10 + (s[i] - '0'); fracD *= 10; }
        }
    }
    return intP + fracP / fracD;
}

// Convert double to string with 2 decimal places
static string dblToStr(double v) {
    int intP = (int)v;
    int fracP = (int)((v - intP) * 100 + 0.5);
    if (fracP >= 100) { intP++; fracP = 0; }
    string s = "";
    if (intP == 0) s = "0";
    else { int t = intP; while (t > 0) { s = char('0'+t%10)+s; t /= 10; } }
    s += ".";
    if (fracP < 10) s += "0";
    int t2 = fracP; string f = "";
    if (t2 == 0) f = "00";
    else { while (t2 > 0) { f = char('0'+t2%10)+f; t2 /= 10; } }
    s += f;
    return s;
}

double bestThreeOfFive(double quizzes[], int n) {
    if (n <= 0) return 0.0;
    if (n <= 3) {
        double sum = 0;
        for (int i = 0; i < n; i++) sum += quizzes[i];
        return sum / n;
    }
    double arr[5];
    for (int i = 0; i < n; i++) arr[i] = quizzes[i];

    int min1 = 0;
    for (int i = 1; i < n; i++) if (arr[i] < arr[min1]) min1 = i;
    arr[min1] = 999999;

    int min2 = 0;
    for (int i = 1; i < n; i++) if (arr[i] < arr[min2]) min2 = i;
    arr[min2] = 999999;

    double sum = 0;
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] < 999998) { sum += arr[i]; count++; }
    }
    return (count > 0) ? sum / count : 0.0;
}

double computeWeightedTotal(double quiz, double asgn, double mid, double finalMark) {
    return quiz * 0.10 + asgn * 0.10 + mid * 0.30 + finalMark * 0.50;
}

string getLetterGrade(double total) {
    if (total >= 85) return "A";
    if (total >= 80) return "B+";
    if (total >= 70) return "B";
    if (total >= 65) return "C+";
    if (total >= 60) return "C";
    if (total >= 50) return "D";
    return "F";
}

void enterMarks(const string& roll, const string& courseCode, const string& semester) {
    cout << "\n--- Enter Marks: " << roll << " | " << courseCode << " ---\n";

    vector<vector<string>> rows = readTXT(GRADES_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 3 && rows[i][0] == roll && rows[i][1] == courseCode && rows[i][2] == semester) {
            cout << "  [Error] Marks already entered for this student/course/semester.\n";
            return;
        }
    }

    int numQuizzes;
    cout << "  How many quizzes (1-5)? ";
    cin >> numQuizzes;
    if (numQuizzes < 1) numQuizzes = 1;
    if (numQuizzes > 5) numQuizzes = 5;

    double quizzes[5] = {0,0,0,0,0};
    for (int i = 0; i < numQuizzes; i++) {
        while (true) {
            cout << "  Quiz " << (i+1) << " marks (0-10): ";
            cin >> quizzes[i];
            if (quizzes[i] >= 0 && quizzes[i] <= 10) break;
            cout << "  [Error] Must be 0-10.\n";
        }
    }
    double quizAvg = bestThreeOfFive(quizzes, numQuizzes);

    double asgn[2] = {0,0};
    for (int i = 0; i < 2; i++) {
        while (true) {
            cout << "  Assignment " << (i+1) << " marks (0-10): ";
            cin >> asgn[i];
            if (asgn[i] >= 0 && asgn[i] <= 10) break;
            cout << "  [Error] Must be 0-10.\n";
        }
    }
    double asgnAvg = (asgn[0] + asgn[1]) / 2.0;

    double mid;
    while (true) {
        cout << "  Mid exam marks (0-40): ";
        cin >> mid;
        if (mid >= 0 && mid <= 40) break;
        cout << "  [Error] Must be 0-40.\n";
    }

    double finalM;
    while (true) {
        cout << "  Final exam marks (0-60): ";
        cin >> finalM;
        if (finalM >= 0 && finalM <= 60) break;
        cout << "  [Error] Must be 0-60.\n";
    }

    double midPct   = (mid / 40.0) * 100.0;
    double finalPct = (finalM / 60.0) * 100.0;
    double quizPct  = (quizAvg / 10.0) * 100.0;
    double asgnPct  = (asgnAvg / 10.0) * 100.0;

    double total = computeWeightedTotal(quizPct, asgnPct, midPct, finalPct);
    string letter = getLetterGrade(total);

    vector<string> newRow = {roll, courseCode, semester, dblToStr(quizPct), dblToStr(asgnPct),
                             dblToStr(mid), dblToStr(finalM), dblToStr(total), letter};
    appendTXT(GRADES_FILE, newRow);

    applyAttendancePenalty(roll, courseCode);

    cout << "  Weighted Total: " << fixed << setprecision(2) << total << " | Grade: " << letter << "\n";
    cout << "  [Success] Marks saved.\n";
}

void applyAttendancePenalty(const string& roll, const string& courseCode) {
    double pct = getAttendancePct(roll, courseCode);
    if (pct < 75.0) {
        vector<vector<string>> rows = readTXT(GRADES_FILE);
        for (int i = 0; i < (int)rows.size(); i++) {
            if (rows[i].size() >= 9 && rows[i][0] == roll && rows[i][1] == courseCode) {
                rows[i][8] = "F";
            }
        }
        writeTXT(GRADES_FILE, GRADES_HEADER, rows);
        cout << "  [Warning] Attendance < 75%. Grade overridden to F.\n";
    }
}

double computeGPA(const string& roll, const string& semester) {
    vector<vector<string>> grades = readTXT(GRADES_FILE);
    vector<vector<string>> courses = readTXT(COURSES_FILE);

    double totalPoints = 0;
    int totalCredits = 0;

    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() < 9) continue;
        if (grades[i][0] != roll || grades[i][2] != semester) continue;

        string letter = grades[i][8];
        double gpaPoint = 0;
        if (letter == "A")  gpaPoint = 4.0;
        else if (letter == "B+") gpaPoint = 3.5;
        else if (letter == "B")  gpaPoint = 3.0;
        else if (letter == "C+") gpaPoint = 2.5;
        else if (letter == "C")  gpaPoint = 2.0;
        else if (letter == "D")  gpaPoint = 1.0;
        else gpaPoint = 0.0;

        string code = grades[i][1];
        for (int j = 0; j < (int)courses.size(); j++) {
            if (courses[j].size() >= 3 && courses[j][0] == code) {
                int cr = 0;
                for (int k = 0; k < (int)courses[j][2].size(); k++)
                    if (isdigit(courses[j][2][k])) cr = cr*10+(courses[j][2][k]-'0');
                totalPoints += gpaPoint * cr;
                totalCredits += cr;
                break;
            }
        }
    }
    if (totalCredits == 0) return 0.0;
    return totalPoints / totalCredits;
}

Stats computeClassStats(const string& courseCode) {
    vector<vector<string>> grades = readTXT(GRADES_FILE);
    vector<double> totals;

    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() < 9) continue;
        if (grades[i][1] != courseCode) continue;
        totals.push_back(strToDouble(grades[i][7]));
    }

    Stats s = {0, 0, 0, 0};
    if (totals.empty()) return s;

    s.highest = totals[0];
    s.lowest  = totals[0];
    double sum = 0;
    for (int i = 0; i < (int)totals.size(); i++) {
        if (totals[i] > s.highest) s.highest = totals[i];
        if (totals[i] < s.lowest)  s.lowest  = totals[i];
        sum += totals[i];
    }
    s.mean = sum / totals.size();

    for (int i = 0; i < (int)totals.size() - 1; i++)
        for (int j = 0; j < (int)totals.size() - 1 - i; j++)
            if (totals[j] > totals[j+1]) { double t = totals[j]; totals[j] = totals[j+1]; totals[j+1] = t; }

    int n = (int)totals.size();
    s.median = (n % 2 == 0) ? (totals[n/2-1] + totals[n/2]) / 2.0 : totals[n/2];
    return s;
}

void printGrade(const vector<string>& g) {
    if (g.size() < 9) { cout << "  [Invalid grade record]\n"; return; }
    cout << "  Roll      : " << g[0] << "\n";
    cout << "  Course    : " << g[1] << "\n";
    cout << "  Semester  : " << g[2] << "\n";
    cout << "  Quiz Avg  : " << g[3] << "%\n";
    cout << "  Asgn Avg  : " << g[4] << "%\n";
    cout << "  Mid       : " << g[5] << "/40\n";
    cout << "  Final     : " << g[6] << "/60\n";
    cout << "  Total     : " << g[7] << "%\n";
    cout << "  Grade     : " << g[8] << "\n";
}