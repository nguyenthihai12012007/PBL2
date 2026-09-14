#include "class.h"
#include <iostream>
using namespace std;

Candidate::Candidate() 
    : User(), IDcandidate(0)  {
    professional.GPA = 0;
}

Candidate::Candidate(int id, string email, string password)
    : User(email, password), IDcandidate(id) {
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

    cout << "Nhap GPA: ";
    cin >> professional.GPA;

    cin.ignore();

    cout << "Nhap kinh nghiem: ";
    getline(cin, professional.experience);

    professional.skills.clear();
    inputSkills(professional.skills);

    inputCertificate(professional.certificates);
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

        cout << "Trinh do: ";
        cin >> c.organization;
        
        certificates.push_back(c);
    }
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
    cout << "GPA: " << professional.GPA << endl;
    cout << "Kinh nghiem: " << professional.experience << endl;

    cout << "\nKy nang:\n";
    for (int i = 0; i < professional.skills.size(); i++) {
        cout << i + 1 << ". "
             << professional.skills[i].name
             << " - Level: "
             << professional.skills[i].level
             << endl;
    }

    cout << "\nChung chi:\n";
    if (professional.certificates.empty()) {
        cout << "Chua co chung chi nao.\n";
    } else {
        for (int i = 0; i < professional.certificates.size(); i++) {
            cout << i + 1 << ". "
                << professional.certificates[i].name
                << " - "
                << professional.certificates[i].organization
                << endl;
        }
    }
}