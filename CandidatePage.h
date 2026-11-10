#ifndef CANDIDATE_PAGE_H
#define CANDIDATE_PAGE_H

#include "class.h"
#include "JobSearch.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>

class QLabel;
class QPushButton;
class QProgressBar;
class QScrollArea;
class QLineEdit;
class QComboBox;
class QFrame;

class CandidatePage : public QWidget {
    Q_OBJECT

public:
    enum class ViewMode {
        AllJobs,     // Trang chủ: Tất cả việc làm còn hiệu lực (không lọc điểm matching)
        MatchedJobs  // Việc làm phù hợp: Chỉ việc làm đạt Matching >= 70%
    };

    explicit CandidatePage(Data &data, JobSearchEngine &searchEngine, QWidget *parent = nullptr);
    ~CandidatePage() override = default;

    void setSession(int userId);
    void filterJobs(const QString &keyword);
    void setViewMode(ViewMode mode);
    ViewMode getViewMode() const { return m_viewMode; }

signals:
    void viewJobRequested(int jobId);
    void viewAllApplicationsRequested();

public slots:
    void onRefreshData();
    void onEditProfileClicked();
    void onAddSkillClicked();
    void onAddCertificateClicked();
    void onAddExperienceClicked();
    void onEditSkillClicked(int index);
    void onDeleteSkillClicked(int index);
    void onEditCertificateClicked(int index);
    void onDeleteCertificateClicked(int index);
    void onEditExperienceClicked(int index);
    void onDeleteExperienceClicked(int index);
    void showAllJobsDialog();
    void onChangeAvatarClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void populateDashboard();
    void buildJobList();
    void buildProfileCard();
    void buildRecentApplicationsCard();
    int calculateProfileCompleteness() const;

    Data &m_data;
    JobSearchEngine &m_searchEngine;
    int m_userId = 0;
    Candidate m_candidate;
    std::vector<Job> m_jobs;
    std::vector<Application> m_applications;
    QString m_filterKeyword;
    QString m_filterLocation;
    QString m_filterMajor;
    ViewMode m_viewMode = ViewMode::AllJobs;

    // UI Containers
    QVBoxLayout *jobsListLayout = nullptr;
    QVBoxLayout *profileContentLayout = nullptr;
    QVBoxLayout *recentAppsLayout = nullptr;

    // Section Header & Search/Filter Widgets
    QLabel *secTitle = nullptr;
    QLabel *secSub = nullptr;
    QPushButton *viewAllBtn = nullptr;
    QFrame *filterBarCard = nullptr;
    QLineEdit *inpageSearchEdit = nullptr;
    QComboBox *inpageLocCombo = nullptr;
    QComboBox *inpageMajorCombo = nullptr;
    QPushButton *clearFilterBtn = nullptr;

    // Profile card widgets
    QLabel *avatarCircleLabel = nullptr;
    QLabel *candidateNameLabel = nullptr;
    QLabel *candidateTitleLabel = nullptr;
    QLabel *candidateLocationLabel = nullptr;
    QLabel *candidateEmailLabel = nullptr;
    QProgressBar *profileProgressBar = nullptr;
    QLabel *progressPercentLabel = nullptr;
    QHBoxLayout *skillsTagsLayout = nullptr;
};

#endif // CANDIDATE_PAGE_H
