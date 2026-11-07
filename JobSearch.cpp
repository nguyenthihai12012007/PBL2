#include "JobSearch.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>

using namespace std;

string JobSearchEngine::normalizeText(const string& input) {
    static const unordered_map<string, char> vietnameseMap = []() {
        unordered_map<string, char> mp;
        const vector<pair<string, char>> groups = {
            {u8"ÀÁẠẢÃĂẰẮẶẲẴÂẦẤẬẨẪàáạảãăằắặẳẵâầấậẩẫ", 'a'},
            {u8"ÈÉẸẺẼÊỀẾỆỂỄèéẹẻẽêềếệểễ", 'e'},
            {u8"ÌÍỊỈĨìíịỉĩ", 'i'},
            {u8"ÒÓỌỎÕÔỒỐỘỔỖƠỜỚỢỞỠòóọỏõôồốộổỗơờớợởỡ", 'o'},
            {u8"ÙÚỤỦŨƯỪỨỰỬỮùúụủũưừứựửữ", 'u'},
            {u8"ỲÝỴỶỸỳýỵỷỹ", 'y'},
            {u8"Đđ", 'd'}
        };
        for (const auto& group : groups) {
            const string& chars = group.first;
            char base = group.second;
            for (size_t i = 0; i < chars.size();) {
                unsigned char c = static_cast<unsigned char>(chars[i]);
                size_t len = 1;
                if ((c & 0xE0) == 0xC0) len = 2;
                else if ((c & 0xF0) == 0xE0) len = 3;
                else if ((c & 0xF8) == 0xF0) len = 4;
                mp[chars.substr(i, len)] = base;
                i += len;
            }
        }
        return mp;
    }();
    
    string result;
    for (size_t i = 0; i < input.size();) {
        unsigned char c = static_cast<unsigned char>(input[i]);
        if (c < 128) {
            if (c >= 'A' && c <= 'Z') {
                c += ('a' - 'A');
            }
            result.push_back(static_cast<char>(c));
            i++;
        }
        else {
            size_t len = 1;
            if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            string character = input.substr(i, len);
            auto it = vietnameseMap.find(character);
            if (it != vietnameseMap.end()) {
                result.push_back(it->second);
            }
            else {
                result += character;
            }
            i += len;
        }
    }
    return result;
}

vector<string> JobSearchEngine::tokenize(const string& input) {
    string text = normalizeText(input);
    vector<string> tokens;
    string word;
    for (size_t i = 0; i < text.size(); i++) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (isalnum(c) || c == '+' || c == '#') {
            word.push_back(static_cast<char>(c));
        }
        else {
            if (!word.empty()) {
                tokens.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty()) {
        tokens.push_back(word);
    }
    return tokens;
}

void JobSearchEngine::buildInvertedIndex() {
    m_keywordIndex.clear();
    m_vocabulary.clear();
    unordered_set<string> allWords;

    for (const Job& job : m_allJobs) {
        string content;
        content += job.getTitle() + " ";
        content += job.getLocation() + " ";
        content += job.getNameMajor() + " ";
        content += job.getDescription() + " ";
        int companyId = job.getIDcompany();
        auto companyIt = m_companyMap.find(companyId);
        if (companyIt != m_companyMap.end()) {
            content += companyIt->second.getNameCompany() + " ";
        }
        for (const auto& skill : job.getRequiredSkill()) {
            content += skill.name + " ";
        }
        vector<string> words = tokenize(content);
        for (const string& word : words) {
            m_keywordIndex[word].insert(job.getIDjob());
            allWords.insert(word);
        }
    }
    m_vocabulary.assign(allWords.begin(), allWords.end());
}

void JobSearchEngine::loadApplications(Data& data) {
    m_candJobAppMap.clear();
    m_appsByCandidate.clear();
    m_applicantCountByJob.clear();

    vector<string> lines;
    if (!data.readFile("applications.txt", lines)) {
        return;
    }
    for (const string& line : lines) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        stringstream ss(line);
        string appID;
        string candidateID;
        string jobID;
        string level;
        string appliedAt;
        string status;

        if (!getline(ss, appID, '|')) continue;
        if (!getline(ss, candidateID, '|')) continue;
        if (!getline(ss, jobID, '|')) continue;
        if (!getline(ss, level, '|')) continue;
        if (!getline(ss, appliedAt, '|')) continue;
        if (!getline(ss, status)) continue;

        try {
            int id = stoi(appID);
            int candId = stoi(candidateID);
            int jId = stoi(jobID);
            float score = stof(level);
            if (m_candJobAppMap[candId].count(jId) > 0) {
                continue;
            }
            Application app(id, candId, jId, score, status, appliedAt);
            m_candJobAppMap[candId][jId] = app;
            m_appsByCandidate[candId].push_back(app);
            m_applicantCountByJob[jId]++;
        }
        catch (...) {
            continue;
        }
    }
}

