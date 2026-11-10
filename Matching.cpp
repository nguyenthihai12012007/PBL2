#include "class.h"
#include "Matching.h"
#include "JobSearch.h"
#include <unordered_map>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <QString>
#include <QStringList>
#include <QDate>
#include <QRegularExpression>
using namespace std;

bool isSameMajor(const std::string& major1, const std::string& major2) {
    QString m1 = QString::fromStdString(major1).trimmed().simplified();
    QString m2 = QString::fromStdString(major2).trimmed().simplified();
    if (m1.isEmpty() && m2.isEmpty()) return true;
    if (m1.isEmpty() || m2.isEmpty()) return false;
    if (QString::compare(m1, m2, Qt::CaseInsensitive) == 0) return true;
    return JobSearchEngine::normalizeText(major1) == JobSearchEngine::normalizeText(major2);
}

std::string formatMajorDisplay(const std::string& major) {
    QString qm = QString::fromStdString(major).trimmed().simplified();
    if (qm.isEmpty()) return "";

    static const std::vector<QString> standardMajors = {
        "Công nghệ thông tin",
        "Kỹ thuật phần mềm",
        "Khoa học máy tính",
        "Hệ thống thông tin",
        "Trí tuệ nhân tạo",
        "Mạng máy tính",
        "An toàn thông tin",
        "Khoa học dữ liệu"
    };

    for (const auto& sm : standardMajors) {
        if (QString::compare(qm, sm, Qt::CaseInsensitive) == 0) {
            return sm.toStdString();
        }
    }

    QStringList words = qm.split(' ', Qt::SkipEmptyParts);
    for (int i = 0; i < words.size(); ++i) {
        QString w = words[i].toLower();
        if (!w.isEmpty()) {
            w[0] = w[0].toUpper();
            words[i] = w;
        }
    }
    return words.join(" ").toStdString();
}

bool isValidPhoneNumber(const std::string& phone) {
    if (phone.empty()) return false;
    for (char c : phone) {
        if (c < '0' || c > '9') return false;
    }
    if (phone[0] != '0') return false;
    if (phone.length() != 10 && phone.length() != 11) return false;
    return true;
}

bool isValidDateOfBirth(const std::string& dateStr) {
    if (dateStr.empty()) return false;
    QString qstr = QString::fromStdString(dateStr).trimmed();

    static const QRegularExpression bdayRegex("^(\\d{2})/(\\d{2})/(\\d{4})$");
    QRegularExpressionMatch match = bdayRegex.match(qstr);
    if (!match.hasMatch()) {
        return false;
    }

    QDate d = QDate::fromString(qstr, "dd/MM/yyyy");
    if (!d.isValid()) {
        return false;
    }

    if (d > QDate::currentDate() || d.year() < 1900) {
        return false;
    }

    return true;
}

std::string formatDateDisplay(const std::string& dateStr) {
    if (dateStr.empty()) return "Chưa cập nhật";
    QString qstr = QString::fromStdString(dateStr).trimmed();
    QDate d = QDate::fromString(qstr, "dd/MM/yyyy");
    if (!d.isValid()) {
        d = QDate::fromString(qstr, "yyyy-MM-dd");
    }
    if (d.isValid()) {
        return d.toString("dd/MM/yyyy").toStdString();
    }
    return qstr.toStdString();
}

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