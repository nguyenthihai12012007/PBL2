#include "class.h"
#include "menuCandidate.h"
#include <iostream>
using namespace std;

void inputSkills(vector<Skill>& skills);
void inputCertificate(vector<Certificate>& certificates);
void inputExperiences(vector<Experience>& experiences);


Candidate::Candidate()
    : User(), IDcandidate(0) {
    professional.GPA = 0;
}


Candidate::Candidate(int id, string email, string password)
    : User(id, email, password, "CANDIDATE"), IDcandidate(id) {
    professional.GPA = 0;
}


void Candidate::inputCandidate() {
    cout << "Nhap ho ten: ";
    getline(cin, personal.fullName);

    cout << "Nhap ngay sinh: ";
    getline(cin, personal.birthday);

    cout << "Nhap gioi tinh: ";
    getline(cin, personal.gender);

    cout << "Nhap so dien thoai: ";
    getline(cin, personal.phoneCandidate);

    cout << "Nhap dia chi: ";
    getline(cin, personal.addressCandidate);

    cout << "Nhap chuyen nganh: ";
    getline(cin, professional.major);

    cout << "Nhap truong: ";
    getline(cin, professional.school);

    cout << "Nhap trinh do hoc van: ";
    getline(cin, professional.educationLevel);

    cout << "Nhap GPA: ";
    cin >> professional.GPA;
    cin.ignore();

    professional.skills.clear();
    inputSkills(professional.skills);

    professional.certificates.clear();
    inputCertificate(professional.certificates);

    experiences.clear();
    inputExperiences(experiences);
}


void inputSkills(vector<Skill>& skills) {
    int n;

    cout << "Nhap so ky nang: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        Skill s;

        cout << "Ten ky nang: ";
        getline(cin, s.name);

        cout << "Level: ";
        cin >> s.level;
        cin.ignore();

        skills.push_back(s);
    }
}


void inputCertificate(vector<Certificate>& certificates) {
    int n;

    cout << "Nhap so chung chi: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        Certificate c;

        cout << "Ten chung chi: ";
        getline(cin, c.name);

        cout << "To chuc cap: ";
        getline(cin, c.organization);

        cout << "Nhap diem hoac cap: ";
        getline(cin, c.levelOrScore);

        certificates.push_back(c);
    }
}


void inputExperiences(vector<Experience>& experiences) {
    int n;

    cout << "Nhap so kinh nghiem lam viec: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        cout << "\nKinh nghiem " << i + 1 << ":\n";

        Experience e;

        e.inputExperience();

        experiences.push_back(e);
    }
}


void Experience::inputExperience() {
    cout << "Ten cong ty: ";
    getline(cin, companyName);

    cout << "Vi tri: ";
    getline(cin, position);

    cout << "Mo ta cong viec: ";
    getline(cin, description);

    cout << "Ngay bat dau: ";
    getline(cin, startDate);

    cout << "Ngay ket thuc: ";
    getline(cin, endDate);
}


void Experience::displayExperience() {
    cout << "Cong ty: " << companyName << endl;
    cout << "Vi tri: " << position << endl;
    cout << "Mo ta: " << description << endl;
    cout << "Thoi gian: " << startDate
         << " - " << endDate << endl;
}