void JobSearchEngine::reload(Data& data) {
    m_allJobs.clear();
    m_jobMap.clear();
    m_companyMap.clear();
    m_keywordIndex.clear();
    m_vocabulary.clear();

    data.getAllJobs(m_allJobs);
    for (const Job& job : m_allJobs) {
        m_jobMap[job.getIDjob()] = job;
    }
    for (const Job& job : m_allJobs) {
        int companyId = job.getIDcompany();
        if (m_companyMap.count(companyId) == 0) {
            Company company;
            if (data.getCompanyByID(companyId, company)) {
                m_companyMap[companyId] = company;
            }
        }
    }
    buildInvertedIndex();
    loadApplications(data);
}

vector<SearchResultItem> JobSearchEngine::searchForCandidate(const JobSearchFilter& filter, const Candidate& candidate) {
    unordered_set<int> matchedJobIDs;
    vector<string> keywords = tokenize(filter.keyword);
    if (keywords.empty()) {
        for (const Job& job : m_allJobs) {
            matchedJobIDs.insert(job.getIDjob());
        }
    }
    else {
        bool firstKeyword = true;
        for (const string& keyword : keywords) {
            unordered_set<int> currentIDs;
            auto it = m_keywordIndex.find(keyword);
            if (it != m_keywordIndex.end()) {
                currentIDs = it->second;
            }
            for (const string& word : m_vocabulary) {
                if (word.find(keyword) != string::npos) {
                    auto wordIt = m_keywordIndex.find(word);
                    if (wordIt != m_keywordIndex.end()) {
                        currentIDs.insert(wordIt->second.begin(), wordIt->second.end());
                    }
                }
            }
            if (firstKeyword) {
                matchedJobIDs = currentIDs;
                firstKeyword = false;
            }
            else {
                unordered_set<int> newIDs;
                for (int id : matchedJobIDs) {
                    if (currentIDs.count(id) > 0) {
                        newIDs.insert(id);
                    }
                }
                matchedJobIDs = newIDs;
            }
            if (matchedJobIDs.empty()) {
                break;
            }
        }
    }

    unordered_map<int, double> scoreMap;
    int candidateID = candidate.getIDcandidate();
    if (candidateID > 0) {
        vector<JobMatchResult> matchingResults = matching(m_allJobs, candidate);
        for (const JobMatchResult& result : matchingResults) {
            scoreMap[result.job.getIDjob()] = result.score;
        }
    }

    vector<SearchResultItem> results;
    for (int jobID : matchedJobIDs) {
        auto jobIt = m_jobMap.find(jobID);
        if (jobIt == m_jobMap.end()) {
            continue;
        }
        const Job& job = jobIt->second;
        if (filter.onlyOpenJobs) {
            if (!job.getStatus() || job.isExpired()) {
                continue;
            }
        }
        if (!filter.major.empty()) {
            if (normalizeText(job.getNameMajor()) != normalizeText(filter.major)) {
                continue;
            }
        }
        if (!filter.location.empty()) {
            string jobLocation = normalizeText(job.getLocation());
            string searchLocation = normalizeText(filter.location);
            if (jobLocation.find(searchLocation) == string::npos) {
                continue;
            }
        }

        SearchResultItem item;
        item.job = job;
        int companyID = job.getIDcompany();
        auto companyIt = m_companyMap.find(companyID);
        if (companyIt != m_companyMap.end()) {
            item.company = companyIt->second;
        }

        Application app;
        if (candidateID > 0 && hasCandidateApplied(candidateID, jobID, app)) {
            item.hasApplied = true;
            item.applyStatus = app.getStatus();
        }
        auto scoreIt = scoreMap.find(jobID);
        if (scoreIt != scoreMap.end()) {
            item.matchScore = scoreIt->second;
            item.isEligible = isfinite(item.matchScore) && item.matchScore >= 0.0;
        }
        else if (candidateID > 0) {
            double score = calculatorTotal(candidate, job);
            if (isfinite(score) && score >= 0.0) {
                item.matchScore = score;
                item.isEligible = true;
            }
            else {
                item.matchScore = -1.0;
                item.isEligible = false;
            }
        }
        else {
            item.matchScore = -1.0;
            item.isEligible = false;
        }
        results.push_back(item);
    }

    sort(results.begin(), results.end(), [](const SearchResultItem& a, const SearchResultItem& b) {
            if (a.isEligible != b.isEligible) {
                return a.isEligible > b.isEligible;
            }
            return a.matchScore > b.matchScore;
        }
    );
    return results;
}

