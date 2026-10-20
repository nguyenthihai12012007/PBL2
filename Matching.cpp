#include "class.h"
#include "matching.h"
#include <unordered_map>
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