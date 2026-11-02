#ifndef JOB_SEARCH_H
#define JOB_SEARCH_H

#include "class.h"
#include "Matching.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct JobSearchFilter {
    string keyword = "";
    string location = "";
    string major = "";
    bool onlyOpenJobs = true;
};

struct SearchResultItem {
    Job job;
    Company company;
    double matchScore = -1.0;
    bool isEligible = false;
    bool hasApplied = false;
    string applyStatus = "";
};

enum class ApplyStatus {
    SUCCESS,
    CANDIDATE_NOT_FOUND,
    JOB_NOT_FOUND,
    JOB_CLOSED_OR_EXPIRED,
    ALREADY_APPLIED,
    NOT_ELIGIBLE,
    CALCULATION_ERROR,
    SAVE_FAILED
};

struct ApplyResult {
    ApplyStatus status = ApplyStatus::JOB_NOT_FOUND;
    Application application;
    Job job;
    double matchScore = -1.0;
    string message;
    bool isSuccess() const {
        return status == ApplyStatus::SUCCESS;
    }
};

class JobSearchEngine {
private:
    vector<Job> m_allJobs;
    unordered_map<int, Job> m_jobMap;
    unordered_map<int, Company> m_companyMap;
    unordered_map<string, unordered_set<int>> m_keywordIndex;
    vector<string> m_vocabulary;
    unordered_map<int, unordered_map<int, Application>> m_candJobAppMap;
    unordered_map<int, vector<Application>> m_appsByCandidate;
    unordered_map<int, int> m_applicantCountByJob;

    void buildInvertedIndex();
    void loadApplications(Data& data);

public:
    JobSearchEngine() = default;
    static string normalizeText(const string& input);
    static vector<string> tokenize(const string& input);
    void reload(Data& data);
    vector<SearchResultItem> searchForCandidate(const JobSearchFilter& filter, const Candidate& candidate);
    vector<Job> searchForCompany(int companyId, const string& keyword);
    bool hasCandidateApplied(int candidateId, int jobId, Application& outApp) const;
    vector<Application> getApplicationsForCandidate(int candidateId) const;
    int countApplicantsForJob(int jobId) const;
    void recordApplication(int candidateId, int jobId, const Application& app);
    void updateApplicationStatusInIndex(int applicationId, const string& newStatus);
    ApplyResult applyJob(int candidateUserId, int jobId, Data& data);
};

#endif