vector<Job> JobSearchEngine::searchForCompany(int companyId, const string& keyword) {
    vector<Job> results;
    string searchText = normalizeText(keyword);
    for (const Job& job : m_allJobs) {
        if (job.getIDcompany() != companyId) {
            continue;
        }
        if (searchText.empty()) {
            results.push_back(job);
            continue;
        }
        string title = normalizeText(job.getTitle());
        string location = normalizeText(job.getLocation());
        string major = normalizeText(job.getNameMajor());
        if (title.find(searchText) != string::npos || location.find(searchText) != string::npos || major.find(searchText) != string::npos) {
            results.push_back(job);
        }
    }
    return results;
}

bool JobSearchEngine::hasCandidateApplied(int candidateId, int jobId, Application& outApp) const {
    auto candidateIt = m_candJobAppMap.find(candidateId);
    if (candidateIt == m_candJobAppMap.end()) {
        return false;
    }
    auto jobIt = candidateIt->second.find(jobId);
    if (jobIt == candidateIt->second.end()) {
        return false;
    }
    outApp = jobIt->second;
    return true;
}

vector<Application>
JobSearchEngine::getApplicationsForCandidate(int candidateId) const {
    auto it = m_appsByCandidate.find(candidateId);
    if (it == m_appsByCandidate.end()) {
        return {};
    }
    return it->second;
}

int JobSearchEngine::countApplicantsForJob(int jobId) const {
    auto it = m_applicantCountByJob.find(jobId);
    if (it == m_applicantCountByJob.end()) {
        return 0;
    }
    return it->second;
}

void JobSearchEngine::recordApplication(int candidateId, int jobId, const Application& app) {
    if (candidateId <= 0 || jobId <= 0 || app.getIDappli() <= 0) {
        return;
    }
    for (const auto& candidatePair : m_candJobAppMap) {
        for (const auto& jobPair : candidatePair.second) {
            if (jobPair.second.getIDappli() == app.getIDappli()) {
                if (candidatePair.first == candidateId && jobPair.first == jobId) {
                    continue;
                }
                return;
            }
        }
    }
    auto& applications = m_candJobAppMap[candidateId];
    auto it = applications.find(jobId);
    if (it != applications.end()) {
        it->second = app;
        bool found = false;
        for (Application& existing : m_appsByCandidate[candidateId]) {
            if (existing.getIDjob() == jobId) {
                existing = app;
                found = true;
                break;
            }
        }
        if (!found) {
            m_appsByCandidate[candidateId].push_back(app);
        }
    }
    else {
        applications[jobId] = app;
        m_appsByCandidate[candidateId].push_back(app);
        m_applicantCountByJob[jobId]++;
    }
}

