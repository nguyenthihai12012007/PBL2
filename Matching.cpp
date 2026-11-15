#include "class.h"
#include "Matching.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <algorithm>

using namespace std;

string normalizeSkillName(const string& str) {
    string result = str;
    while (!result.empty() && isspace(static_cast<unsigned char>(result[0]))) {
        result.erase(0, 1);
    }
    while (!result.empty() && isspace(static_cast<unsigned char>(result[result.size() - 1]))) {
        result.erase(result.size() - 1, 1);
    }
    for (int i = 0; i < result.size(); i++) {
        result[i] = tolower(static_cast<unsigned char>(result[i]));
    }
    return result;
}

bool isSameSkill(const string& skill1, const string& skill2) {
    return normalizeSkillName(skill1) == normalizeSkillName(skill2);
}

bool isSameMajor(const string& major1, const string& major2) {
    string m1 = normalizeSkillName(major1);
    string m2 = normalizeSkillName(major2);
    return m1 == m2;
}

string formatMajorDisplay(const string& major) {
    if (major.empty()) return "";
    static const vector<string> standardMajors = {
        "Công nghệ thông tin", "Kỹ thuật phần mềm", "Khoa học máy tính",
        "Hệ thống thông tin", "Trí tuệ nhân tạo", "Mạng máy tính",
        "An toàn thông tin", "Khoa học dữ liệu"
    };
    string norm = normalizeSkillName(major);
    for (const auto& sm : standardMajors) {
        if (normalizeSkillName(sm) == norm) {
            return sm;
        }
    }
    return major;
}

bool isValidPhoneNumber(const string& phone) {
    if (phone.empty()) return false;
    for (char c : phone) {
        if (c < '0' || c > '9') return false;
    }
    if (phone[0] != '0') return false;
    if (phone.length() != 10 && phone.length() != 11) return false;
    return true;
}

bool isValidDateOfBirth(const string& dateStr) {
    if (dateStr.empty()) return false;
    int day, month, year;
    if (dateStr.find('/') != string::npos) {
        if (sscanf(dateStr.c_str(), "%d/%d/%d", &day, &month, &year) != 3)
            return false;
    }
    else {
        if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) != 3)
            return false;
    }
    if (year < 1900 || month < 1 || month > 12)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
        daysInMonth[1] = 29;
    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    time_t now = time(nullptr);
    tm* today = localtime(&now);
    if (today == nullptr) return false;

    int currentYear = today->tm_year + 1900;
    int currentMonth = today->tm_mon + 1;
    int currentDay = today->tm_mday;

    if (year > currentYear) return false;
    if (year == currentYear && month > currentMonth) return false;
    if (year == currentYear && month == currentMonth && day > currentDay) return false;

    return true;
}

string formatDateDisplay(const string& dateStr) {
    if (dateStr.empty())
        return "Chưa cập nhật";
    int day, month, year;
    if (dateStr.find('-') != string::npos) {
        if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) != 3)
            return dateStr;
    }
    else if (dateStr.find('/') != string::npos) {
        if (sscanf(dateStr.c_str(), "%d/%d/%d", &day, &month, &year) != 3)
            return dateStr;
    }
    else {
        return dateStr;
    }
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", day, month, year);

    return string(buffer);
}

double calculaterSkillMath(const Candidate& candidate,const Job& job) {
    const vector<Skill>& candidateSkills = candidate.getSkill();
    const vector<Skill>& requiredSkills = job.getRequiredSkill();

    if (requiredSkills.empty()) {
        return 100.0;
    }

    unordered_map<string, int> skillLevels;
    for (const Skill& skill : candidateSkills) {
        string key = normalizeSkillName(skill.name);
        if (!key.empty()) {
            if (skillLevels.find(key) == skillLevels.end() || skill.level > skillLevels[key]) {
                skillLevels[key] = skill.level;
            }
        }
    }

    double totalScore = 0.0;
    for (const Skill& requiredSkill : requiredSkills) {
        string reqKey = normalizeSkillName(requiredSkill.name);
        auto it = skillLevels.find(reqKey);
        if (it != skillLevels.end()) {
            double score = double(it->second) / requiredSkill.level;
            if (score > 1.0) {
                score = 1.0;
            }
            totalScore += score;
        }
    }
    return 100.0 * totalScore / requiredSkills.size(); 
}

double caculaterExperience (const Candidate& candidate, const Job& job) {
    int timeRequired = job.getMinExperience();
    int timeCandidate = candidate.getTimeExperience();
    if (timeRequired == 0) {
        return 100.0;
    }
    double score = 100.0 * timeCandidate / timeRequired;
    if (score > 100.0) {
        return 100.0;
    }

    return score;
}