void Candidate::displayCandidate() {
    cout << "\n===== THONG TIN UNG VIEN =====\n";

    cout << "ID: " << IDcandidate << endl;

    cout << "Ho ten: " << personal.fullName << endl;
    cout << "Ngay sinh: " << personal.birthday << endl;
    cout << "Gioi tinh: " << personal.gender << endl;
    cout << "So dien thoai: " << personal.phoneCandidate << endl;
    cout << "Dia chi: " << personal.addressCandidate << endl;

    cout << "Chuyen nganh: " << professional.major << endl;
    cout << "Truong: " << professional.school << endl;
    cout << "Trinh do hoc van: "
         << professional.educationLevel << endl;
    cout << "GPA: " << professional.GPA << endl;


    cout << "\nKy nang:\n";

    if (professional.skills.empty()) {
        cout << "Chua co ky nang nao.\n";
    }
    else {
        for (int i = 0; i < professional.skills.size(); i++) {
            cout << i + 1 << ". "
                 << professional.skills[i].name
                 << " - Level: "
                 << professional.skills[i].level
                 << endl;
        }
    }


    cout << "\nChung chi:\n";

    if (professional.certificates.empty()) {
        cout << "Chua co chung chi nao.\n";
    }
    else {
        for (int i = 0; i < professional.certificates.size(); i++) {
            cout << i + 1 << ". "
                 << professional.certificates[i].name
                 << " - "
                 << professional.certificates[i].levelOrScore
                 << endl;
        }
    }


    cout << "\nKinh nghiem lam viec:\n";

    if (experiences.empty()) {
        cout << "Chua co kinh nghiem lam viec.\n";
    }
    else {
        for (int i = 0; i < experiences.size(); i++) {
            cout << "\n" << i + 1 << ".\n";
            experiences[i].displayExperience();
        }
    }
}

void Candidate::updateCandidate() {
    int choice;

    do {
        menuUpdateCandidate();

        cout << "Lua chon: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Nhap ho ten moi: ";
                getline(cin, personal.fullName);
                break;

            case 2:
                cout << "Nhap ngay sinh moi: ";
                getline(cin, personal.birthday);
                break;

            case 3:
                cout << "Nhap gioi tinh moi: ";
                getline(cin, personal.gender);
                break;

            case 4:
                cout << "Nhap so dien thoai moi: ";
                getline(cin, personal.phoneCandidate);
                break;

            case 5:
                cout << "Nhap dia chi moi: ";
                getline(cin, personal.addressCandidate);
                break;

            case 6:
                cout << "Nhap chuyen nganh moi: ";
                getline(cin, professional.major);
                break;

            case 7:
                cout << "Nhap truong moi: ";
                getline(cin, professional.school);
                break;

            case 8:
                cout << "Nhap trinh do hoc van moi: ";
                getline(cin, professional.educationLevel);
                break;

            case 9:
                cout << "Nhap GPA moi: ";
                cin >> professional.GPA;
                cin.ignore();
                break;

            case 0:
                cout << "Hoan tat cap nhat!\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }

    } while (choice != 0);
}

void Candidate::addSkill() {
    Skill s;

    cout << "Nhap ten ky nang: ";
    getline(cin, s.name);

    cout << "Nhap level: ";
    cin >> s.level;
    cin.ignore();

    professional.skills.push_back(s);
}

void Candidate::updateSkill() {
    if (professional.skills.empty()) {
        cout << "Chua co ky nang nao!\n";
        return;
    }

    cout << "\n===== DANH SACH KY NANG =====\n";
    for (int i = 0; i < professional.skills.size(); i++) {
        cout << i + 1 << ". "
             << professional.skills[i].name
             << " - Level: "
             << professional.skills[i].level << endl;
    }

    int choice;
    cout << "Chon ky nang can sua: ";
    cin >> choice;
    cin.ignore();

    if (choice < 1 || choice > professional.skills.size()) {
        cout << "Lua chon khong hop le!\n";
        return;
    }

    int index = choice - 1;

    cout << "Nhap ten ky nang moi: ";
    getline(cin, professional.skills[index].name);

    cout << "Nhap level moi: ";
    cin >> professional.skills[index].level;
    cin.ignore();

    cout << "Cap nhat ky nang thanh cong!\n";
}

void Candidate::deleteSkill() {
    if (professional.skills.empty()) {
        cout << "Chua co ky nang nao!\n";
        return;
    }

    cout << "\n===== DANH SACH KY NANG =====\n";
    for (int i = 0; i < professional.skills.size(); i++) {
        cout << i + 1 << ". "
             << professional.skills[i].name
             << " - Level: "
             << professional.skills[i].level << endl;
    }

    int choice;
    cout << "Chon ky nang can xoa: ";
    cin >> choice;

    if (choice < 1 || choice > professional.skills.size()) {
        cout << "Lua chon khong hop le!\n";
        return;
    }

    professional.skills.erase(
        professional.skills.begin() + choice - 1
    );

    cout << "Xoa ky nang thanh cong!\n";
}

