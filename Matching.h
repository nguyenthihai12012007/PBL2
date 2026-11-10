#ifndef MATCHING_H
#define MATCHING_H

#include "class.h"
#include <string>

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

double calculatorTotal (const Candidate& candidate, const Job& job);

// Chuẩn hóa và xác thực dữ liệu dùng chung trong hệ thống
bool isSameMajor(const std::string& major1, const std::string& major2);
std::string formatMajorDisplay(const std::string& major);
bool isValidPhoneNumber(const std::string& phone);
bool isValidDateOfBirth(const std::string& dateStr);
std::string formatDateDisplay(const std::string& dateStr);

#endif
