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
