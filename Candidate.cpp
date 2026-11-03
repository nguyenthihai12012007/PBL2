#include "class.h"
#include <iostream>
#include <ctime>
#include <cctype>
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

//bỏ
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

//bỏ
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

//bỏ
void inputCertificate(vector<Certificate>& certificates) {
    int n;

    cout << "Nhap so chung chi: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        Certificate c;

        cout << "ID chung chi (1: IELTS, 2: TOEIC L&R, 3: HSK): ";
        cin >> c.certificateTypeID;
        cin.ignore();

        cout << "To chuc cap: ";
        getline(cin, c.organization);

        cout << "Nhap diem hoac cap: ";
        cin >> c.value;
        cin.ignore();

        certificates.push_back(c);
    }
}

//bỏ
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

//bỏ
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

//bỏ
void Experience::displayExperience() {
    cout << "Cong ty: " << companyName << endl;
    cout << "Vi tri: " << position << endl;
    cout << "Mo ta: " << description << endl;
    cout << "Thoi gian: " << startDate
         << " - " << endDate << endl;
}

//bỏ
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
                 << professional.certificates[i].certificateTypeID
                 << " - "
                 << professional.certificates[i].value
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

//bỏ
void Candidate::updateCandidate() {
    int choice;

    do {
        cout << "\n===== CAP NHAT THONG TIN CANDIDATE =====\n";
        cout << "1. Cap nhat ho ten\n";
        cout << "2. Cap nhat ngay sinh\n";
        cout << "3. Cap nhat gioi tinh\n";
        cout << "4. Cap nhat so dien thoai\n";
        cout << "5. Cap nhat dia chi\n";
        cout << "6. Cap nhat chuyen nganh\n";
        cout << "7. Cap nhat truong\n";
        cout << "8. Cap nhat trinh do hoc van\n";
        cout << "9. Cap nhat GPA\n";
        cout << "0. Quay lai\n";

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

//bỏ
void Candidate::addSkill() {
    Skill s;

    cout << "Nhap ten ky nang: ";
    getline(cin, s.name);

    cout << "Nhap level: ";
    cin >> s.level;
    cin.ignore();

    professional.skills.push_back(s);
}

//bỏ
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

//bỏ
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

//bỏ
void Candidate::addCertificate() {
    Certificate c;

    cout << "Nhap ID chung chi (1: IELTS, 2: TOEIC L&R, 3: HSK): ";
    cin >> c.certificateTypeID;
    cin.ignore();

    cout << "Nhap to chuc cap: ";
    getline(cin, c.organization);

    cout << "Nhap diem hoac cap: ";
    cin >> c.value;
    cin.ignore();

    professional.certificates.push_back(c);

    cout << "Them chung chi thanh cong!\n";
}

//bỏ
void Candidate::updateCertificate() {
    if (professional.certificates.empty()) {
        cout << "Chua co chung chi nao!\n";
        return;
    }

    cout << "\n===== DANH SACH CHUNG CHI =====\n";

    for (int i = 0; i < professional.certificates.size(); i++) {
        cout << i + 1 << ". "
             << professional.certificates[i].certificateTypeID
             << " - "
             << professional.certificates[i].organization
             << " - "
             << professional.certificates[i].value
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

    cout << "Nhap ID chung chi moi (1: IELTS, 2: TOEIC L&R, 3: HSK): ";
    cin >> professional.certificates[index].certificateTypeID;
    cin.ignore();

    cout << "Nhap to chuc cap moi: ";
    getline(cin, professional.certificates[index].organization);

    cout << "Nhap diem hoac cap moi: ";
    cin >> professional.certificates[index].value;
    cin.ignore();

    cout << "Cap nhat chung chi thanh cong!\n";
}

//bỏ
void Candidate::deleteCertificate() {
    if (professional.certificates.empty()) {
        cout << "Chua co chung chi nao!\n";
        return;
    }

    cout << "\n===== DANH SACH CHUNG CHI =====\n";

    for (int i = 0; i < professional.certificates.size(); i++) {
        cout << i + 1 << ". "
             << professional.certificates[i].certificateTypeID
             << " - "
             << professional.certificates[i].organization
             << " - "
             << professional.certificates[i].value
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

//bỏ
void Candidate::addExperience() {
    Experience e;

    e.inputExperience();
    experiences.push_back(e);

    cout << "Them kinh nghiem thanh cong!\n";
}

//bỏ
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

//bỏ
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

const vector<Skill>& Candidate::getSkill() const {
    return professional.skills;
}

const vector<Certificate>& Candidate::getCertificate() const {
    return professional.certificates;
}

bool parseExperienceDate(const string& date,int& year, int& month, int& day) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            continue;
        }

        if (date[i] < '0' || date[i] > '9') {
            return false;
        }
    }

    year = stoi(date.substr(0, 4));
    month = stoi(date.substr(5, 2));
    day = stoi(date.substr(8, 2));

    if (year < 1 || month < 1 || month > 12) {
        return false;
    }

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);

    if (leapYear) {
        daysInMonth[1] = 29;
    }

    return day >= 1 && day <= daysInMonth[month - 1];
}

int calculateMonths(const string& start, const string& end) {
    int startYear, startMonth, startDay;
    int endYear, endMonth, endDay;

    if (!parseExperienceDate(start, startYear, startMonth, startDay)) {
        return 0;
    }

    if (end == "Hiện tại") {
        time_t now = time(nullptr);
        tm* today = localtime(&now);

        if (today == nullptr) {
            return 0;
        }

        endYear = today->tm_year + 1900;
        endMonth = today->tm_mon + 1;
        endDay = today->tm_mday;
    } else {
        if (!parseExperienceDate(end, endYear, endMonth, endDay)) {
            return 0;
        }
    }

    int months = (endYear - startYear) * 12
               + (endMonth - startMonth);

    if (endDay < startDay) {
        months--;
    }

    if (months < 0) {
        return 0;
    }

    return months;
}

int Candidate::getTimeExperience() const {
    int totalTime = 0;

    for(const Experience& experience : experiences) {
        string start = experience.getStartDate();
        string end = experience.getEndDate();
        totalTime += calculateMonths(start, end);
    }
    return totalTime;
}

bool Candidate::addSkill(const Skill& skill, Data& dt) {
    professional.skills.push_back(skill);
    if (dt.saveCandidateSkill(*this)) {
        return true;
    }
    professional.skills.pop_back();
    return false;
}

bool Candidate::updateSkill(int index, const Skill& skill, Data& dt) {
    if (index < 0 || index >= static_cast<int>(professional.skills.size())) {
        return false;
    }
    Skill oldSkill = professional.skills[index];
    professional.skills[index] = skill;
    if (dt.saveCandidateSkill(*this)) {
        return true;
    }
    professional.skills[index] = oldSkill;
    return false;
}

bool Candidate::deleteSkill(int index, Data& dt) {
    if (index < 0 || index >= static_cast<int>(professional.skills.size())) {
        return false;
    }
    Skill oldSkill = professional.skills[index];
    professional.skills.erase(professional.skills.begin() + index);
    if (dt.saveCandidateSkill(*this)) {
        return true;
    }
    professional.skills.insert(professional.skills.begin() + index, oldSkill);
    return false;
}

bool Candidate::addCertificate(const Certificate& cert, Data& dt) {
    professional.certificates.push_back(cert);
    if (dt.saveCandidateCertificate(*this)) {
        return true;
    }
    professional.certificates.pop_back();
    return false;
}

bool Candidate::updateCertificate(int index, const Certificate& cert, Data& dt) {
    if (index < 0 || index >= static_cast<int>(professional.certificates.size())) {
        return false;
    }
    Certificate oldCert = professional.certificates[index];
    professional.certificates[index] = cert;
    if (dt.saveCandidateCertificate(*this)) {
        return true;
    }
    professional.certificates[index] = oldCert;
    return false;
}

bool Candidate::deleteCertificate(int index, Data& dt) {
    if (index < 0 || index >= static_cast<int>(professional.certificates.size())) {
        return false;
    }
    Certificate oldCert = professional.certificates[index];
    professional.certificates.erase(professional.certificates.begin() + index);
    if (dt.saveCandidateCertificate(*this)) {
        return true;
    }
    professional.certificates.insert(professional.certificates.begin() + index, oldCert);
    return false;
}

bool Candidate::addExperience(const Experience& exp, Data& dt) {
    experiences.push_back(exp);
    if (dt.saveCandidateExperience(*this)) {
        return true;
    }
    experiences.pop_back();
    return false;
}

bool Candidate::updateExperience(int index, const Experience& exp, Data& dt) {
    if (index < 0 || index >= static_cast<int>(experiences.size())) {
        return false;
    }
    Experience oldExp = experiences[index];
    experiences[index] = exp;
    if (dt.saveCandidateExperience(*this)) {
        return true;
    }
    experiences[index] = oldExp;
    return false;
}

bool Candidate::deleteExperience(int index, Data& dt) {
    if (index < 0 || index >= static_cast<int>(experiences.size())) {
        return false;
    }
    Experience oldExp = experiences[index];
    experiences.erase(experiences.begin() + index);
    if (dt.saveCandidateExperience(*this)) {
        return true;
    }
    experiences.insert(experiences.begin() + index, oldExp);
    return false;
}

bool Candidate::updateCandidate(const PersonalInfo& p, const ProfessionalInfo& prof, Data& dt) {
    personal = p;
    professional = prof;
    return dt.saveCandidate(*this);
}
