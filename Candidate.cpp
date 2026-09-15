#include "class.h"
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
                 << professional.certificates[i].organization
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