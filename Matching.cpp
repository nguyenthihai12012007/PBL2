#include "class.h"
#include "Matching.h"
#include <unordered_map>
#include <ctime>
#include <iostream>
using namespace std;

double calculaterSkillMath(const Candidate& candidate,const Job& job) {
    const vector<Skill>& candidateSkills = candidate.getSkill();
    const vector<Skill>& requiredSkills = job.getRequiredSkill();

    unordered_map<string, int> skillLevels;

    for(const Skill& skill : candidateSkills) {
        skillLevels[skill.name] = skill.level;
    }

    double totalScore = 0.0;

    for (const Skill& requiredSkill : requiredSkills) {
        auto it = skillLevels.find(requiredSkill.name);

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

    return candidateMajor == requiredMajor;
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

    return scoreTotal / scoreWeight;
}

void Application::displayApplication() const {
    cout << "\n===== CHI TIET DON UNG TUYEN =====\n";
    cout << "Ma don ung tuyen: " << IDappli << endl;
    cout << "Ma ung vien: " << IDcandidate << endl;
    cout << "Ma cong viec: " << IDjob << endl;
    cout << "Muc do phu hop: " << appropriateLevel << "%" << endl;
    cout << "Trang thai: " << status << endl;
    cout << "Ngay ung tuyen: " << appliedAt << endl;
}