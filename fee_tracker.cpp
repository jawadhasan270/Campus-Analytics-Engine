#include "fee_tracker.h"
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
using namespace std;

const string FEES_FILE = "fees.txt";
const string FEES_HEADER = "fee_id,roll_no,semester,total_fee,amount_paid,due_date,payment_date,payment_method,status";

// Manual string to int
static int strToInt(const string& s) {
    int v = 0;
    for (int i = 0; i < (int)s.size(); i++)
        if (isdigit(s[i])) v = v * 10 + (s[i] - '0');
    return v;
}

// Manual double to string (2 decimal places)
static string dblToStr(double v) {
    int intP = (int)v;
    int fracP = (int)((v - intP) * 100 + 0.5);
    if (fracP >= 100) { intP++; fracP = 0; }
    string s = "";
    if (intP == 0) s = "0";
    else { int t = intP; while (t > 0) { s = char('0'+t%10)+s; t /= 10; } }
    s += ".";
    string f = "";
    if (fracP == 0) f = "00";
    else {
        int t2 = fracP;
        while (t2 > 0) { f = char('0'+t2%10)+f; t2 /= 10; }
        if ((int)f.size() < 2) f = "0" + f;
    }
    s += f;
    return s;
}

// Validate date DD-MM-YYYY
bool isValidDate(const string& date) {
    if (date.size() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(date[i])) return false;
    }
    int dd = strToInt(date.substr(0, 2));
    int mm = strToInt(date.substr(3, 2));
    int yy = strToInt(date.substr(6, 4));
    if (mm < 1 || mm > 12) return false;
    if (dd < 1 || dd > 31) return false;
    if (yy < 2000 || yy > 2100) return false;
    return true;
}

// Convert DD-MM-YYYY to total days from year 0 (manual)
static int dateToDays(const string& date) {
    int dd = strToInt(date.substr(0, 2));
    int mm = strToInt(date.substr(3, 2));
    int yy = strToInt(date.substr(6, 4));

    int monthDays[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int days = yy * 365;
    days += (yy / 4) - (yy / 100) + (yy / 400);

    bool leap = (yy % 4 == 0 && yy % 100 != 0) || (yy % 400 == 0);
    if (leap) monthDays[2] = 29;

    for (int m = 1; m < mm; m++) days += monthDays[m];
    days += dd;
    return days;
}

int daysBetween(const string& date1, const string& date2) {
    return dateToDays(date2) - dateToDays(date1);
}

void recordPayment(const string& roll, const string& semester, double amount, const string& paidDate) {
    if (!isValidDate(paidDate)) {
        cout << "  [Error] Invalid date format. Use DD-MM-YYYY.\n";
        return;
    }

    vector<vector<string>> rows = readTXT(FEES_FILE);
    bool found = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 9) continue;
        if (rows[i][1] != roll || rows[i][2] != semester) continue;

        double totalFee = strToInt(rows[i][3]);
        double alreadyPaid = strToInt(rows[i][4]);
        double newPaid = alreadyPaid + amount;
        if (newPaid > totalFee) newPaid = totalFee;

        rows[i][4] = dblToStr(newPaid);
        rows[i][6] = paidDate;
        rows[i][7] = "Cash";

        if (newPaid >= totalFee) {
            int diff = daysBetween(rows[i][5], paidDate);
            rows[i][8] = (diff > 0) ? "paid_late" : "paid";
        } else {
            rows[i][8] = "partial";
        }
        found = true;
        break;
    }

    if (!found) {
        cout << "  [Error] Fee record not found for " << roll << " semester " << semester << "\n";
        return;
    }
    writeTXT(FEES_FILE, FEES_HEADER, rows);
    cout << "  [Success] Payment of Rs." << dblToStr(amount) << " recorded.\n";
}

double computeLateFine(const string& roll, const string& semester) {
    vector<string> row = findRow(FEES_FILE, 1, roll);
    if (row.empty() || row.size() < 9) return 0.0;
    if (row[2] != semester) return 0.0;

    string dueDate  = row[5];
    string paidDate = row[6];

    if (paidDate.empty() || paidDate == "pending" || paidDate == "-") return 0.0;

    int diff = daysBetween(dueDate, paidDate);
    if (diff <= 0) return 0.0;

    int completeWeeks = diff / 7;
    double totalFee = (double)strToInt(row[3]);
    double fine = totalFee * 0.02 * completeWeeks;
    return fine;
}

