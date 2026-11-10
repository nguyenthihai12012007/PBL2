#ifndef COMPANY_PAGE_H
#define COMPANY_PAGE_H

#include "class.h"
#include "JobSearch.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>

class QLabel;
class QPushButton;
class QScrollArea;

class CompanyPage : public QWidget {
    Q_OBJECT

public:
    explicit CompanyPage(Data &data, JobSearchEngine &searchEngine, QWidget *parent = nullptr);
    ~CompanyPage() override = default;

    void setSession(int userId);
    void filterContent(const QString &keyword);
    void showApplicantsDialog(int jobId = 0, const QString &jobTitle = "");

signals:
    void viewJobRequested(int jobId);
    void viewApplicantsRequested(int jobId, const QString &jobTitle);

public slots:
    void onCreateJobClicked();
    void onEditJobClicked(int jobId);
    void onEditCompanyProfileClicked();
    void onRefreshData();
    void onChangeLogoClicked();
    void viewCandidateDetails(int candidateId, const QString &jobTitle, float score, int appId = 0, const QString &currentStatus = "", int jobId = 0);
    bool processApplicationDecision(int appId, int jobId, const QString &candidateName, const QString &newStatus, QWidget *parentDialog = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void populateDashboard();
    void buildCompanyProfileHeader();
    void buildJobsList();
    void buildLatestApplicantsCard();

    Data &m_data;
    JobSearchEngine &m_searchEngine;
    int m_userId = 0;
    Company m_company;
    std::vector<Job> m_jobs;
    std::vector<Application> m_applications;
    QString m_filterKeyword;
    QLabel *companyLogoLabel = nullptr;

    // UI layouts
    QVBoxLayout *companyHeaderLayout = nullptr;
    QVBoxLayout *jobsListLayout = nullptr;
    QVBoxLayout *latestApplicantsLayout = nullptr;
};

#endif // COMPANY_PAGE_H
