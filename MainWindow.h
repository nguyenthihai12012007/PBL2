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
    void showCandidateProfilePage();
    void showCompanyProfilePage();
    void showNotificationsDialog();
    void showSettingsDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void updateNavigationForRole(const QString &role);
    void setActiveNavBtn(QPushButton *activeBtn);
    void buildCandidateProfilePage();
    void buildCompanyProfilePage();

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
    QPushButton *btnHomeNav = nullptr;
    QPushButton *btnProfileNav = nullptr;

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
    QWidget *candidateProfilePageWidget = nullptr;
    QVBoxLayout *candidateProfileContentLayout = nullptr;
    QWidget *companyProfilePageWidget = nullptr;
    QVBoxLayout *companyProfileContentLayout = nullptr;
};

#endif // MAIN_WINDOW_H