void generateReceipt(const string& roll, const string& semester) {
    vector<vector<string>> rows = readTXT(FEES_FILE);
    vector<string> feeRow;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 9 && rows[i][1] == roll && rows[i][2] == semester) {
            feeRow = rows[i];
            break;
        }
    }
    if (feeRow.empty()) {
        cout << "  [Error] No fee record found.\n";
        return;
    }

    vector<string> stu = searchByRoll(roll);
    string name = (stu.size() >= 2) ? stu[1] : "Unknown";

    double totalFee   = (double)strToInt(feeRow[3]);
    double amtPaid    = strToInt(feeRow[4]);
    double lateFine   = computeLateFine(roll, semester);
    double totalDue   = totalFee + lateFine;
    double balance    = totalDue - amtPaid;

    cout << "\n";
    cout << string(50, '=') << "\n";
    cout << setw(30) << "   CAMPUS FEE RECEIPT\n";
    cout << string(50, '=') << "\n";
    cout << left << setw(20) << "  Student Roll:" << roll << "\n";
    cout << left << setw(20) << "  Student Name:" << name << "\n";
    cout << left << setw(20) << "  Semester:"     << semester << "\n";
    cout << left << setw(20) << "  Due Date:"     << feeRow[5] << "\n";
    cout << left << setw(20) << "  Paid Date:"    << feeRow[6] << "\n";
    cout << string(50, '-') << "\n";
    cout << left << setw(30) << "  Tuition Fee:"
         << right << setw(10) << ("Rs. " + dblToStr(totalFee)) << "\n";
    if (lateFine > 0) {
        cout << left << setw(30) << "  Late Fine:"
             << right << setw(10) << ("Rs. " + dblToStr(lateFine)) << "\n";
    }
    cout << string(50, '-') << "\n";
    cout << left << setw(30) << "  Total Due:"
         << right << setw(10) << ("Rs. " + dblToStr(totalDue)) << "\n";
    cout << left << setw(30) << "  Amount Paid:"
         << right << setw(10) << ("Rs. " + dblToStr(amtPaid)) << "\n";
    cout << left << setw(30) << "  Balance:"
         << right << setw(10) << ("Rs. " + dblToStr(balance)) << "\n";
    cout << string(50, '=') << "\n";
    cout << "  Status: " << feeRow[8] << "\n";
    cout << string(50, '=') << "\n";
}

vector<vector<string>> getDefaulters() {
    vector<vector<string>> rows = readTXT(FEES_FILE);
    vector<vector<string>> defaulters;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 9) continue;
        double totalFee = (double)strToInt(rows[i][3]);
        double amtPaid  = (double)strToInt(rows[i][4]);
        double balance  = totalFee - amtPaid;
        if (balance > 0) {
            vector<string> d;
            d.push_back(rows[i][1]);
            d.push_back(rows[i][2]);
            d.push_back(rows[i][3]);
            d.push_back(rows[i][4]);
            d.push_back(dblToStr(balance));
            d.push_back(rows[i][5]);
            defaulters.push_back(d);
        }
    }

    for (int i = 0; i < (int)defaulters.size() - 1; i++) {
        for (int j = 0; j < (int)defaulters.size() - 1 - i; j++) {
            double b1 = 0, b2 = 0;
            bool dot1 = false; double d1 = 1;
            for (int k = 0; k < (int)defaulters[j][4].size(); k++) {
                if (defaulters[j][4][k] == '.') { dot1 = true; }
                else if (!dot1) b1 = b1*10+(defaulters[j][4][k]-'0');
                else { b1 += (defaulters[j][4][k]-'0')/d1/10; d1 *= 10; }
            }
            bool dot2 = false; double d2 = 1;
            for (int k = 0; k < (int)defaulters[j+1][4].size(); k++) {
                if (defaulters[j+1][4][k] == '.') { dot2 = true; }
                else if (!dot2) b2 = b2*10+(defaulters[j+1][4][k]-'0');
                else { b2 += (defaulters[j+1][4][k]-'0')/d2/10; d2 *= 10; }
            }
            if (b1 < b2) {
                vector<string> tmp = defaulters[j];
                defaulters[j] = defaulters[j+1];
                defaulters[j+1] = tmp;
            }
        }
    }
    return defaulters;
}