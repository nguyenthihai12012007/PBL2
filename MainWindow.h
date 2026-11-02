#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QList>
#include "class.h"
#include "JobSearch.h"

class QStackedWidget;
class QLabel;
class QPushButton;
class QLineEdit;
class QVBoxLayout;
class CandidatePage;
class CompanyPage;
class JobPage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Data &data, QWidget *parent = nullptr);
    ~MainWindow() override = default;

    void setSession(int userId, const QString &email, const QString &role);

signals:
    void logoutRequested();

private slots:
    void onLogoutClicked();
    void onShowJobDetail(int jobId);
    void onBackFromJobPage();
    void onSearchTextChanged(const QString &text);
    void onApplyJobRequested(int jobId);
    void showCandidateProfileDialog();
    void showCandidateApplicationsDialog();
    void showCompanyProfileDialog();
    void showNotificationsDialog();
    void showSettingsDialog();

private:
    void setupUI();
    void updateNavigationForRole(const QString &role);

    Data &m_data;
    JobSearchEngine m_searchEngine;
    int currentUserId = 0;
    QString currentEmail;
    QString currentRole;
    int previousPageIndex = 0;

    // Sidebar Widgets
    QLabel *sidebarBrandTitle = nullptr;
    QLabel *sidebarSubtitle = nullptr;
    QVBoxLayout *navButtonsLayout = nullptr;
    QList<QPushButton*> navButtons;

    // Top Bar Widgets
    QLineEdit *searchEdit = nullptr;
    QLabel *avatarCircleLabel = nullptr;
    QLabel *topUserNameLabel = nullptr;
    QLabel *topUserRoleLabel = nullptr;

    // Content Widgets
    QStackedWidget *stackedWidget = nullptr;
    CandidatePage *candidatePage = nullptr;
    CompanyPage *companyPage = nullptr;
    JobPage *jobPage = nullptr;
};

#endif // MAIN_WINDOW_H