bool meetsCertificateRequirement( const vector<Certificate>& certificates, const CertificateRequirement& requirement) {
    for (const Certificate& certificate : certificates) {
        if (certificate.certificateTypeID == requirement.certificateTypeID
            && certificate.value >= requirement.minValue) {
            return true;
        }
    }

    return false;
}

// IELTS >= 6.0 HOAC TOEIC >= 750.
bool meetsCertificateGroup( const vector<Certificate>& certificates, const CertificateRequirementGroup& group) {
    for (const CertificateRequirement& requirement : group.alternatives) {
        if (meetsCertificateRequirement(certificates, requirement)) {
            return true;
        }
    }

    return false;
}

CertificateMatchResult caculaterCertificateMath( const Candidate& candidate, const Job& job) 
{
    CertificateMatchResult result;

    const vector<Certificate>& certificates = candidate.getCertificate();

    const vector<CertificateRequirementGroup>& groups = job.getCertificateGroups();

    int totalPreferredGroups = 0;
    int matchedPreferredGroups = 0;

    for (const CertificateRequirementGroup& group : groups) {
        bool matched = meetsCertificateGroup(certificates, group);

        if (group.isRequired) {
            if (!matched) {
                result.eligible = false;
            }
        } else { ++totalPreferredGroups;
            if (matched) {
                ++matchedPreferredGroups;
            }
        }
    }

    if (totalPreferredGroups > 0) {
        result.hasPreferenceScore = true;

        result.score = 100.0 * matchedPreferredGroups / totalPreferredGroups;
    }

    return result;
}

double calculaterExtra (const Candidate& candidate, const Job& job) {
    double GPACandidate = candidate.getGPACandidate();
    float GPA = job.getMinGPA();

    if (GPA == 0) {
        return 100.0;
    }

    double score = GPACandidate / GPA;
    
    if (score >= 1) {
        return 100.0;
    }

    return score*100.0;
}

bool isJobOpen(const Job& job) {
    if (!job.getStatus()) {
        return false;
    }

    time_t now = time(nullptr);
    tm* currentTime = localtime(&now);

    if (currentTime == nullptr) {
        return false;
    }

    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", currentTime);

    return job.getDeadline() >= string(today);
}

bool meetsMajorRequirement(const Candidate& candidate, const Job& job) {
    string candidateMajor = candidate.getProfessionalInfo().major;
    string requiredMajor = job.getNameMajor();

    if (requiredMajor.empty()) {
        return true;
    }
    return isSameMajor(candidateMajor, requiredMajor);
}

bool check (const Candidate& candidate, const Job& job) {
    if (!isJobOpen(job)) {
        return false;
    }

    if (!meetsMajorRequirement(candidate, job)) {
        return false;
    }

    const vector<Certificate>& certificates = candidate.getCertificate();

    const vector<CertificateRequirementGroup>& groups = job.getCertificateGroups();

    for (const CertificateRequirementGroup& group : groups) {
        if (group.isRequired) {
            if (!meetsCertificateGroup(certificates, group)) {
                return false;
            }
        }
    }

    return true;
}

double calculatorTotal (const Candidate& candidate, const Job& job) {

    if (!check(candidate, job)) {
        return -1.0;
    }

    double scoreTotal = 0.0;
    double scoreWeight = 0.0;

    if (!job.getRequiredSkill().empty()) {
        scoreTotal += calculaterSkillMath(candidate, job) * 0.5;
        scoreWeight += 0.5;
    }

    CertificateMatchResult certificateResult = caculaterCertificateMath(candidate, job);

    if (certificateResult.hasPreferenceScore) {
        scoreTotal += certificateResult.score * 0.1;
        scoreWeight += 0.1;
    }

    if (job.getMinExperience() > 0) {
        scoreTotal += caculaterExperience(candidate, job) * 0.3;
        scoreWeight += 0.3;
    }

    if (job.getMinGPA() > 0) {
        scoreTotal += calculaterExtra(candidate, job) * 0.1;
        scoreWeight += 0.1; 
    }

    if (scoreWeight <= 0.0001) {
        return 100.0;
    }

    return scoreTotal / scoreWeight;
}

bool compareJobMatch(const JobMatchResult& a, const JobMatchResult& b) {
    return a.score > b.score;
}

vector<JobMatchResult> matching(const vector<Job>& jobs, const Candidate& candidate) {
    vector<JobMatchResult> satisfiedJob;

    for (int i = 0; i < jobs.size(); i++) {
        double score = calculatorTotal(candidate, jobs[i]);

        if (score >= 70.0) {
            JobMatchResult result;
            result.job = jobs[i];
            result.score = score;

            satisfiedJob.push_back(result);
        }
    }
    sort(satisfiedJob.begin(), satisfiedJob.end(), compareJobMatch);

    return satisfiedJob;
}