void Candidate::addCertificate() {
    Certificate c;

    cout << "Nhap ten chung chi: ";
    getline(cin, c.name);

    cout << "Nhap to chuc cap: ";
    getline(cin, c.organization);

    cout << "Nhap diem hoac cap: ";
    getline(cin, c.levelOrScore);

    professional.certificates.push_back(c);

    cout << "Them chung chi thanh cong!\n";
}

void Candidate::updateCertificate() {
    if (professional.certificates.empty()) {
        cout << "Chua co chung chi nao!\n";
        return;
    }

    cout << "\n===== DANH SACH CHUNG CHI =====\n";

    for (int i = 0; i < professional.certificates.size(); i++) {
        cout << i + 1 << ". "
             << professional.certificates[i].name
             << " - "
             << professional.certificates[i].organization
             << " - "
             << professional.certificates[i].levelOrScore
             << endl;
    }

    int choice;
    cout << "Chon chung chi can sua: ";
    cin >> choice;
    cin.ignore();

    if (choice < 1 || choice > professional.certificates.size()) {
        cout << "Lua chon khong hop le!\n";
        return;
    }

    int index = choice - 1;

    cout << "Nhap ten chung chi moi: ";
    getline(cin, professional.certificates[index].name);

    cout << "Nhap to chuc cap moi: ";
    getline(cin, professional.certificates[index].organization);

    cout << "Nhap diem hoac cap moi: ";
    getline(cin, professional.certificates[index].levelOrScore);

    cout << "Cap nhat chung chi thanh cong!\n";
}

void Candidate::deleteCertificate() {
    if (professional.certificates.empty()) {
        cout << "Chua co chung chi nao!\n";
        return;
    }

    cout << "\n===== DANH SACH CHUNG CHI =====\n";

    for (int i = 0; i < professional.certificates.size(); i++) {
        cout << i + 1 << ". "
             << professional.certificates[i].name
             << " - "
             << professional.certificates[i].organization
             << " - "
             << professional.certificates[i].levelOrScore
             << endl;
    }

    int choice;
    cout << "Chon chung chi can xoa: ";
    cin >> choice;

    if (choice < 1 || choice > professional.certificates.size()) {
        cout << "Lua chon khong hop le!\n";
        return;
    }

    professional.certificates.erase(
        professional.certificates.begin() + choice - 1
    );

    cout << "Xoa chung chi thanh cong!\n";
}

void Candidate::addExperience() {
    Experience e;

    e.inputExperience();
    experiences.push_back(e);

    cout << "Them kinh nghiem thanh cong!\n";
}

void Candidate::updateExperience() {
    if (experiences.empty()) {
        cout << "Chua co kinh nghiem nao!\n";
        return;
    }

    cout << "\n===== DANH SACH KINH NGHIEM =====\n";

    for (int i = 0; i < experiences.size(); i++) {
        cout << "\n" << i + 1 << ".\n";
        experiences[i].displayExperience();
    }

    int choice;

    cout << "Chon kinh nghiem can sua: ";
    cin >> choice;
    cin.ignore();

    if (choice < 1 || choice > experiences.size()) {
        cout << "Lua chon khong hop le!\n";
        return;
    }

    experiences[choice - 1].inputExperience();

    cout << "Cap nhat kinh nghiem thanh cong!\n";
}

void Candidate::deleteExperience() {
    if (experiences.empty()) {
        cout << "Chua co kinh nghiem nao!\n";
        return;
    }

    cout << "\n===== DANH SACH KINH NGHIEM =====\n";

    for (int i = 0; i < experiences.size(); i++) {
        cout << "\n" << i + 1 << ".\n";
        experiences[i].displayExperience();
    }

    int choice;

    cout << "Chon kinh nghiem can xoa: ";
    cin >> choice;

    if (choice < 1 || choice > experiences.size()) {
        cout << "Lua chon khong hop le!\n";
        return;
    }

    experiences.erase(
        experiences.begin() + choice - 1
    );

    cout << "Xoa kinh nghiem thanh cong!\n";
}

