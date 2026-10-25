#include "class.h"
#include <iostream>
#include <ctime>
#include <cstdio>
using namespace std;

Job::Job()
    : IDjob(0), IDcompany(0), salary(0), 
      minGPA(0), minExperience(0), status(true) {
}

void Job::inputRequiredSkills() {
    int n;

    cout << "\nNhap so ky nang yeu cau: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        Skill s;

        cout << "Nhap ten ky nang: ";
        getline(cin, s.name);

        cout << "Nhap level toi thieu: ";
        cin >> s.level;
        cin.ignore();

        requiredSkills.push_back(s);
    }
}

void Job::inputCertificateGroups() {
    int n;

    do {
        cout << "Nhap so nhom chung chi (0: khong yeu cau): ";
        cin >> n;
        cin.ignore();
    } while (n < 0);

    certificateGroups.clear();

    for (int i = 0; i < n; i++) {
        CertificateRequirementGroup group;
        int type;
        int count;

        cout << "\n===== NHOM CHUNG CHI " << i + 1 << " =====\n";

        do {
            cout << "1. Bat buoc\n";
            cout << "0. Uu tien\n";
            cout << "Chon loai nhom: ";
            cin >> type;
            cin.ignore();
        } while (type != 0 && type != 1);

        if (type == 1) {
            group.isRequired = true;
        } else {
            group.isRequired = false;
        }

        do {
            cout << "Nhap so lua chon trong nhom (it nhat 1): ";
            cin >> count;
            cin.ignore();
        } while (count < 1);

        for (int j = 0; j < count; j++) {
            CertificateRequirement requirement;

            cout << "\nLua chon " << j + 1 << ":\n";

            do {
                cout << "1. IELTS\n";
                cout << "2. TOEIC \n";
                cout << "3. HSK\n";
                cout << "Nhap ID chung chi: ";
                cin >> requirement.certificateTypeID;
                cin.ignore();
            } while (requirement.certificateTypeID < 1
                     || requirement.certificateTypeID > 3);

            do {
                cout << "Nhap diem hoac cap toi thieu: ";
                cin >> requirement.minValue;
                cin.ignore();
            } while (requirement.minValue < 0);

            group.alternatives.push_back(requirement);
        }

        certificateGroups.push_back(group);
    }
}

void Job::inputJob(int companyID) {
    cin.ignore();
    IDcompany = companyID;

    cout << "Nhap ten cong viec: ";
    getline(cin, title);

    cout << "Nhap chuyen nganh: ";
    getline(cin, nameMajor);

    cout << "Nhap mo ta cong viec: ";
    getline(cin, description);

    cout << "Nhap muc luong: ";
    cin >> salary;
    cin.ignore();

    cout << "Nhap dia diem: ";
    getline(cin, location);

    cout << "Nhap GPA toi thieu: ";
    cin >> minGPA;

    cout << "Nhap so thang kinh nghiem toi thieu: ";
    cin >> minExperience;
    cin.ignore();

    inputCertificateGroups();

    cout << "Nhap han nop ho so: ";
    getline(cin, deadline);

    requiredSkills.clear();
    inputRequiredSkills();

    status = true;
}

void Job::displayCertificate() {
    cout << "\nChung chi yeu cau:\n";

    if (certificateGroups.empty()) {
        cout << "Khong yeu cau chung chi.\n\n";
    } else {
        for (int i = 0; i < certificateGroups.size(); i++) {
            cout << "Nhom " << i + 1;
            if (certificateGroups[i].isRequired) {
                cout << " (Bat buoc):\n";
            } else {
                cout << " (Uu tien):\n";
            }

            for (int j = 0; j < certificateGroups[i].alternatives.size(); j++) {
                const CertificateRequirement& requirement = certificateGroups[i].alternatives[j];
                if (requirement.certificateTypeID == 1) {
                    cout << "  - IELTS";
                } else if (requirement.certificateTypeID == 2) {
                    cout << "  - TOEIC L&R";
                } else if (requirement.certificateTypeID == 3) {
                    cout << "  - HSK";
                } else {
                    cout << "  - Chung chi ID " << requirement.certificateTypeID;
                }

                cout << " >= " << requirement.minValue << endl;
                cout <<"\n";
            }
        }
    }
}

void Job::displayJob() {
    cout << "\n===== THONG TIN CONG VIEC ===== \n";
    cout << "ID cong viec: " << IDjob << endl;
    cout << "ID cong ty: " << IDcompany << endl;
    cout << "Ten cong viec: " << title << endl;
    cout << "Chuyen nganh yeu cau: " << nameMajor << endl;
    cout << "Mo ta: " << description << endl;
    cout << "\nKy nang yeu cau:\n";

    if (requiredSkills.empty()) {
        cout << "Khong yeu cau ky nang cu the.\n";
    } else {
        for (int i = 0; i < requiredSkills.size(); i++) {
            cout << i + 1 << ". "
                << requiredSkills[i].name
                << " - Level toi thieu: "
                << requiredSkills[i].level
                << endl;
        }
    }

    displayCertificate();
    
    cout << "Luong: " << salary << endl;
    cout << "Dia diem: " << location << endl;
    cout << "GPA toi thieu: " << minGPA << endl;
    cout << "Kinh nghiem toi thieu: " << minExperience << " thang" << endl;
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
        cout << "9. Cap nhat ky nang yeu cau\n";
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

            case 9:
                cout << "Nhap ky nang yeu cau moi: ";
                requiredSkills.clear();
                inputRequiredSkills();
                break;

            case 0:
                //cout << "Cap nhat cong viec thanh cong!\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while (choice != 0);
}

const vector<Skill>& Job::getRequiredSkill() const {
    return requiredSkills;
}

const vector<CertificateRequirementGroup>& Job::getCertificateGroups() const {
    return certificateGroups;
}

bool Job::isExpired() const {
    int year, month, day;
    if (sscanf(deadline.c_str(), "%d-%d-%d",
               &year, &month, &day) != 3) {
        return false;
    }
    time_t now = time(0);
    tm* today = localtime(&now);
    if (today == NULL) {
        return false;
    }
    int currentYear = today->tm_year + 1900;
    int currentMonth = today->tm_mon + 1;
    int currentDay = today->tm_mday;
    if (year < currentYear) 
        return true;
    if (year == currentYear && month < currentMonth) 
        return true;
    if (year == currentYear && month == currentMonth && day < currentDay) 
        return true;
    return false;
}

bool Job::isEligible(const Candidate& candidate) const {
    if (candidate.getGPACandidate() < minGPA) {
        return false;
    }
    if (candidate.getTimeExperience() < minExperience) {
        return false;
    }
    if (!nameMajor.empty() && candidate.getProfessionalInfo().major != nameMajor) {
        return false;
    }
    const vector<Skill>& candidateSkills = candidate.getSkill();
    for (const Skill& required : requiredSkills) {
        bool found = false;
        for (const Skill& skill : candidateSkills) {
            if (skill.name == required.name &&
                skill.level >= required.level) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    const vector<Certificate>& candidateCertificates = candidate.getCertificate();
    for (const CertificateRequirementGroup& group : certificateGroups) {
        if (!group.isRequired) {
            continue;
        }
        bool found = false;
        for (const CertificateRequirement& requirement : group.alternatives) {
            for (const Certificate& certificate : candidateCertificates) {
                if (certificate.certificateTypeID ==
                        requirement.certificateTypeID &&
                    certificate.value >= requirement.minValue) {
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}