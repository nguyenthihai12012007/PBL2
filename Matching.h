#ifndef MATCHING_H
#define MATCHING_H

#include "class.h"

struct CertificateMatchResult {
    bool eligible = true;
    bool hasPreferenceScore = false;
    double score = 0.0;
};

double calculaterSkillMath(
    const Candidate& candidate, const Job& job
);

CertificateMatchResult caculaterCertificateMath(
    const Candidate& candidate, const Job& job
);

double caculaterExperience(
    const Candidate& candidate, const Job& job
);

#endif