#include "class.h"
#include "menuCandidate.h"
#include "menuCompany.h"
#include <iostream>
#include <limits>
using namespace std;

void loginMenu(Data& dt) {
    string email, password;
    User user;

    cout << "\n";
    cout << "  +================================================+\n";
    cout << "  |                  DANG NHAP                     |\n";
    cout << "  +================================================+\n";

    cout << "  Email: ";
    getline(cin, email);
    cout << "  Mat khau: ";
    getline(cin, password);

    if (user.login(dt, email, password)) {
        if (user.getRole() == "CANDIDATE") {
            cout << "\n  Xin chao ung vien!\n";
            Candidate candidate;

            candidate.setIDuser(user.getIDuser());
            candidate.setEmail(user.getEmail());
            candidate.setRole(user.getRole());
            
            if (dt.getCandidateByUserID(user.getIDuser(), candidate)) {
                dt.loadCandidateSkill(candidate);
                dt.loadCandidateCertificate(candidate);
                dt.loadCandidateExperience(candidate);
            }
            menuCandidate(candidate,dt);

        } else if (user.getRole() == "COMPANY") {
            cout << "\n  Xin chao cong ty!\n";
            Company company;
            company.setIDuser(user.getIDuser());
            company.setEmail(user.getEmail());
            company.setRole(user.getRole());

            dt.getCompanyByUserID(user.getIDuser(), company);

            companyMenu(company, dt);
        } else {
            cout << "\n  Tai khoan khong ton tai!\n";
        }
    }
}

void registerMenu(Data& dt) {
    string email, password, role;
    int choice;

    cout << "\n";
    cout << "  +================================================+\n";
    cout << "  |                  DANG KY                       |\n";
    cout << "  +================================================+\n";

    cout << "  Email: ";
    getline(cin, email);
    if (email.empty()) {
        cout << "\n  Email khong duoc de trong!\n";
        return;
    }
    if (dt.emailExists(email)) {
        cout << "\n  Email da duoc su dung!\n";
        return;
    }
    cout << "  Mat khau: ";
    getline(cin, password);
    if (password.empty()) {
        cout << "\n  Mat khau khong duoc de trong!\n";
        return;
    }

    cout << "\n  Chon loai tai khoan:\n";
    cout << "  [1] Ung vien\n";
    cout << "  [2] Cong ty\n";
    cout << "  Lua chon: ";
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n  Lua chon khong hop le!\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (choice == 1) {
        role = "CANDIDATE";
    }
    else if (choice == 2) {
        role = "COMPANY";
    }
    else {
        cout << "\n  Vui long chon 1 hoac 2!\n";
        return;
    }
    int id = dt.getNextID("users.txt");
    User newUser(id, email, password, role);
    if (dt.registerUser(newUser)) {
        cout << "\n  Dang ky tai khoan thanh cong!\n";
        cout << "  Ban co the dang nhap ngay bay gio.\n";
    }
    else {
        cout << "\n  Dang ky that bai!\n";
    }
}

void mainMenu(Data& dt) {
    int choice;
    do {
        cout << "\n";
        cout << "  +================================================+\n";
        cout << "  |                                                |\n";
        cout << "  |                   JOB SYSTEM                   |\n";
        cout << "  |            HE THONG QUAN LY VIEC LAM           |\n";
        cout << "  |                                                |\n";
        cout << "  +================================================+\n";
        cout << "  |                                                |\n";
        cout << "  |       [1]  DANG NHAP                           |\n";
        cout << "  |       [2]  DANG KY                             |\n";
        cout << "  |       [0]  THOAT                               |\n";
        cout << "  |                                                |\n";
        cout << "  +================================================+\n";
        cout << "\n  Lua chon cua ban >> ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n  Vui long nhap so hop le!\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice) {
            case 1:
                loginMenu(dt);
                break;

            case 2:
                registerMenu(dt);
                break;

            case 0:
                cout << "\n  Cam on ban da su dung JOB SYSTEM!\n";
                break;

            default:
                cout << "\n  Lua chon khong hop le. Vui long chon lai!\n";
        }
    } while (choice != 0);
}
