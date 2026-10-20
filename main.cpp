#include "class.h"
#include "menuCandidate.h"
#include "menuCompany.h"
#include <iostream>
#include <limits>

using namespace std;

int main() {
    Candidate candidate(1, "candidate@gmail.com", "123456");
    Company company(2, "company@gmail.com", "123456");

    cout << "===== NHAP HO SO UNG VIEN MAU =====\n";
    candidate.inputCandidate();

    cout << "\n===== NHAP HO SO CONG TY MAU =====\n";
    company.inputCompany();

    int choice;

    do {
        cout << "\n===== MENU CHINH - CHAY THU =====\n";
        cout << "1. Vao menu ung vien\n";
        cout << "2. Vao menu cong ty\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "Lua chon: ";

        if (!(cin >> choice)) {
            if (cin.eof()) {
                return 0;
            }

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Vui long nhap so!\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                menuCandidate(candidate);
                break;

            case 2:
                companyMenu(company);
                break;

            case 0:
                cout << "Da thoat chuong trinh!\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }

    } while (true);

    return 0;
}