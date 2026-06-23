#include <iostream>
#include <fstream>
#include <string>

#include "filehandler.h"

using namespace std;

void displayStudents()
{
    ifstream file("students.txt");

    string line;

    while(getline(file, line))
    {
        cout << line << endl;
    }

    file.close();
}