void JobSearchEngine::updateApplicationStatusInIndex(int applicationId, const string& newStatus) {
    for (auto& candidatePair : m_candJobAppMap) {
        for (auto& jobPair : candidatePair.second) {
            if (jobPair.second.getIDappli() == applicationId) {
                jobPair.second.setStatus(newStatus);
            }
        }
    }
    for (auto& candidatePair : m_appsByCandidate) {
        for (Application& app : candidatePair.second) {
            if (app.getIDappli() == applicationId) {
                app.setStatus(newStatus);
            }
        }
    }
}

ApplyResult JobSearchEngine::applyJob(int candidateUserId, int jobId, Data& data) {
    ApplyResult result;
    if (candidateUserId <= 0) {
        result.status = ApplyStatus::CANDIDATE_NOT_FOUND;
        result.message = "Mã tài khoản người dùng không hợp lệ.";
        return result;
    }

    Candidate candidate;
    if (!data.getCandidateByUserID(candidateUserId, candidate) || candidate.getIDcandidate() <= 0) {
        result.status = ApplyStatus::CANDIDATE_NOT_FOUND;
        result.message = "Không tìm thấy hồ sơ ứng viên hợp lệ liên kết với tài khoản này.";
        return result;
    }

    int candidateId = candidate.getIDcandidate();
    data.loadCandidateSkill(candidate);
    data.loadCandidateCertificate(candidate);
    data.loadCandidateExperience(candidate);

    if (jobId <= 0) {
        result.status = ApplyStatus::JOB_NOT_FOUND;
        result.message = "Mã công việc không hợp lệ.";
        return result;
    }

    Job job;
    if (!data.getJobByID(jobId, job) || job.getIDjob() <= 0) {
        result.status = ApplyStatus::JOB_NOT_FOUND;
        result.message = "Không thể xác định thông tin công việc để nộp đơn.";
        return result;
    }
    result.job = job;
    if (!job.getStatus() || job.isExpired()) {
        result.status = ApplyStatus::JOB_CLOSED_OR_EXPIRED;
        result.message = "Tin tuyển dụng này đã đóng hoặc đã hết hạn nộp hồ sơ.";
        return result;
    }

    Application existingApp;
    if (hasCandidateApplied(candidateId, jobId, existingApp)) {
        result.status = ApplyStatus::ALREADY_APPLIED;
        result.application = existingApp;
        result.message = "Bạn đã nộp đơn ứng tuyển vào vị trí này trước đó rồi.";
        return result;
    }

    double rawScore = calculatorTotal(candidate, job);
    if (!std::isfinite(rawScore)) {
        result.status = ApplyStatus::CALCULATION_ERROR;
        result.message = "Hệ thống chưa thể tính điểm phù hợp cho vị trí này.";
        return result;
    }

    if (rawScore < 0.0) {
        result.status = ApplyStatus::NOT_ELIGIBLE;
        result.matchScore = rawScore;
        result.message = "Hồ sơ của bạn không đáp ứng điều kiện tiên quyết của vị trí này.";
        return result;
    }

    float matchScore = static_cast<float>(rawScore);
    result.matchScore = rawScore;

    int newAppId = data.getNextID("applications.txt");
    time_t now = time(nullptr);
    tm* currentTime = localtime(&now);
    char todayBuf[32] = {0};
    if (currentTime != nullptr) {
        strftime(todayBuf, sizeof(todayBuf), "%Y-%m-%d", currentTime);
    }
    string today = todayBuf;

    Application newApp(newAppId, candidateId, jobId, matchScore, "PENDING", today);
    if (!data.saveApplication(newApp)) {
        result.status = ApplyStatus::SAVE_FAILED;
        result.message = "Không thể lưu đơn ứng tuyển vào cơ sở dữ liệu applications.txt.";
        return result;
    }

    recordApplication(candidateId, jobId, newApp);

    result.status = ApplyStatus::SUCCESS;
    result.application = newApp;
    result.message = "Ứng tuyển thành công.";
    return result;
}