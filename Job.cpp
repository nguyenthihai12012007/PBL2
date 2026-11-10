#include "class.h"
#include "Matching.h"
#include <iostream>
#include <ctime>
#include <cstdio>
using namespace std;

Job::Job()
    : IDjob(0), IDcompany(0), salary(0), 
      minGPA(0), minExperience(0), status(true) {
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
    if (!nameMajor.empty() && !isSameMajor(candidate.getProfessionalInfo().major, nameMajor)) {
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