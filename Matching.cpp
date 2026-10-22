#include "class.h"
#include "matching.h"
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
    double scoreSkill = 100.0;
    double scoreExperience = 100.0;
    double scoreCertificate = 100.0;
    double scoreExtra = 100.0;

    if (!job.getRequiredSkill().empty()) {
        scoreSkill = calculaterSkillMath(candidate, job);
    }

    CertificateMatchResult certificateResult = caculaterCertificateMath(candidate, job);

    if (!job.getCertificateGroups().empty()) {
        scoreCertificate = certificateResult.score;
    }

    if (!job.getMinExperience() > 0) {
        scoreExperience = caculaterExperience(candidate, job);
    }

    if (job.getMinGPA() > 0) {
        scoreExtra = calculaterExtra(candidate, job);
    }

    return scoreSkill * 0.5 + scoreExperience * 0.3 + scoreCertificate * 0.1 + scoreExtra * 0.1;
}
