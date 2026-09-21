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

void Job::updateJob() {
    int choice;
    do {
        cout << "\n===== CAP NHAT CONG VIEC =====\n";
        cout << "1. Cap nhat vi tri tuyen dung\n";
        cout << "2. Cap nhat mo ta\n";
        cout << "3. Cap nhat luong\n";
        cout << "4. Cap nhat dia diem\n";
        cout << "5. Cap nhat GPA toi thieu\n";
        cout << "6. Cap nhat kinh nghiem toi thieu\n";
        cout << "7. Cap nhat han tuyen dung\n";
        cout << "8. Cap nhat chuyen nganh\n";
        cout << "0. Hoan tat\n";
        cout << "Lua chon: ";
        cin >> choice;
        cin.ignore();
        switch(choice) {
            case 1:
                cout << "Nhap vi tri tuyen dung moi: ";
                getline(cin, title);
                break;

            case 2:
                cout << "Nhap mo ta moi: ";
                getline(cin, description);
                break;

            case 3:
                cout << "Nhap luong moi: ";
                cin >> salary;
                cin.ignore();
                break;

            case 4:
                cout << "Nhap dia diem moi: ";
                getline(cin, location);
                break;

            case 5:
                cout << "Nhap GPA toi thieu moi: ";
                cin >> minGPA;
                cin.ignore();
                break;

            case 6:
                cout << "Nhap kinh nghiem toi thieu moi: ";
                cin >> minExperience;
                cin.ignore();
                break;

            case 7:
                cout << "Nhap han tuyen dung moi: ";
                getline(cin, deadline);
                break;

            case 8:
                cout << "Nhap chuyen nganh moi: ";
                getline(cin, nameMajor);
                break;

            case 0:
                cout << "Cap nhat cong viec thanh cong!\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while (choice != 0);
}

bool Job::checkStatus() {
    return status;
}

int Job::getIDjob() const {
    return IDjob;
}

void Job::setStatus(bool s) {
    status = s;
}

