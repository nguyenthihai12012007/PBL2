#include "class.h"
#include <iostream>
using namespace std;

Job::Job()
    : IDjob(0), IDcompany(0), salary(0), 
      minGPA(0), minExperience(0), status(true) {
}

void Job::inputJob(int companyID) {
    IDcompany = companyID;
    cout << "Nhap ten cong viec: ";
    getline(cin, title);
    cout << "Nhap mo ta cong viec: ";
    getline(cin, description);
    cout << "Nhap muc luong: ";
    cin >> salary;
    cin.ignore();
    cout << "Nhap dia diem: ";
    getline(cin, location);
    cout << "Nhap GPA toi thieu: ";
    cin >> minGPA;
    cout << "Nhap so nam kinh nghiem toi thieu: ";
    cin >> minExperience;
    cin.ignore();
    cout << "Nhap han nop ho so: ";
    getline(cin, deadline);
    status = true;
}

void Job::displayJob() {
    cout << "\n===== THONG TIN CONG VIEC ===== \n";
    cout << "ID cong viec: " << IDjob << endl;
    cout << "ID cong ty: " << IDcompany << endl;
    cout << "Ten cong viec: " << title << endl;
    cout << "Mo ta: " << description << endl;
    cout << "Luong: " << salary << endl;
    cout << "Dia diem: " << location << endl;
    cout << "GPA toi thieu: " << minGPA << endl;
    cout << "Kinh nghiem toi thieu: " << minExperience << " nam" << endl;
    cout << "Han nop ho so: " << deadline << endl;
    cout << "Trang thai: ";
    if (status) 
        cout << "Dang tuyen" << endl;
    else 
        cout << "Da dong" << endl;
}