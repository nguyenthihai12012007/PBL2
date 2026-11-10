#include "MainWindow.h"
#include "CandidatePage.h"
#include "CompanyPage.h"
#include "JobPage.h"
#include "StyleHelper.h"
#include "Matching.h"
#include "CustomMessageBox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QDialog>
#include <QScrollArea>
#include <QFormLayout>
#include <QTextEdit>
#include <QDate>
#include <QFileDialog>
#include <QComboBox>
#include <QListView>
#include <cmath>
#include <cctype>
#include <sstream>

namespace {
void safeClearLayout(QLayout *layout) {
    if (!layout) return;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        } else if (item->layout()) {
            safeClearLayout(item->layout());
        }
        delete item;
    }
}
}

MainWindow::MainWindow(Data &data, QWidget *parent)
    : QMainWindow(parent), m_data(data), currentUserId(0), previousPageIndex(0) {
    m_searchEngine.reload(m_data);
    setWindowTitle("JobMatch — Hệ thống Kết nối Tuyển dụng");
    resize(1260, 780);
    setMinimumSize(1024, 640);
    setupUI();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // =========================================================================
    // 1. SIDEBAR BÊN TRÁI: Màu cam đất Terracotta (#B95334)
    // =========================================================================
    QFrame *sidebar = new QFrame(centralWidget);
    sidebar->setObjectName("sidebarFrame");
    sidebar->setFixedWidth(240);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 24, 18, 20);
    sidebarLayout->setSpacing(18);

    // Brand Logo & Header
    QHBoxLayout *brandHeader = new QHBoxLayout();
    brandHeader->setSpacing(10);
    brandHeader->setAlignment(Qt::AlignVCenter);

    QLabel *logoImg = new QLabel(sidebar);
    logoImg->setFixedSize(38, 38);
    logoImg->setAlignment(Qt::AlignCenter);
    QPixmap lPix("avatars/jobmatch_logo.png");
    if (!lPix.isNull()) {
        logoImg->setPixmap(lPix.scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoImg->setText("💼");
        logoImg->setStyleSheet("background-color: rgba(255, 255, 255, 0.2); color: #FFFFFF; font-size: 16px; border-radius: 8px;");
    }

    QVBoxLayout *brandTextLayout = new QVBoxLayout();
    brandTextLayout->setSpacing(2);
    brandTextLayout->setAlignment(Qt::AlignVCenter);
    sidebarBrandTitle = new QLabel("JobMatch", sidebar);
    sidebarBrandTitle->setObjectName("sidebarTitle");
    sidebarSubtitle = new QLabel("Kết nối tài năng với cơ hội", sidebar);
    sidebarSubtitle->setObjectName("sidebarSubtitle");
    sidebarSubtitle->setWordWrap(true);
    brandTextLayout->addWidget(sidebarBrandTitle);
    brandTextLayout->addWidget(sidebarSubtitle);

    brandHeader->addWidget(logoImg, 0, Qt::AlignVCenter);
    brandHeader->addLayout(brandTextLayout);
    sidebarLayout->addLayout(brandHeader);

    // Navigation buttons container
    navButtonsLayout = new QVBoxLayout();
    navButtonsLayout->setSpacing(6);
    sidebarLayout->addLayout(navButtonsLayout);

    sidebarLayout->addStretch();

    // Nút Đăng xuất ở chân Sidebar
    QPushButton *logoutBtn = new QPushButton("🚪 Đăng xuất", sidebar);
    logoutBtn->setObjectName("logoutButton");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    sidebarLayout->addWidget(logoutBtn);

    rootLayout->addWidget(sidebar);

    // =========================================================================
    // 2. VÙNG NỘI DUNG PHẢI (Top Bar + Stacked Pages)
    // =========================================================================
    QWidget *rightContainer = new QWidget(centralWidget);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // TOP BAR
    QFrame *topBar = new QFrame(rightContainer);
    topBar->setObjectName("topBarFrame");
    topBar->setFixedHeight(64);

    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(28, 12, 28, 12);
    topBarLayout->setSpacing(16);

    // Khối ngày giờ ở bên trái top bar ở trang ứng viên
    topDatePill = new QFrame(topBar);
    topDatePill->setObjectName("topDatePill");
    topDatePill->setStyleSheet(
        "#topDatePill {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 18px;"
        "   padding: 5px 16px;"
        "}"
    );
    QHBoxLayout *datePillLay = new QHBoxLayout(topDatePill);
    datePillLay->setContentsMargins(0, 0, 0, 0);
    datePillLay->setSpacing(6);

    QDate curDate = QDate::currentDate();
    static const QString vietnameseDays[] = {"", "Thứ Hai", "Thứ Ba", "Thứ Tư", "Thứ Năm", "Thứ Sáu", "Thứ Bảy", "Chủ Nhật"};
    int dow = curDate.dayOfWeek();
    QString dowStr = (dow >= 1 && dow <= 7) ? vietnameseDays[dow] : "Hôm nay";
    QString dateStr = QString("📅  %1, ngày %2").arg(dowStr, curDate.toString("dd/MM/yyyy"));

    topDateLabel = new QLabel(dateStr, topDatePill);
    topDateLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #4A4A4A; background: transparent;");
    datePillLay->addWidget(topDateLabel);
    topDatePill->setVisible(false);
    topBarLayout->addWidget(topDatePill);

    // Thanh tìm kiếm Search Pill
    searchEdit = new QLineEdit(topBar);
    searchEdit->setObjectName("topSearchBar");
    searchEdit->setPlaceholderText("🔍  Tìm kiếm việc làm, công ty, kỹ năng...");
    searchEdit->setFixedWidth(380);
    topBarLayout->addWidget(searchEdit);

    topBarLayout->addStretch();

    // Icon Chuông thông báo
    QPushButton *bellBtn = new QPushButton("🔔", topBar);
    bellBtn->setFixedSize(36, 36);
    bellBtn->setCursor(Qt::PointingHandCursor);
    bellBtn->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 18px; font-size: 14px;");
    connect(bellBtn, &QPushButton::clicked, this, &MainWindow::showNotificationsDialog);
    topBarLayout->addWidget(bellBtn);

    // Cụm User Profile ở góc phải
    QHBoxLayout *userProfileLayout = new QHBoxLayout();
    userProfileLayout->setSpacing(10);
    userProfileLayout->setAlignment(Qt::AlignVCenter);

    avatarCircleLabel = new QLabel("ND", topBar);
    avatarCircleLabel->setObjectName("userAvatarCircle");
    avatarCircleLabel->setFixedSize(36, 36);
    avatarCircleLabel->setAlignment(Qt::AlignCenter);
    avatarCircleLabel->setCursor(Qt::PointingHandCursor);
    avatarCircleLabel->setToolTip("Nhấn để xem trang hồ sơ");
    avatarCircleLabel->installEventFilter(this);

    QVBoxLayout *userNameLayout = new QVBoxLayout();
    userNameLayout->setSpacing(1);
    userNameLayout->setAlignment(Qt::AlignVCenter);
    topUserNameLabel = new QLabel("Ứng viên ▾", topBar);
    topUserNameLabel->setObjectName("userNameTopLabel");
    topUserNameLabel->setCursor(Qt::PointingHandCursor);
    topUserNameLabel->setToolTip("Nhấn để xem trang hồ sơ");
    topUserNameLabel->installEventFilter(this);
    topUserRoleLabel = new QLabel("Ứng viên", topBar);
    topUserRoleLabel->setObjectName("userRoleTopLabel");
    userNameLayout->addWidget(topUserNameLabel);
    userNameLayout->addWidget(topUserRoleLabel);

    userProfileLayout->addWidget(avatarCircleLabel, 0, Qt::AlignVCenter);
    userProfileLayout->addLayout(userNameLayout);
    topBarLayout->addLayout(userProfileLayout);

    rightLayout->addWidget(topBar);

    // STACKED CONTENT AREA
    stackedWidget = new QStackedWidget(rightContainer);
    candidatePage = new CandidatePage(m_data, m_searchEngine, stackedWidget);
    companyPage = new CompanyPage(m_data, m_searchEngine, stackedWidget);
    jobPage = new JobPage(stackedWidget);

    // Dedicated Candidate Profile Page (Full view, Index 3)
    candidateProfilePageWidget = new QWidget(stackedWidget);
    QVBoxLayout *candProfileRoot = new QVBoxLayout(candidateProfilePageWidget);
    candProfileRoot->setContentsMargins(0, 0, 0, 0);
    candProfileRoot->setSpacing(0);
    QScrollArea *candScroll = new QScrollArea(candidateProfilePageWidget);
    candScroll->setWidgetResizable(true);
    candScroll->setFrameShape(QFrame::NoFrame);
    candScroll->setStyleSheet("background-color: transparent; border: none;");
    QWidget *candContent = new QWidget(candScroll);
    candContent->setStyleSheet("background-color: transparent;");
    candidateProfileContentLayout = new QVBoxLayout(candContent);
    candidateProfileContentLayout->setContentsMargins(28, 24, 28, 28);
    candidateProfileContentLayout->setSpacing(20);
    candScroll->setWidget(candContent);
    candProfileRoot->addWidget(candScroll);

    // Dedicated Company Profile Page (Full view, Index 4)
    companyProfilePageWidget = new QWidget(stackedWidget);
    QVBoxLayout *compProfileRoot = new QVBoxLayout(companyProfilePageWidget);
    compProfileRoot->setContentsMargins(0, 0, 0, 0);
    compProfileRoot->setSpacing(0);
    QScrollArea *compScroll = new QScrollArea(companyProfilePageWidget);
    compScroll->setWidgetResizable(true);
    compScroll->setFrameShape(QFrame::NoFrame);
    compScroll->setStyleSheet("background-color: transparent; border: none;");
    QWidget *compContent = new QWidget(compScroll);
    compContent->setStyleSheet("background-color: transparent;");
    companyProfileContentLayout = new QVBoxLayout(compContent);
    companyProfileContentLayout->setContentsMargins(28, 24, 28, 28);
    companyProfileContentLayout->setSpacing(20);
    compScroll->setWidget(compContent);
    compProfileRoot->addWidget(compScroll);

    // Dedicated Candidate Applications Page (Full view, Index 5)
    candidateAppsPageWidget = new QWidget(stackedWidget);
    QVBoxLayout *candAppsRoot = new QVBoxLayout(candidateAppsPageWidget);
    candAppsRoot->setContentsMargins(0, 0, 0, 0);
    candAppsRoot->setSpacing(0);
    QScrollArea *candAppsScroll = new QScrollArea(candidateAppsPageWidget);
    candAppsScroll->setWidgetResizable(true);
    candAppsScroll->setFrameShape(QFrame::NoFrame);
    candAppsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    candAppsScroll->setStyleSheet("background-color: transparent; border: none;");
    QWidget *candAppsContent = new QWidget(candAppsScroll);
    candAppsContent->setStyleSheet("background-color: transparent;");
    candidateAppsContentLayout = new QVBoxLayout(candAppsContent);
    candidateAppsContentLayout->setContentsMargins(28, 24, 28, 28);
    candidateAppsContentLayout->setSpacing(20);
    candAppsScroll->setWidget(candAppsContent);
    candAppsRoot->addWidget(candAppsScroll);

    // Dedicated Company Applications Page (Full view, Index 6)
    companyAppsPageWidget = new QWidget(stackedWidget);
    QVBoxLayout *compAppsRoot = new QVBoxLayout(companyAppsPageWidget);
    compAppsRoot->setContentsMargins(0, 0, 0, 0);
    compAppsRoot->setSpacing(0);
    QScrollArea *compAppsScroll = new QScrollArea(companyAppsPageWidget);
    compAppsScroll->setWidgetResizable(true);
    compAppsScroll->setFrameShape(QFrame::NoFrame);
    compAppsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    compAppsScroll->setStyleSheet("background-color: transparent; border: none;");
    QWidget *compAppsContent = new QWidget(compAppsScroll);
    compAppsContent->setStyleSheet("background-color: transparent;");
    companyAppsContentLayout = new QVBoxLayout(compAppsContent);
    companyAppsContentLayout->setContentsMargins(28, 24, 28, 28);
    companyAppsContentLayout->setSpacing(20);
    compAppsScroll->setWidget(compAppsContent);
    compAppsRoot->addWidget(compAppsScroll);

    stackedWidget->addWidget(candidatePage);               // Index 0
    stackedWidget->addWidget(companyPage);                 // Index 1
    stackedWidget->addWidget(jobPage);                     // Index 2
    stackedWidget->addWidget(candidateProfilePageWidget);  // Index 3
    stackedWidget->addWidget(companyProfilePageWidget);    // Index 4
    stackedWidget->addWidget(candidateAppsPageWidget);     // Index 5
    stackedWidget->addWidget(companyAppsPageWidget);       // Index 6

    rightLayout->addWidget(stackedWidget);
    rootLayout->addWidget(rightContainer, 1);

    // Event Connections
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(candidatePage, &CandidatePage::viewJobRequested, this, &MainWindow::onShowJobDetail);
    connect(candidatePage, &CandidatePage::viewAllApplicationsRequested, this, &MainWindow::showCandidateApplicationsPage);
    connect(companyPage, &CompanyPage::viewJobRequested, this, &MainWindow::onShowJobDetail);
    connect(companyPage, &CompanyPage::viewApplicantsRequested, this, &MainWindow::showCompanyApplicationsPage);
    connect(jobPage, &JobPage::backRequested, this, &MainWindow::onBackFromJobPage);
    connect(jobPage, &JobPage::applyRequested, this, &MainWindow::onApplyJobRequested);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if ((obj == avatarCircleLabel || obj == topUserNameLabel) && event->type() == QEvent::MouseButtonPress) {
        if (currentRole == "CANDIDATE") {
            setActiveNavBtn(btnProfileNav);
            showCandidateProfilePage();
        } else if (currentRole == "COMPANY") {
            setActiveNavBtn(btnProfileNav);
            showCompanyProfilePage();
        }
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setSession(int userId, const QString &email, const QString &role) {
    currentUserId = userId;
    currentEmail = email;
    currentRole = role;

    QString displayName;
    QString initials = "US";

    if (role == "COMPANY") {
        Company comp;
        if (m_data.getCompanyByUserID(userId, comp)) {
            displayName = QString::fromStdString(comp.getNameCompany());
        }
        if (displayName.trimmed().isEmpty()) displayName = "Công ty";
        initials = StyleHelper::getInitials(displayName, "CT");

        topUserNameLabel->setText(displayName + " ▾");
        topUserRoleLabel->setText("Nhà tuyển dụng");
        sidebarSubtitle->setText("Kết nối nhà tuyển dụng với nhân tài");
        searchEdit->setPlaceholderText("🔍  Tìm kiếm ứng viên, kỹ năng, vị trí...");
        searchEdit->setVisible(true);
        if (topDatePill) topDatePill->setVisible(false);

        QString logoPath = StyleHelper::getCompanyLogoPath(comp.getIDcompany());
        avatarCircleLabel->setFixedSize(36, 36);
        avatarCircleLabel->setAlignment(Qt::AlignCenter);
        avatarCircleLabel->setPixmap(StyleHelper::getCircularAvatar(logoPath, initials, 36, "#B95334", "#FFFFFF"));
        avatarCircleLabel->setStyleSheet("border: none; background: transparent;");

        companyPage->setSession(userId);
        stackedWidget->setCurrentIndex(1);
    } else {
        // CANDIDATE
        Candidate cand;
        if (m_data.getCandidateByUserID(userId, cand)) {
            displayName = QString::fromStdString(cand.getPersonalInfo().fullName);
        }
        if (displayName.trimmed().isEmpty()) displayName = "Ứng viên";
        initials = StyleHelper::getInitials(displayName, "UV");

        topUserNameLabel->setText(displayName + " ▾");
        topUserRoleLabel->setText("Ứng viên");
        sidebarSubtitle->setText("Kết nối tài năng với cơ hội");
        // Ẩn thanh tìm kiếm trên topBar vì trang ứng viên đã có thanh tìm kiếm & bộ lọc riêng
        searchEdit->setVisible(false);
        if (topDatePill) topDatePill->setVisible(true);

        QString avPath = StyleHelper::getCandidateAvatarPath(cand.getIDcandidate());
        avatarCircleLabel->setFixedSize(36, 36);
        avatarCircleLabel->setAlignment(Qt::AlignCenter);
        avatarCircleLabel->setPixmap(StyleHelper::getCircularAvatar(avPath, initials, 36, "#EFE4DA", "#8A4E36"));
        avatarCircleLabel->setStyleSheet("border: none; background: transparent;");

        candidatePage->setSession(userId);
        stackedWidget->setCurrentIndex(0);
    }

    updateNavigationForRole(role);
}

void MainWindow::setActiveNavBtn(QPushButton *activeBtn) {
    for (QPushButton *btn : navButtons) {
        if (btn == activeBtn) {
            btn->setProperty("class", "navButton active");
            btn->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700; border: none; border-radius: 8px; text-align: left; padding-left: 14px; font-size: 13px;");
        } else {
            btn->setProperty("class", "navButton");
            btn->setStyleSheet("background-color: transparent; color: #FFFFFF; font-weight: 500; border: none; border-radius: 8px; text-align: left; padding-left: 14px; font-size: 13px;");
        }
    }
}

void MainWindow::updateNavigationForRole(const QString &role) {
    // Xóa các nút cũ
    QLayoutItem *item;
    
    while ((item = navButtonsLayout->takeAt(0)) != nullptr) {
        QWidget *widget = item->widget();
        
        if (widget != nullptr) {
            widget->hide();
            widget->deleteLater();
        }
        
        delete item;
    }
    
    navButtons.clear();

    auto createNavBtn = [this](const QString &text, bool active = false) -> QPushButton* {
        QPushButton *btn = new QPushButton(text, this);
        btn->setProperty("class", active ? "navButton active" : "navButton");
        btn->setCursor(Qt::PointingHandCursor);
        if (active) {
            btn->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700; border: none; border-radius: 8px; text-align: left; padding-left: 14px; font-size: 13px;");
        } else {
            btn->setStyleSheet("background-color: transparent; color: #FFFFFF; font-weight: 500; border: none; border-radius: 8px; text-align: left; padding-left: 14px; font-size: 13px;");
        }
        navButtons.append(btn);
        navButtonsLayout->addWidget(btn);
        return btn;
    };

    if (role == "COMPANY") {
        btnHomeNav = createNavBtn("🏠  Trang chủ", true);
        btnProfileNav = createNavBtn("🏢  Hồ sơ công ty", false);
        btnAppsNav = createNavBtn("📄  Hồ sơ ứng tuyển", false);

        // Divider
        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("background-color: rgba(255, 255, 255, 0.2); margin: 6px 0;");
        navButtonsLayout->addWidget(line);

        QPushButton *btnNotif = createNavBtn("🔔  Thông báo (5)", false);
        QPushButton *btnSettings = createNavBtn("⚙️  Cài đặt", false);

        connect(btnHomeNav, &QPushButton::clicked, this, [this]() {
            setActiveNavBtn(btnHomeNav);
            stackedWidget->setCurrentIndex(1);
        });
        connect(btnProfileNav, &QPushButton::clicked, this, [this]() {
            setActiveNavBtn(btnProfileNav);
            showCompanyProfilePage();
        });
        connect(btnAppsNav, &QPushButton::clicked, this, [this]() {
            setActiveNavBtn(btnAppsNav);
            showCompanyApplicationsPage(0, "Tất cả vị trí tuyển dụng");
        });
        connect(btnNotif, &QPushButton::clicked, this, [this]() {
            showNotificationsDialog();
        });
        connect(btnSettings, &QPushButton::clicked, this, [this]() {
            showSettingsDialog();
        });
    } else {
        // CANDIDATE
        btnHomeNav = createNavBtn("🏠  Trang chủ", true);
        btnProfileNav = createNavBtn("👤  Hồ sơ của tôi", false);
        QPushButton *btnJobs = createNavBtn("💼  Việc làm phù hợp", false);
        btnAppsNav = createNavBtn("📄  Đơn ứng tuyển", false);

        // Divider
        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("background-color: rgba(255, 255, 255, 0.2); margin: 6px 0;");
        navButtonsLayout->addWidget(line);

        QPushButton *btnNotif = createNavBtn("🔔  Thông báo (3)", false);
        QPushButton *btnSettings = createNavBtn("⚙️  Cài đặt", false);

        connect(btnHomeNav, &QPushButton::clicked, this, [this]() {
            setActiveNavBtn(btnHomeNav);
            candidatePage->setViewMode(CandidatePage::ViewMode::AllJobs);
            stackedWidget->setCurrentIndex(0);
        });
        connect(btnProfileNav, &QPushButton::clicked, this, [this]() {
            setActiveNavBtn(btnProfileNav);
            showCandidateProfilePage();
        });
        connect(btnJobs, &QPushButton::clicked, this, [this, btnJobs]() {
            setActiveNavBtn(btnJobs);
            candidatePage->setViewMode(CandidatePage::ViewMode::MatchedJobs);
            stackedWidget->setCurrentIndex(0);
        });
        connect(btnAppsNav, &QPushButton::clicked, this, [this]() {
            setActiveNavBtn(btnAppsNav);
            showCandidateApplicationsPage();
        });
        connect(btnNotif, &QPushButton::clicked, this, [this]() {
            showNotificationsDialog();
        });
        connect(btnSettings, &QPushButton::clicked, this, [this]() {
            showSettingsDialog();
        });
    }
}

void MainWindow::onSearchTextChanged(const QString &text) {
    if (stackedWidget->currentIndex() == 0) {
        candidatePage->filterJobs(text);
    } else if (stackedWidget->currentIndex() == 1) {
        companyPage->filterContent(text);
    }
}

void MainWindow::onShowJobDetail(int jobId) {
    Job job;
    if (!m_data.getJobByID(jobId, job)) {
        CustomMessageBox::warning(this, "Không tìm thấy công việc", "Không thể tải thông tin công việc từ cơ sở dữ liệu.");
        return;
    }

    Company company;
    if (!m_data.getCompanyByID(job.getIDcompany(), company)) {
        company.setNameCompany("Doanh nghiệp");
    }

    previousPageIndex = stackedWidget->currentIndex();

    // Tính điểm phù hợp nếu phiên hiện tại là Candidate
    double matchScore = -1.0;
    Candidate cand;
    bool hasValidCandidate = false;
    if (currentRole == "CANDIDATE") {
        if (m_data.getCandidateByUserID(currentUserId, cand) && cand.getIDcandidate() > 0) {
            m_data.loadCandidateSkill(cand);
            m_data.loadCandidateCertificate(cand);
            m_data.loadCandidateExperience(cand);
            matchScore = calculatorTotal(cand, job);
            hasValidCandidate = true;
        }
    }

    // BƯỚC 1: Gọi loadJob TRƯỚC để nạp thông tin công việc và điểm matching vào giao diện gốc
    jobPage->loadJob(job, company, matchScore);

    // BƯỚC 2: Thiết lập trạng thái nút nộp đơn SAU ĐÓ để không bị loadJob ghi đè
    if (!job.getStatus() || job.isExpired()) {
        jobPage->setApplyState(true, "Đã đóng tuyển dụng");
    } else if (currentRole != "CANDIDATE") {
        jobPage->setApplyState(true, "Chỉ dành cho ứng viên");
    } else {
        if (!hasValidCandidate) {
            // Không tìm thấy hồ sơ Candidate hợp lệ: vô hiệu hóa nút và hiển thị trạng thái phù hợp
            jobPage->setApplyState(true, "Chưa hoàn thiện hồ sơ");
        } else {
            int candId = cand.getIDcandidate();
            Application existingApp;
            if (m_searchEngine.hasCandidateApplied(candId, jobId, existingApp)) {
                QString appliedStatusText = "Đang xem xét";
                std::string st = existingApp.getStatus();
                if (st == "ACCEPTED") appliedStatusText = "Đã chấp nhận";
                else if (st == "REJECTED") appliedStatusText = "Đã từ chối";
                jobPage->setApplyState(true, appliedStatusText);
            } else {
                jobPage->setApplyState(false);
            }
        }
    }

    stackedWidget->setCurrentIndex(2);
}

void MainWindow::onApplyJobRequested(int jobId) {
    if (currentRole != "CANDIDATE") {
        CustomMessageBox::warning(this, "Thông báo", "Chức năng nộp đơn ứng tuyển chỉ dành cho tài khoản Ứng viên.");
        return;
    }

    ApplyResult result = m_searchEngine.applyJob(currentUserId, jobId, m_data);

    switch (result.status) {
    case ApplyStatus::SUCCESS: {
        CustomMessageBox::success(
            this,
            "Ứng tuyển thành công",
            QString("Hồ sơ của bạn đã được ghi nhận và gửi thành công đến nhà tuyển dụng!\n\n"
                    "• Mã đơn ứng tuyển: #%1\n"
                    "• Vị trí: %2\n"
                    "• Điểm phù hợp tính toán: %3%\n"
                    "• Trạng thái: Đang xem xét\n"
                    "• Ngày nộp: %4")
            .arg(result.application.getIDappli())
            .arg(QString::fromStdString(result.job.getTitle()))
            .arg(QString::number(result.matchScore, 'f', 1))
            .arg(QString::fromStdString(result.application.getAppliedAt()))
        );

        jobPage->setApplyState(true, "Đang xem xét");
        candidatePage->setSession(currentUserId);
        break;
    }
    case ApplyStatus::CANDIDATE_NOT_FOUND:
        CustomMessageBox::critical(
            this,
            "Lỗi hồ sơ ứng viên",
            "Không tìm thấy hồ sơ ứng viên hợp lệ liên kết với tài khoản này.\n"
            "Vui lòng tạo hoặc cập nhật hồ sơ trước khi nộp đơn ứng tuyển."
        );
        break;
    case ApplyStatus::JOB_NOT_FOUND:
        CustomMessageBox::warning(this, "Lỗi dữ liệu", "Không thể xác định thông tin công việc để nộp đơn.");
        break;
    case ApplyStatus::JOB_CLOSED_OR_EXPIRED:
        CustomMessageBox::warning(this, "Thông báo", "Tin tuyển dụng này đã đóng hoặc đã hết hạn nộp hồ sơ.");
        jobPage->setApplyState(true, "Đã đóng tuyển dụng");
        break;
    case ApplyStatus::ALREADY_APPLIED: {
        QString vnStatus = "Đang xem xét";
        std::string st = result.application.getStatus();
        if (st == "ACCEPTED") vnStatus = "Đã chấp nhận";
        else if (st == "REJECTED") vnStatus = "Đã từ chối";

        CustomMessageBox::information(
            this,
            "Thông báo",
            QString("Bạn đã nộp đơn ứng tuyển vào vị trí này trước đó rồi.\nTrạng thái hiện tại: %1").arg(vnStatus)
        );
        jobPage->setApplyState(true, vnStatus);
        break;
    }
    case ApplyStatus::CALCULATION_ERROR:
        CustomMessageBox::warning(
            this,
            "Không thể tính điểm phù hợp",
            "Hệ thống chưa thể tính điểm phù hợp cho vị trí này. "
            "Đơn ứng tuyển chưa được tạo."
        );
        break;
    case ApplyStatus::NOT_ELIGIBLE:
        CustomMessageBox::warning(
            this,
            "Không đủ điều kiện ứng tuyển",
            "Hồ sơ của bạn không đáp ứng điều kiện tiên quyết của vị trí này "
            "(chuyên ngành không phù hợp hoặc thiếu chứng chỉ bắt buộc theo quy định)."
        );
        break;
    case ApplyStatus::SAVE_FAILED:
        CustomMessageBox::critical(this, "Lỗi lưu trữ", "Không thể lưu đơn ứng tuyển vào cơ sở dữ liệu applications.txt.");
        break;
    }
}

void MainWindow::onBackFromJobPage() {
    stackedWidget->setCurrentIndex(previousPageIndex);
    if (previousPageIndex == 3 || previousPageIndex == 4) {
        setActiveNavBtn(btnProfileNav);
    } else if (previousPageIndex == 5 || previousPageIndex == 6) {
        if (btnAppsNav) setActiveNavBtn(btnAppsNav);
    } else {
        setActiveNavBtn(btnHomeNav);
    }
}

void MainWindow::showCandidateProfilePage() {
    buildCandidateProfilePage();
    stackedWidget->setCurrentIndex(3);
}

void MainWindow::buildCandidateProfilePage() {
    safeClearLayout(candidateProfileContentLayout);

    Candidate cand;
    if (!m_data.getCandidateByUserID(currentUserId, cand) || cand.getIDcandidate() <= 0) {
        cand = Candidate();
        cand.setIDCandidate(m_data.getNextID("candidates.txt"));
        cand.setIDuser(currentUserId);
        cand.setRole("CANDIDATE");
    } else {
        m_data.loadCandidateSkill(cand);
        m_data.loadCandidateCertificate(cand);
        m_data.loadCandidateExperience(cand);
    }

    const auto &p = cand.getPersonalInfo();
    const auto &prof = cand.getProfessionalInfo();
    QString fullName = QString::fromStdString(p.fullName.empty() ? "Ứng viên" : p.fullName);

    // 1. Header Page Title
    QHBoxLayout *headerPageRow = new QHBoxLayout();
    QVBoxLayout *headerPageText = new QVBoxLayout();
    headerPageText->setSpacing(2);
    QLabel *headerTitle = new QLabel("👤 Hồ sơ của tôi", candidateProfilePageWidget);
    headerTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #1E1E1E;");
    QLabel *headerSubtitle = new QLabel("Quản lý thông tin cá nhân, học vấn, kỹ năng chuyên môn, chứng chỉ và theo dõi đơn ứng tuyển", candidateProfilePageWidget);
    headerSubtitle->setStyleSheet("font-size: 13px; color: #6C757D;");
    headerPageText->addWidget(headerTitle);
    headerPageText->addWidget(headerSubtitle);
    headerPageRow->addLayout(headerPageText);
    headerPageRow->addStretch();
    candidateProfileContentLayout->addLayout(headerPageRow);

    // 2. Card 1: Thông tin cá nhân & Học vấn
    QFrame *infoCard = new QFrame(candidateProfilePageWidget);
    infoCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 22px;");
    QHBoxLayout *infoCardLayout = new QHBoxLayout(infoCard);
    infoCardLayout->setSpacing(20);

    // Avatar Column
    QVBoxLayout *avCol = new QVBoxLayout();
    avCol->setSpacing(6);
    avCol->setAlignment(Qt::AlignCenter);

    QString initials = "UV";
    QStringList parts = fullName.split(' ', Qt::SkipEmptyParts);
    if (parts.size() >= 2) initials = parts.first().left(1).toUpper() + parts.last().left(1).toUpper();
    else if (!parts.isEmpty()) initials = parts.first().left(2).toUpper();

    QString avPath = StyleHelper::getCandidateAvatarPath(cand.getIDcandidate());
    QPushButton *avBtn = new QPushButton(infoCard);
    avBtn->setFixedSize(76, 76);
    avBtn->setCursor(Qt::PointingHandCursor);
    avBtn->setToolTip("Nhấn để đổi ảnh đại diện");
    avBtn->setStyleSheet("QPushButton { border: none; background: transparent; padding: 0; } QPushButton:hover { opacity: 0.85; }");
    avBtn->setIcon(QIcon(StyleHelper::getCircularAvatar(avPath, initials, 76)));
    avBtn->setIconSize(QSize(76, 76));

    QPushButton *changeAvBtn = new QPushButton("📷 Đổi ảnh", infoCard);
    changeAvBtn->setCursor(Qt::PointingHandCursor);
    changeAvBtn->setStyleSheet("background-color: transparent; color: #B95334; font-size: 11px; font-weight: 600; border: none; padding: 2px;");

    auto doChangeCandidateAvatar = [this, cand]() {
        QString file = QFileDialog::getOpenFileName(this, "Chọn ảnh đại diện", "", "Hình ảnh (*.png *.jpg *.jpeg *.bmp *.webp)");
        if (file.isEmpty()) return;

        QString savedPath;
        if (StyleHelper::saveCandidateAvatar(cand.getIDcandidate(), file, savedPath)) {
            buildCandidateProfilePage();
            // Refresh topbar and candidate dashboard
            QString initials = "UV";
            QString name = QString::fromStdString(cand.getPersonalInfo().fullName);
            QStringList p = name.split(' ', Qt::SkipEmptyParts);
            if (p.size() >= 2) initials = p.first().left(1).toUpper() + p.last().left(1).toUpper();
            avatarCircleLabel->setPixmap(StyleHelper::getCircularAvatar(savedPath, initials, 36));
            candidatePage->onRefreshData();
            CustomMessageBox::success(this, "Cập nhật thành công", "Đã cập nhật ảnh đại diện mới thành công!");
        } else {
            CustomMessageBox::warning(this, "Lỗi cập nhật", "Không thể nạp tệp hình ảnh đã chọn. Vui lòng chọn tệp định dạng JPG hoặc PNG hợp lệ.");
        }
    };

    connect(avBtn, &QPushButton::clicked, this, doChangeCandidateAvatar);
    connect(changeAvBtn, &QPushButton::clicked, this, doChangeCandidateAvatar);

    avCol->addWidget(avBtn, 0, Qt::AlignCenter);
    avCol->addWidget(changeAvBtn, 0, Qt::AlignCenter);
    infoCardLayout->setAlignment(Qt::AlignVCenter);
    infoCardLayout->addLayout(avCol);

    // Detail Fields Column
    QVBoxLayout *fieldsCol = new QVBoxLayout();
    fieldsCol->setSpacing(6);
    fieldsCol->setAlignment(Qt::AlignVCenter);

    QLabel *nameLabel = new QLabel(fullName, infoCard);
    nameLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #1E1E1E;");

    QLabel *majorLabel = new QLabel("🎓 " + QString::fromStdString(prof.major.empty() ? "Chưa cập nhật chuyên ngành" : prof.major), infoCard);
    majorLabel->setStyleSheet("font-size: 14px; font-weight: 700; color: #B95334;");

    QGridLayout *metaGrid = new QGridLayout();
    metaGrid->setHorizontalSpacing(24);
    metaGrid->setVerticalSpacing(8);

    QLabel *schoolLbl = new QLabel("🏫 " + QString::fromStdString(prof.school.empty() ? "Chưa cập nhật trường" : prof.school), infoCard);
    schoolLbl->setStyleSheet("font-size: 12px; color: #555555;");

    QLabel *eduLbl = new QLabel("📜 Hệ: " + QString::fromStdString(prof.educationLevel.empty() ? "Đại học" : prof.educationLevel), infoCard);
    eduLbl->setStyleSheet("font-size: 12px; color: #555555;");

    QLabel *gpaLbl = new QLabel(QString("⭐ GPA: %1/4.0").arg(QString::number(prof.GPA, 'f', 2)), infoCard);
    gpaLbl->setStyleSheet("background-color: #FEF3E2; color: #B76E00; font-weight: 700; font-size: 11px; padding: 2px 8px; border-radius: 4px;");

    QHBoxLayout *eduGpaLayout = new QHBoxLayout();
    eduGpaLayout->setSpacing(8);
    eduGpaLayout->setContentsMargins(0, 0, 0, 0);
    eduGpaLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    eduGpaLayout->addWidget(eduLbl);
    eduGpaLayout->addWidget(gpaLbl);
    eduGpaLayout->addStretch();

    QString birthStr = QString::fromStdString(p.birthday.empty() ? "N/A" : p.birthday);
    QString genderStr = QString::fromStdString(p.gender.empty() ? "Khác" : p.gender);
    QLabel *birthLbl = new QLabel(QString("🎂 %1 (%2)").arg(birthStr, genderStr), infoCard);
    birthLbl->setStyleSheet("font-size: 12px; color: #6C757D;");

    QLabel *emailLbl = new QLabel("📧 " + QString::fromStdString(cand.getEmail()), infoCard);
    emailLbl->setStyleSheet("font-size: 12px; color: #6C757D;");

    QLabel *phoneLbl = new QLabel("📞 " + QString::fromStdString(p.phoneCandidate.empty() ? "Chưa có SĐT" : p.phoneCandidate), infoCard);
    phoneLbl->setStyleSheet("font-size: 12px; color: #6C757D;");

    QLabel *addrLbl = new QLabel("📍 " + QString::fromStdString(p.addressCandidate.empty() ? "Chưa cập nhật địa chỉ" : p.addressCandidate), infoCard);
    addrLbl->setStyleSheet("font-size: 12px; color: #6C757D;");

    // Hàng 0: Học vấn & Thông tin cá nhân
    metaGrid->addWidget(schoolLbl, 0, 0);
    metaGrid->addLayout(eduGpaLayout, 0, 1);
    metaGrid->addWidget(birthLbl, 0, 2);

    // Hàng 1: Liên hệ & Địa chỉ cư trú
    metaGrid->addWidget(emailLbl, 1, 0);
    metaGrid->addWidget(phoneLbl, 1, 1);
    metaGrid->addWidget(addrLbl, 1, 2);

    fieldsCol->addWidget(nameLabel);
    fieldsCol->addWidget(majorLabel);
    fieldsCol->addLayout(metaGrid);
    infoCardLayout->addLayout(fieldsCol, 1);

    // Action button
    QPushButton *editProfileBtn = new QPushButton("✏ Chỉnh sửa hồ sơ", infoCard);
    editProfileBtn->setCursor(Qt::PointingHandCursor);
    editProfileBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; font-size: 13px; border: none; border-radius: 8px; padding: 9px 18px;");
    connect(editProfileBtn, &QPushButton::clicked, this, [this]() {
        candidatePage->onEditProfileClicked();
        buildCandidateProfilePage();
    });
    infoCardLayout->addWidget(editProfileBtn, 0, Qt::AlignVCenter);

    candidateProfileContentLayout->addWidget(infoCard);

    // 3. Card 2: Kỹ năng chuyên môn
    QFrame *skillCard = new QFrame(candidateProfilePageWidget);
    skillCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 20px;");
    QVBoxLayout *skillLayout = new QVBoxLayout(skillCard);
    skillLayout->setSpacing(14);

    QHBoxLayout *skillHead = new QHBoxLayout();
    QLabel *skillTitle = new QLabel("🎯 Kỹ năng chuyên môn", skillCard);
    skillTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QPushButton *addSkillBtn = new QPushButton("+ Thêm kỹ năng", skillCard);
    addSkillBtn->setCursor(Qt::PointingHandCursor);
    addSkillBtn->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 600; border: 1px solid #F5D3C7; border-radius: 6px; padding: 5px 12px; font-size: 12px;");
    connect(addSkillBtn, &QPushButton::clicked, this, [this]() {
        candidatePage->onAddSkillClicked();
        buildCandidateProfilePage();
    });
    skillHead->addWidget(skillTitle);
    skillHead->addStretch();
    skillHead->addWidget(addSkillBtn);
    skillLayout->addLayout(skillHead);

    const auto &skills = cand.getSkill();
    if (skills.empty()) {
        QLabel *emptySkill = new QLabel("Chưa cập nhật kỹ năng chuyên môn.", skillCard);
        emptySkill->setStyleSheet("color: #6C757D; font-size: 12px; font-style: italic;");
        skillLayout->addWidget(emptySkill);
    } else {
        QHBoxLayout *tagsRow = new QHBoxLayout();
        tagsRow->setSpacing(10);
        for (size_t i = 0; i < skills.size(); ++i) {
            const auto &s = skills[i];
            QFrame *pill = new QFrame(skillCard);
            pill->setStyleSheet("background-color: #F8F4F0; border: 1px solid #ECE5DE; border-radius: 8px; padding: 6px 12px;");
            QHBoxLayout *pLay = new QHBoxLayout(pill);
            pLay->setContentsMargins(6, 4, 6, 4);
            pLay->setSpacing(8);

            QLabel *sName = new QLabel(QString::fromStdString(s.name), pill);
            sName->setStyleSheet("font-weight: 700; color: #1E1E1E; font-size: 13px;");

            QString lvlStr = (s.level == 1) ? "Cơ bản" : (s.level == 2) ? "Trung bình" : (s.level == 3) ? "Khá" : (s.level == 4) ? "Giỏi" : "Chuyên gia";
            QLabel *sLvl = new QLabel(lvlStr, pill);
            sLvl->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-size: 11px; font-weight: 600; padding: 2px 6px; border-radius: 4px;");

            QPushButton *delBtn = new QPushButton("✕", pill);
            delBtn->setCursor(Qt::PointingHandCursor);
            delBtn->setStyleSheet("color: #999999; font-weight: bold; font-size: 11px; border: none; background: transparent;");
            int idx = static_cast<int>(i);
            connect(delBtn, &QPushButton::clicked, this, [this, idx]() {
                candidatePage->onDeleteSkillClicked(idx);
                buildCandidateProfilePage();
            });

            pLay->addWidget(sName);
            pLay->addWidget(sLvl);
            pLay->addWidget(delBtn);
            tagsRow->addWidget(pill);
        }
        tagsRow->addStretch();
        skillLayout->addLayout(tagsRow);
    }
    candidateProfileContentLayout->addWidget(skillCard);

    // 4. Card 3: Chứng chỉ ngoại ngữ & Chuyên môn
    QFrame *certCard = new QFrame(candidateProfilePageWidget);
    certCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 20px;");
    QVBoxLayout *certLayout = new QVBoxLayout(certCard);
    certLayout->setSpacing(14);

    QHBoxLayout *certHead = new QHBoxLayout();
    QLabel *certTitle = new QLabel("📜 Chứng chỉ ngoại ngữ & Chuyên môn", certCard);
    certTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QPushButton *addCertBtn = new QPushButton("+ Thêm chứng chỉ", certCard);
    addCertBtn->setCursor(Qt::PointingHandCursor);
    addCertBtn->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 600; border: 1px solid #F5D3C7; border-radius: 6px; padding: 5px 12px; font-size: 12px;");
    connect(addCertBtn, &QPushButton::clicked, this, [this]() {
        candidatePage->onAddCertificateClicked();
        buildCandidateProfilePage();
    });
    certHead->addWidget(certTitle);
    certHead->addStretch();
    certHead->addWidget(addCertBtn);
    certLayout->addLayout(certHead);

    const auto &certs = cand.getCertificate();
    if (certs.empty()) {
        QLabel *emptyCert = new QLabel("Chưa cập nhật chứng chỉ ngoại ngữ hoặc chuyên môn.", certCard);
        emptyCert->setStyleSheet("color: #6C757D; font-size: 12px; font-style: italic;");
        certLayout->addWidget(emptyCert);
    } else {
        for (size_t i = 0; i < certs.size(); ++i) {
            const auto &c = certs[i];
            QString typeName = (c.certificateTypeID == 1) ? "IELTS" : (c.certificateTypeID == 2) ? "TOEIC" : (c.certificateTypeID == 3) ? "TOEFL / HSK" : (c.certificateTypeID == 4) ? "JLPT" : "Chứng chỉ chuyên môn";
            QString scoreText = (c.certificateTypeID == 1) ? QString::number(c.value, 'f', 1) : QString::number(c.value, 'f', 0);

            QFrame *item = new QFrame(certCard);
            item->setStyleSheet("background-color: #F8F4F0; border: 1px solid #ECE5DE; border-radius: 8px; padding: 10px 14px;");
            QHBoxLayout *iLay = new QHBoxLayout(item);
            iLay->setContentsMargins(8, 6, 8, 6);
            iLay->setSpacing(12);

            QLabel *tLbl = new QLabel(typeName, item);
            tLbl->setStyleSheet("font-weight: 700; font-size: 13px; color: #1E1E1E;");
            QLabel *sBadge = new QLabel(scoreText, item);
            sBadge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-weight: 700; font-size: 12px; padding: 2px 8px; border-radius: 4px;");
            QLabel *orgLbl = new QLabel("Cấp bởi: " + QString::fromStdString(c.organization.empty() ? "Quốc tế" : c.organization), item);
            orgLbl->setStyleSheet("color: #6C757D; font-size: 12px;");

            QPushButton *delCertBtn = new QPushButton("🗑 Xóa", item);
            delCertBtn->setCursor(Qt::PointingHandCursor);
            delCertBtn->setStyleSheet("color: #DC2626; border: none; background: transparent; font-size: 12px; font-weight: 600;");
            int idx = static_cast<int>(i);
            connect(delCertBtn, &QPushButton::clicked, this, [this, idx]() {
                candidatePage->onDeleteCertificateClicked(idx);
                buildCandidateProfilePage();
            });

            iLay->addWidget(tLbl);
            iLay->addWidget(sBadge);
            iLay->addWidget(orgLbl);
            iLay->addStretch();
            iLay->addWidget(delCertBtn);
            certLayout->addWidget(item);
        }
    }
    candidateProfileContentLayout->addWidget(certCard);

    // 5. Card 4: Kinh nghiệm làm việc
    QFrame *expCard = new QFrame(candidateProfilePageWidget);
    expCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 20px;");
    QVBoxLayout *expLayout = new QVBoxLayout(expCard);
    expLayout->setSpacing(14);

    QHBoxLayout *expHead = new QHBoxLayout();
    QLabel *expTitle = new QLabel("💼 Kinh nghiệm làm việc", expCard);
    expTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QPushButton *addExpBtn = new QPushButton("+ Thêm kinh nghiệm", expCard);
    addExpBtn->setCursor(Qt::PointingHandCursor);
    addExpBtn->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 600; border: 1px solid #F5D3C7; border-radius: 6px; padding: 5px 12px; font-size: 12px;");
    connect(addExpBtn, &QPushButton::clicked, this, [this]() {
        candidatePage->onAddExperienceClicked();
        buildCandidateProfilePage();
    });
    expHead->addWidget(expTitle);
    expHead->addStretch();
    expHead->addWidget(addExpBtn);
    expLayout->addLayout(expHead);

    const auto &exps = cand.getExperiences();
    if (exps.empty()) {
        QLabel *emptyExp = new QLabel("Chưa cập nhật kinh nghiệm làm việc.", expCard);
        emptyExp->setStyleSheet("color: #6C757D; font-size: 12px; font-style: italic;");
        expLayout->addWidget(emptyExp);
    } else {
        for (size_t i = 0; i < exps.size(); ++i) {
            const auto &e = exps[i];
            QFrame *item = new QFrame(expCard);
            item->setStyleSheet("background-color: #F8F4F0; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px 16px;");
            QVBoxLayout *iLay = new QVBoxLayout(item);
            iLay->setSpacing(6);

            QHBoxLayout *topRow = new QHBoxLayout();
            QLabel *posLbl = new QLabel(QString::fromStdString(e.getPosition()) + "  •  " + QString::fromStdString(e.getCompanyName()), item);
            posLbl->setStyleSheet("font-size: 14px; font-weight: 700; color: #1E1E1E;");
            QLabel *dateLbl = new QLabel(QString("📅 %1 ➔ %2").arg(QString::fromStdString(e.getStartDate())).arg(QString::fromStdString(e.getEndDate())), item);
            dateLbl->setStyleSheet("font-size: 12px; color: #B95334; font-weight: 600;");

            QPushButton *delExpBtn = new QPushButton("🗑 Xóa", item);
            delExpBtn->setCursor(Qt::PointingHandCursor);
            delExpBtn->setStyleSheet("color: #DC2626; border: none; background: transparent; font-size: 12px; font-weight: 600;");
            int idx = static_cast<int>(i);
            connect(delExpBtn, &QPushButton::clicked, this, [this, idx]() {
                candidatePage->onDeleteExperienceClicked(idx);
                buildCandidateProfilePage();
            });

            topRow->addWidget(posLbl);
            topRow->addStretch();
            topRow->addWidget(dateLbl);
            topRow->addWidget(delExpBtn);
            iLay->addLayout(topRow);

            if (!e.getDescription().empty()) {
                QLabel *descLbl = new QLabel(QString::fromStdString(e.getDescription()), item);
                descLbl->setStyleSheet("font-size: 12px; color: #555555;");
                descLbl->setWordWrap(true);
                iLay->addWidget(descLbl);
            }
            expLayout->addWidget(item);
        }
    }
    candidateProfileContentLayout->addWidget(expCard);

    // 6. Card 5: Lịch sử nộp đơn ứng tuyển
    QFrame *appCard = new QFrame(candidateProfilePageWidget);
    appCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 20px;");
    QVBoxLayout *appLayout = new QVBoxLayout(appCard);
    appLayout->setSpacing(12);

    std::vector<Application> apps = m_searchEngine.getApplicationsForCandidate(cand.getIDcandidate());
    QLabel *appTitle = new QLabel(QString("📄 Lịch sử nộp đơn ứng tuyển (%1)").arg(apps.size()), appCard);
    appTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    appLayout->addWidget(appTitle);

    if (apps.empty()) {
        QLabel *emptyApp = new QLabel("Bạn chưa nộp đơn ứng tuyển vào vị trí nào.", appCard);
        emptyApp->setStyleSheet("color: #6C757D; font-size: 12px; font-style: italic;");
        appLayout->addWidget(emptyApp);
    } else {
        for (const auto &a : apps) {
            Job j;
            Company c;
            m_data.getJobByID(a.getIDjob(), j);
            m_data.getCompanyByID(j.getIDcompany(), c);

            QFrame *item = new QFrame(appCard);
            item->setStyleSheet("background-color: #F8F4F0; border: 1px solid #ECE5DE; border-radius: 8px; padding: 10px 14px;");
            QHBoxLayout *iLay = new QHBoxLayout(item);
            iLay->setSpacing(12);
            iLay->setAlignment(Qt::AlignVCenter);

            QString qCompName = QString::fromStdString(c.getNameCompany());
            QString initials = StyleHelper::getInitials(qCompName, "DN");
            QLabel *logo = new QLabel(item);
            logo->setAlignment(Qt::AlignCenter);
            logo->setFixedSize(44, 44);
            QString logoPath = StyleHelper::getCompanyLogoPath(c.getIDcompany());
            logo->setPixmap(StyleHelper::getRoundedBadge(logoPath, initials, 44, 10));

            QLabel *jTitle = new QLabel(QString::fromStdString(j.getTitle()), item);
            jTitle->setStyleSheet("font-size: 13px; font-weight: 700; color: #1E1E1E;");
            QLabel *cName = new QLabel(qCompName, item);
            cName->setStyleSheet("font-size: 12px; color: #6C757D;");
            float aScore = a.getAppropriateLevel();
            bool isHighScore = (aScore >= 70.0f);
            QLabel *scoreLbl = new QLabel(QString("🎯 %1%").arg(QString::number(aScore, 'f', 1)), item);
            scoreLbl->setStyleSheet(isHighScore
                ? "background-color: #E3F7EB; color: #1B7F49; font-size: 11px; font-weight: 700; padding: 2px 6px; border-radius: 4px;"
                : "background-color: #FEF3E2; color: #B76E00; font-size: 11px; font-weight: 700; padding: 2px 6px; border-radius: 4px;");
            QLabel *dText = new QLabel("📅 " + QString::fromStdString(a.getAppliedAt()), item);
            dText->setStyleSheet("font-size: 11px; color: #6C757D;");

            QLabel *stLbl = new QLabel(item);
            if (a.getStatus() == "ACCEPTED") {
                stLbl->setText("✓ Đã chấp nhận");
                stLbl->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-weight: 700; font-size: 11px; padding: 3px 8px; border-radius: 4px;");
            } else if (a.getStatus() == "REJECTED") {
                stLbl->setText("✕ Đã từ chối");
                stLbl->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-weight: 700; font-size: 11px; padding: 3px 8px; border-radius: 4px;");
            } else {
                stLbl->setText("⏳ Đang chờ duyệt");
                stLbl->setStyleSheet("background-color: #FEF3E2; color: #B76E00; font-weight: 700; font-size: 11px; padding: 3px 8px; border-radius: 4px;");
            }

            iLay->addWidget(logo, 0, Qt::AlignVCenter);
            iLay->addWidget(jTitle);
            iLay->addWidget(cName);
            iLay->addStretch();
            iLay->addWidget(scoreLbl);
            iLay->addWidget(dText);
            iLay->addWidget(stLbl);
            appLayout->addWidget(item);
        }
    }
    candidateProfileContentLayout->addWidget(appCard);
}

void MainWindow::showCompanyProfilePage() {
    buildCompanyProfilePage();
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::buildCompanyProfilePage() {
    safeClearLayout(companyProfileContentLayout);

    Company comp;
    std::vector<Job> jobs;
    std::vector<Application> apps;
    if (!m_data.getCompanyByUserID(currentUserId, comp) || comp.getIDcompany() <= 0) {
        comp = Company();
        comp.setIDcompany(m_data.getNextID("companies.txt"));
        comp.setIDuser(currentUserId);
        comp.setRole("COMPANY");
    } else {
        m_data.getJobsByCompanyID(comp.getIDcompany(), jobs);
        m_data.getApplicationsByCompanyID(comp.getIDcompany(), apps);
    }

    // 1. Header Title
    QHBoxLayout *headerPageRow = new QHBoxLayout();
    QVBoxLayout *headerPageText = new QVBoxLayout();
    headerPageText->setSpacing(2);
    QLabel *headerTitle = new QLabel("🏢 Hồ sơ doanh nghiệp", companyProfilePageWidget);
    headerTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #1E1E1E;");
    QLabel *headerSubtitle = new QLabel("Quản lý thông tin thương hiệu, hồ sơ công ty và hiệu quả chiến dịch tuyển dụng", companyProfilePageWidget);
    headerSubtitle->setStyleSheet("font-size: 13px; color: #6C757D;");
    headerPageText->addWidget(headerTitle);
    headerPageText->addWidget(headerSubtitle);
    headerPageRow->addLayout(headerPageText);
    headerPageRow->addStretch();
    companyProfileContentLayout->addLayout(headerPageRow);

    // 2. Card 1: Thông tin doanh nghiệp
    QFrame *infoCard = new QFrame(companyProfilePageWidget);
    infoCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 22px;");
    QHBoxLayout *infoCardLayout = new QHBoxLayout(infoCard);
    infoCardLayout->setSpacing(20);

    // Logo Column
    QVBoxLayout *logoCol = new QVBoxLayout();
    logoCol->setSpacing(6);
    logoCol->setAlignment(Qt::AlignCenter);

    QString compName = QString::fromStdString(comp.getNameCompany());
    if (compName.trimmed().isEmpty()) compName = "Công ty";
    QString initials = StyleHelper::getInitials(compName, "CT");

    QString logoPath = StyleHelper::getCompanyLogoPath(comp.getIDcompany());
    QPushButton *logoBtn = new QPushButton(infoCard);
    logoBtn->setFixedSize(76, 76);
    logoBtn->setCursor(Qt::PointingHandCursor);
    logoBtn->setToolTip("Nhấn để đổi logo công ty");
    logoBtn->setStyleSheet("QPushButton { border: none; background: transparent; padding: 0; } QPushButton:hover { opacity: 0.85; }");
    logoBtn->setIcon(QIcon(StyleHelper::getRoundedBadge(logoPath, initials, 76, 14)));
    logoBtn->setIconSize(QSize(76, 76));

    QPushButton *changeLogoBtn = new QPushButton("📷 Đổi logo", infoCard);
    changeLogoBtn->setCursor(Qt::PointingHandCursor);
    changeLogoBtn->setStyleSheet("background-color: transparent; color: #B95334; font-size: 11px; font-weight: 600; border: none; padding: 2px;");

    auto doChangeCompanyLogo = [this, comp]() {
        QString file = QFileDialog::getOpenFileName(this, "Chọn logo công ty", "", "Hình ảnh (*.png *.jpg *.jpeg *.bmp *.webp)");
        if (file.isEmpty()) return;

        QString savedPath;
        if (StyleHelper::saveCompanyLogo(comp.getIDcompany(), file, savedPath)) {
            buildCompanyProfilePage();
            companyPage->onRefreshData();
            // Refresh topbar
            avatarCircleLabel->setPixmap(StyleHelper::getCircularAvatar(savedPath, StyleHelper::getInitials(QString::fromStdString(comp.getNameCompany()), "DN"), 36, "#B95334", "#FFFFFF"));
            CustomMessageBox::success(this, "Cập nhật thành công", "Đã cập nhật logo công ty mới thành công!");
        } else {
            CustomMessageBox::warning(this, "Lỗi cập nhật", "Không thể nạp tệp logo đã chọn. Vui lòng chọn tệp định dạng JPG hoặc PNG hợp lệ.");
        }
    };

    connect(logoBtn, &QPushButton::clicked, this, doChangeCompanyLogo);
    connect(changeLogoBtn, &QPushButton::clicked, this, doChangeCompanyLogo);

    infoCardLayout->setAlignment(Qt::AlignVCenter);
    logoCol->addWidget(logoBtn, 0, Qt::AlignCenter);
    logoCol->addWidget(changeLogoBtn, 0, Qt::AlignCenter);
    infoCardLayout->addLayout(logoCol);

    // Detail Fields Column
    QVBoxLayout *fieldsCol = new QVBoxLayout();
    fieldsCol->setSpacing(6);
    fieldsCol->setAlignment(Qt::AlignVCenter);

    QLabel *nameLabel = new QLabel(compName, infoCard);
    nameLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #1E1E1E;");

    QHBoxLayout *meta1 = new QHBoxLayout();
    meta1->setSpacing(20);
    QLabel *webLbl = new QLabel("🌐 " + QString::fromStdString(comp.getWebsite().empty() ? "Chưa có website" : comp.getWebsite()), infoCard);
    webLbl->setStyleSheet("font-size: 13px; color: #B95334; font-weight: 600;");
    QLabel *addrLbl = new QLabel("📍 " + QString::fromStdString(comp.getAddressCompany()), infoCard);
    addrLbl->setStyleSheet("font-size: 12px; color: #6C757D;");
    QLabel *phoneLbl = new QLabel("📞 " + QString::fromStdString(comp.getPhoneCompany()), infoCard);
    phoneLbl->setStyleSheet("font-size: 12px; color: #6C757D;");
    meta1->addWidget(webLbl);
    meta1->addWidget(addrLbl);
    meta1->addWidget(phoneLbl);
    meta1->addStretch();

    QString compDesc = QString::fromStdString(comp.getDescription());
    if (compDesc.isEmpty()) compDesc = "Doanh nghiệp công nghệ phát triển giải pháp phần mềm.";
    QLabel *descLbl = new QLabel(compDesc, infoCard);
    descLbl->setStyleSheet("font-size: 13px; color: #555555; line-height: 1.4;");
    descLbl->setWordWrap(true);

    fieldsCol->addWidget(nameLabel);
    fieldsCol->addLayout(meta1);
    fieldsCol->addWidget(descLbl);
    infoCardLayout->addLayout(fieldsCol, 1);

    QPushButton *editCompBtn = new QPushButton("✏ Chỉnh sửa thông tin", infoCard);
    editCompBtn->setCursor(Qt::PointingHandCursor);
    editCompBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; font-size: 13px; border: none; border-radius: 8px; padding: 9px 18px;");
    connect(editCompBtn, &QPushButton::clicked, this, [this]() {
        companyPage->onEditCompanyProfileClicked();
        buildCompanyProfilePage();
    });
    infoCardLayout->addWidget(editCompBtn, 0, Qt::AlignVCenter);

    companyProfileContentLayout->addWidget(infoCard);

    // 3. Stats Badges Row
    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(16);

    int pendingCount = 0;
    int acceptedCount = 0;
    for (const auto &a : apps) {
        if (a.getStatus() == "PENDING") pendingCount++;
        else if (a.getStatus() == "ACCEPTED") acceptedCount++;
    }

    auto makeStatCard = [this](const QString &num, const QString &label, const QString &color) -> QFrame* {
        QFrame *f = new QFrame(companyProfilePageWidget);
        f->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
        QVBoxLayout *l = new QVBoxLayout(f);
        l->setSpacing(4);
        QLabel *n = new QLabel(num, f);
        n->setStyleSheet(QString("font-size: 24px; font-weight: 800; color: %1;").arg(color));
        QLabel *lb = new QLabel(label, f);
        lb->setStyleSheet("font-size: 12px; color: #6C757D; font-weight: 500;");
        l->addWidget(n);
        l->addWidget(lb);
        return f;
    };

    statsRow->addWidget(makeStatCard(QString::number(jobs.size()), "Tin tuyển dụng đang mở", "#B95334"));
    statsRow->addWidget(makeStatCard(QString::number(apps.size()), "Hồ sơ ứng viên đã nhận", "#1E1E1E"));
    statsRow->addWidget(makeStatCard(QString::number(acceptedCount), "Ứng viên đã phê duyệt", "#1B7F49"));
    statsRow->addWidget(makeStatCard(QString::number(pendingCount), "Hồ sơ chờ xem xét", "#B76E00"));

    companyProfileContentLayout->addLayout(statsRow);

    // 4. Quản lý việc làm
    QFrame *jobsCard = new QFrame(companyProfilePageWidget);
    jobsCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 20px;");
    QVBoxLayout *jobsLayout = new QVBoxLayout(jobsCard);
    jobsLayout->setSpacing(14);

    QHBoxLayout *jHead = new QHBoxLayout();
    QLabel *jTitle = new QLabel(QString("💼 Danh sách việc làm đã đăng (%1)").arg(jobs.size()), jobsCard);
    jTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QPushButton *createJobBtn = new QPushButton("+ Đăng tin tuyển dụng mới", jobsCard);
    createJobBtn->setCursor(Qt::PointingHandCursor);
    createJobBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; font-size: 12px; border: none; border-radius: 6px; padding: 6px 14px;");
    connect(createJobBtn, &QPushButton::clicked, this, [this]() {
        companyPage->onCreateJobClicked();
        buildCompanyProfilePage();
    });
    jHead->addWidget(jTitle);
    jHead->addStretch();
    jHead->addWidget(createJobBtn);
    jobsLayout->addLayout(jHead);

    if (jobs.empty()) {
        QLabel *emptyJob = new QLabel("Chưa có tin tuyển dụng nào.", jobsCard);
        emptyJob->setStyleSheet("color: #6C757D; font-size: 12px; font-style: italic;");
        jobsLayout->addWidget(emptyJob);
    } else {
        for (const auto &j : jobs) {
            int appCount = m_searchEngine.countApplicantsForJob(j.getIDjob());

            QFrame *item = new QFrame(jobsCard);
            item->setStyleSheet("background-color: #F8F4F0; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px 16px;");
            QHBoxLayout *iLay = new QHBoxLayout(item);
            iLay->setSpacing(14);

            QVBoxLayout *tBox = new QVBoxLayout();
            tBox->setSpacing(2);
            QLabel *titleLbl = new QLabel(QString::fromStdString(j.getTitle()), item);
            titleLbl->setStyleSheet("font-size: 14px; font-weight: 700; color: #1E1E1E;");
            QLabel *meta = new QLabel(QString("📍 %1  |  💰 %2 VNĐ  |  ⏰ Hạn: %3").arg(QString::fromStdString(j.getLocation())).arg(QLocale(QLocale::Vietnamese).toString(j.getSalary())).arg(QString::fromStdString(j.getDeadline())), item);
            meta->setStyleSheet("font-size: 11px; color: #6C757D;");
            tBox->addWidget(titleLbl);
            tBox->addWidget(meta);

            QLabel *countBadge = new QLabel(QString("👥 %1 ứng viên").arg(appCount), item);
            countBadge->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700; font-size: 11px; padding: 3px 8px; border-radius: 4px;");

            QPushButton *viewBtn = new QPushButton("Xem hồ sơ", item);
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet("background-color: #FFFFFF; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 12px; font-size: 12px; font-weight: 600;");
            int jId = j.getIDjob();
            QString qTitle = QString::fromStdString(j.getTitle());
            connect(viewBtn, &QPushButton::clicked, this, [this, jId, qTitle]() {
                showCompanyApplicationsPage(jId, qTitle);
            });

            iLay->addLayout(tBox, 1);
            iLay->addWidget(countBadge);
            iLay->addWidget(viewBtn);
            jobsLayout->addWidget(item);
        }
    }
    companyProfileContentLayout->addWidget(jobsCard);
}

void MainWindow::onLogoutClicked() {
    if (CustomMessageBox::question(
        this,
        "Đăng xuất",
        "Bạn có chắc chắn muốn đăng xuất khỏi phiên làm việc hiện tại?",
        "Đăng xuất",
        "Hủy bỏ"
    )) {
        emit logoutRequested();
    }
}

void MainWindow::showCandidateProfileDialog() {
    Candidate cand;
    if (!m_data.getCandidateByUserID(currentUserId, cand) || cand.getIDcandidate() <= 0) {
        CustomMessageBox::warning(this, "Thông báo", "Chưa tìm thấy thông tin hồ sơ ứng viên.");
        return;
    }
    m_data.loadCandidateSkill(cand);
    m_data.loadCandidateCertificate(cand);
    m_data.loadCandidateExperience(cand);

    const auto &p = cand.getPersonalInfo();
    const auto &prof = cand.getProfessionalInfo();
    QString fullName = QString::fromStdString(p.fullName.empty() ? "Ứng viên" : p.fullName);

    QDialog dialog(this);
    dialog.setWindowTitle("Hồ sơ năng lực — " + fullName);
    dialog.resize(650, 560);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    // Header Card
    QFrame *hCard = new QFrame(&dialog);
    hCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QHBoxLayout *hLayout = new QHBoxLayout(hCard);
    hLayout->setSpacing(16);
    hLayout->setAlignment(Qt::AlignVCenter);

    QString initials = "UV";
    QStringList parts = fullName.split(' ', Qt::SkipEmptyParts);
    if (parts.size() >= 2) initials = parts.first().left(1).toUpper() + parts.last().left(1).toUpper();
    else if (!parts.isEmpty()) initials = parts.first().left(2).toUpper();

    QString avPath = StyleHelper::getCandidateAvatarPath(cand.getIDcandidate());
    QLabel *avatar = new QLabel(hCard);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(64, 64);
    avatar->setPixmap(StyleHelper::getCircularAvatar(avPath, initials, 64, "#B95334", "#FFFFFF"));

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(3);
    infoLayout->setAlignment(Qt::AlignVCenter);
    QLabel *nameLbl = new QLabel(fullName, hCard);
    nameLbl->setStyleSheet("font-size: 18px; font-weight: 800; color: #1E1E1E;");
    QLabel *majorLbl = new QLabel("Chuyên ngành: " + QString::fromStdString(prof.major.empty() ? "Chưa cập nhật" : prof.major), hCard);
    majorLbl->setStyleSheet("font-size: 13px; font-weight: 600; color: #B95334;");
    infoLayout->addWidget(nameLbl);
    infoLayout->addWidget(majorLbl);

    hLayout->addWidget(avatar, 0, Qt::AlignVCenter);
    hLayout->addLayout(infoLayout, 1);
    mainLayout->addWidget(hCard);

    // Scroll details
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *content = new QWidget(scrollArea);
    content->setStyleSheet("background-color: transparent;");
    QVBoxLayout *cLayout = new QVBoxLayout(content);
    cLayout->setContentsMargins(0, 0, 0, 0);
    cLayout->setSpacing(12);

    // Thẻ thông tin hợp nhất, loại bỏ các ô trắng phân mục rời rạc chiếm diện tích
    QFrame *detailsCard = new QFrame(content);
    detailsCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QVBoxLayout *dLayout = new QVBoxLayout(detailsCard);
    dLayout->setSpacing(12);

    auto makeSection = [detailsCard](QVBoxLayout *parentLayout, const QString &title, const QString &icon, bool addDivider) -> QVBoxLayout* {
        if (addDivider) {
            QFrame *line = new QFrame(detailsCard);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            line->setStyleSheet("background-color: #F0EAE4; max-height: 1px; border: none; margin: 4px 0px;");
            parentLayout->addWidget(line);
        }
        QVBoxLayout *secLay = new QVBoxLayout();
        secLay->setSpacing(6);
        QLabel *l = new QLabel(icon + " " + title, detailsCard);
        l->setStyleSheet("font-size: 13px; font-weight: 700; color: #1E1E1E;");
        secLay->addWidget(l);
        parentLayout->addLayout(secLay);
        return secLay;
    };

    // 1. Thông tin cá nhân
    QVBoxLayout *pLay = makeSection(dLayout, "Thông tin cá nhân & Liên hệ", "👤", false);
    QLabel *contactInfo = new QLabel(QString(
        "• Email: %1\n• Điện thoại: %2\n• Ngày sinh: %3 (%4)\n• Địa chỉ: %5"
    ).arg(currentEmail)
     .arg(QString::fromStdString(p.phoneCandidate))
     .arg(QString::fromStdString(p.birthday))
     .arg(QString::fromStdString(p.gender))
     .arg(QString::fromStdString(p.addressCandidate)), detailsCard);
    contactInfo->setStyleSheet("font-size: 12px; line-height: 1.5; color: #333333;");
    pLay->addWidget(contactInfo);

    // 2. Học vấn & GPA
    QVBoxLayout *eduLay = makeSection(dLayout, "Học vấn & Trình độ", "🎓", true);
    QLabel *eduInfo = new QLabel(QString(
        "• Trường đào tạo: %1\n• Chuyên ngành: %2\n• Trình độ: %3\n• Điểm GPA: %4 / 4.00"
    ).arg(QString::fromStdString(prof.school))
     .arg(QString::fromStdString(prof.major))
     .arg(QString::fromStdString(prof.educationLevel))
     .arg(QString::number(prof.GPA, 'f', 2)), detailsCard);
    eduInfo->setStyleSheet("font-size: 12px; line-height: 1.5; color: #333333;");
    eduLay->addWidget(eduInfo);

    // 3. Kỹ năng
    const auto &skills = cand.getSkill();
    if (!skills.empty()) {
        QVBoxLayout *skLay = makeSection(dLayout, "Kỹ năng chuyên môn", "⚡", true);
        QHBoxLayout *tagsLay = new QHBoxLayout();
        tagsLay->setSpacing(8);
        for (const auto &sk : skills) {
            QLabel *tag = new QLabel(QString::fromStdString(sk.name) + " (Lv " + QString::number(sk.level) + ")", detailsCard);
            tag->setStyleSheet("background-color: #FCE8E2; color: #B95334; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 8px;");
            tagsLay->addWidget(tag);
        }
        tagsLay->addStretch();
        skLay->addLayout(tagsLay);
    }

    // 4. Chứng chỉ chuyên ngành & Ngoại ngữ
    const auto &certs = cand.getCertificate();
    if (!certs.empty()) {
        QVBoxLayout *certLay = makeSection(dLayout, "Chứng chỉ chuyên ngành & Ngoại ngữ", "📜", true);
        for (const auto &c : certs) {
            QString typeName = "Chứng chỉ";
            if (c.certificateTypeID == 1) typeName = "IELTS";
            else if (c.certificateTypeID == 2) typeName = "TOEIC";
            else if (c.certificateTypeID == 3) typeName = "TOEFL / HSK";
            else if (c.certificateTypeID == 4) typeName = "JLPT (Tiếng Nhật)";
            else if (c.certificateTypeID == 5) typeName = "Cloud / AWS";
            else typeName = "Chuyên ngành";

            QString valStr;
            if (c.certificateTypeID == 1) { // IELTS dạng 7.0, 6.5
                valStr = QString::number(c.value, 'f', 1);
            } else if (c.certificateTypeID == 2) { // TOEIC dạng số nguyên 700, 650
                valStr = QString::number(static_cast<int>(std::round(c.value)));
            } else if (c.certificateTypeID == 4) { // JLPT dạng N3, N2...
                int n = static_cast<int>(std::round(c.value));
                valStr = (n >= 1 && n <= 5) ? QString("N%1").arg(n) : QString::number(n);
            } else {
                valStr = (c.value == static_cast<int>(c.value)) ? QString::number(static_cast<int>(c.value)) : QString::number(c.value, 'f', 1);
            }

            QString orgStr = c.organization.empty() ? "Tổ chức uy tín" : QString::fromStdString(c.organization);
            QString cText = QString("• %1: %2 (%3)").arg(typeName).arg(valStr).arg(orgStr);
            QLabel *cLbl = new QLabel(cText, detailsCard);
            cLbl->setStyleSheet("font-size: 12px; color: #333333;");
            certLay->addWidget(cLbl);
        }
    }

    // 5. Kinh nghiệm làm việc
    const auto &exps = cand.getExperiences();
    if (!exps.empty()) {
        QVBoxLayout *expLay = makeSection(dLayout, "Kinh nghiệm làm việc", "💼", true);
        for (const auto &e : exps) {
            QVBoxLayout *eBox = new QVBoxLayout();
            eBox->setSpacing(2);
            QLabel *eLbl = new QLabel(QString("• %1 @ %2 (%3 → %4)")
                .arg(QString::fromStdString(e.getPosition()))
                .arg(QString::fromStdString(e.getCompanyName()))
                .arg(QString::fromStdString(e.getStartDate()))
                .arg(QString::fromStdString(e.getEndDate())), detailsCard);
            eLbl->setStyleSheet("font-size: 12px; font-weight: 600; color: #1E1E1E;");
            eBox->addWidget(eLbl);
            if (!e.getDescription().empty()) {
                QLabel *descLbl = new QLabel(QString::fromStdString(e.getDescription()), detailsCard);
                descLbl->setStyleSheet("font-size: 11px; color: #555555; padding-left: 12px;");
                descLbl->setWordWrap(true);
                eBox->addWidget(descLbl);
            }
            expLay->addLayout(eBox);
        }
    }

    cLayout->addWidget(detailsCard);

    scrollArea->setWidget(content);
    mainLayout->addWidget(scrollArea, 1);

    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->setSpacing(10);
    botLay->addStretch();

    QPushButton *editBtn = new QPushButton("✏ Chỉnh sửa hồ sơ", &dialog);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setStyleSheet("background-color: transparent; color: #B95334; border: 1px solid #B95334; font-weight: 600; border-radius: 6px; padding: 7px 18px;");
    connect(editBtn, &QPushButton::clicked, [&]() {
        dialog.accept();
        if (candidatePage) {
            candidatePage->onEditProfileClicked();
        }
    });

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 24px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    botLay->addWidget(editBtn);
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showCandidateApplicationsDialog() {
    showCandidateApplicationsPage();
}

void MainWindow::showCompanyProfileDialog() {
    Company comp;
    if (!m_data.getCompanyByUserID(currentUserId, comp)) {
        CustomMessageBox::warning(this, "Thông báo", "Chưa tìm thấy thông tin công ty.");
        return;
    }

    std::vector<Job> jobs;
    m_data.getJobsByCompanyID(comp.getIDcompany(), jobs);
    std::vector<Application> apps;
    m_data.getApplicationsByCompanyID(comp.getIDcompany(), apps);

    QDialog dialog(this);
    dialog.setWindowTitle("Thông tin doanh nghiệp — " + QString::fromStdString(comp.getNameCompany()));
    dialog.resize(600, 480);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // Header Card
    QFrame *hCard = new QFrame(&dialog);
    hCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QHBoxLayout *hLayout = new QHBoxLayout(hCard);
    hLayout->setSpacing(16);
    hLayout->setAlignment(Qt::AlignVCenter);

    QString logoPath = StyleHelper::getCompanyLogoPath(comp.getIDcompany());
    QString compInitials = StyleHelper::getInitials(QString::fromStdString(comp.getNameCompany()), "DN");
    QLabel *logo = new QLabel(hCard);
    logo->setAlignment(Qt::AlignCenter);
    logo->setFixedSize(54, 54);
    logo->setPixmap(StyleHelper::getRoundedBadge(logoPath, compInitials, 54, 12));

    QVBoxLayout *tBox = new QVBoxLayout();
    tBox->setSpacing(3);
    tBox->setAlignment(Qt::AlignVCenter);
    QLabel *nameLbl = new QLabel(QString::fromStdString(comp.getNameCompany()), hCard);
    nameLbl->setStyleSheet("font-size: 18px; font-weight: 800; color: #1E1E1E;");
    QLabel *webLbl = new QLabel("🌐 " + QString::fromStdString(comp.getWebsite().empty() ? "https://abctech.vn" : comp.getWebsite()), hCard);
    webLbl->setStyleSheet("font-size: 12px; color: #B95334;");
    tBox->addWidget(nameLbl);
    tBox->addWidget(webLbl);

    hLayout->addWidget(logo, 0, Qt::AlignVCenter);
    hLayout->addLayout(tBox, 1);
    mainLayout->addWidget(hCard);

    // Stats
    QHBoxLayout *statsLay = new QHBoxLayout();
    statsLay->setSpacing(12);

    auto makeStatCard = [&dialog](const QString &num, const QString &text) -> QFrame* {
        QFrame *f = new QFrame(&dialog);
        f->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px;");
        QVBoxLayout *l = new QVBoxLayout(f);
        l->setSpacing(2);
        l->setAlignment(Qt::AlignCenter);
        QLabel *n = new QLabel(num, f);
        n->setAlignment(Qt::AlignCenter);
        n->setStyleSheet("font-size: 20px; font-weight: 800; color: #B95334;");
        QLabel *t = new QLabel(text, f);
        t->setAlignment(Qt::AlignCenter);
        t->setStyleSheet("font-size: 11px; color: #6C757D;");
        l->addWidget(n);
        l->addWidget(t);
        return f;
    };

    statsLay->addWidget(makeStatCard(QString::number(jobs.size()), "Tin tuyển dụng đang mở"));
    statsLay->addWidget(makeStatCard(QString::number(apps.size()), "Lượt ứng tuyển đã nhận"));
    mainLayout->addLayout(statsLay);

    // Info details Card
    QFrame *infoCard = new QFrame(&dialog);
    infoCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QVBoxLayout *infoLay = new QVBoxLayout(infoCard);
    infoLay->setSpacing(8);

    QLabel *detTitle = new QLabel("🏢 Chi tiết doanh nghiệp", infoCard);
    detTitle->setStyleSheet("font-size: 14px; font-weight: 700; color: #1E1E1E;");
    infoLay->addWidget(detTitle);

    QString compDesc = comp.getDescription().empty() ? "Doanh nghiệp công nghệ phát triển giải pháp phần mềm." : QString::fromStdString(comp.getDescription());
    QLabel *detContent = new QLabel(QString(
        "• Địa chỉ trụ sở: %1\n• Điện thoại liên hệ: %2\n• Email quản trị: %3\n• Giới thiệu: %4"
    ).arg(QString::fromStdString(comp.getAddressCompany()))
     .arg(QString::fromStdString(comp.getPhoneCompany()))
     .arg(currentEmail)
     .arg(compDesc), infoCard);
    detContent->setStyleSheet("font-size: 12px; line-height: 1.6; color: #333333;");
    detContent->setWordWrap(true);
    infoLay->addWidget(detContent);
    mainLayout->addWidget(infoCard, 1);

    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->setSpacing(10);
    botLay->addStretch();

    QPushButton *editCompBtn = new QPushButton("✏ Chỉnh sửa thông tin", &dialog);
    editCompBtn->setCursor(Qt::PointingHandCursor);
    editCompBtn->setStyleSheet("background-color: transparent; color: #B95334; border: 1px solid #B95334; font-weight: 600; border-radius: 6px; padding: 7px 18px;");
    connect(editCompBtn, &QPushButton::clicked, [&]() {
        dialog.accept();
        if (companyPage) {
            companyPage->onEditCompanyProfileClicked();
        }
    });

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 24px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    botLay->addWidget(editCompBtn);
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showNotificationsDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Thông báo hệ thống");
    dialog.resize(520, 380);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    QLabel *titleLbl = new QLabel("🔔 Trung tâm thông báo", &dialog);
    titleLbl->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
    mainLayout->addWidget(titleLbl);

    QFrame *nCard1 = new QFrame(&dialog);
    nCard1->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px;");
    QVBoxLayout *l1 = new QVBoxLayout(nCard1);
    QLabel *t1 = new QLabel("⚡ Chào mừng đến với JobMatch", nCard1);
    t1->setStyleSheet("font-weight: 700; color: #1E1E1E; font-size: 13px;");
    QLabel *d1 = new QLabel("Hệ thống kết nối tuyển dụng và tìm kiếm việc làm bằng C++ & Qt Widgets đã sẵn sàng.", nCard1);
    d1->setStyleSheet("color: #6C757D; font-size: 12px;");
    l1->addWidget(t1);
    l1->addWidget(d1);
    mainLayout->addWidget(nCard1);

    QFrame *nCard2 = new QFrame(&dialog);
    nCard2->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px;");
    QVBoxLayout *l2 = new QVBoxLayout(nCard2);
    QLabel *t2 = new QLabel("✅ Dữ liệu thời gian thực đồng bộ", nCard2);
    t2->setStyleSheet("font-weight: 700; color: #1B7F49; font-size: 13px;");
    QLabel *d2 = new QLabel(QString("Phiên làm việc hiện tại: %1 (%2)").arg(currentEmail).arg(currentRole == "COMPANY" ? "Doanh nghiệp" : "Ứng viên"), nCard2);
    d2->setStyleSheet("color: #6C757D; font-size: 12px;");
    l2->addWidget(t2);
    l2->addWidget(d2);
    mainLayout->addWidget(nCard2);

    mainLayout->addStretch();

    QPushButton *closeBtn = new QPushButton("Đã hiểu", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 20px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->addStretch();
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showSettingsDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Cài đặt & Thông tin hệ thống");
    dialog.resize(520, 420);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    QLabel *titleLbl = new QLabel("⚙️ Cài đặt & Thông tin PBL2", &dialog);
    titleLbl->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
    mainLayout->addWidget(titleLbl);

    QFrame *card = new QFrame(&dialog);
    card->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QVBoxLayout *l = new QVBoxLayout(card);
    l->setSpacing(8);

    QLabel *projTitle = new QLabel("JobMatch — Hệ thống Tuyển dụng & Kết nối Việc làm", card);
    projTitle->setStyleSheet("font-weight: 800; font-size: 14px; color: #B95334;");
    QLabel *info = new QLabel(
        "• Học phần: Đồ án Cơ sở Lập trình (PBL2)\n"
        "• Khoa: Công nghệ Thông tin — Trường ĐH Bách Khoa, ĐHĐN\n"
        "• Ngôn ngữ: C++17 (Kiến trúc chuẩn OOP & Modular)\n"
        "• Giao diện: Qt 6 Widgets (Terracotta Design System)\n"
        "• Phiên bản: 1.0.0 (Release Candidate)\n"
        "• Cơ sở dữ liệu: Hệ thống tệp tin phẳng (.txt Database)", card);
    info->setStyleSheet("font-size: 12px; line-height: 1.6; color: #333333;");
    l->addWidget(projTitle);
    l->addWidget(info);
    mainLayout->addWidget(card);

    mainLayout->addStretch();

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 24px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->addStretch();
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showCandidateApplicationsPage() {
    buildCandidateApplicationsPage();
    stackedWidget->setCurrentIndex(5);
    setActiveNavBtn(btnAppsNav);
}

void MainWindow::buildCandidateApplicationsPage() {
    safeClearLayout(candidateAppsContentLayout);

    Candidate cand;
    if (!m_data.getCandidateByUserID(currentUserId, cand) || cand.getIDcandidate() <= 0) {
        QFrame *errCard = new QFrame(candidateAppsPageWidget);
        errCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 32px;");
        QVBoxLayout *errLay = new QVBoxLayout(errCard);
        errLay->setSpacing(12);
        errLay->setAlignment(Qt::AlignCenter);

        QLabel *errTitle = new QLabel("Chưa tìm thấy thông tin hồ sơ ứng viên", errCard);
        errTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
        QLabel *errSub = new QLabel("Vui lòng cập nhật hồ sơ cá nhân trước khi theo dõi đơn ứng tuyển.", errCard);
        errSub->setStyleSheet("font-size: 13px; color: #6C757D;");
        QPushButton *goProfBtn = new QPushButton("Cập nhật hồ sơ ngay", errCard);
        goProfBtn->setCursor(Qt::PointingHandCursor);
        goProfBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; padding: 10px 20px; border-radius: 8px; border: none; font-size: 13px;");
        connect(goProfBtn, &QPushButton::clicked, this, [this]() {
            setActiveNavBtn(btnProfileNav);
            showCandidateProfilePage();
        });

        errLay->addWidget(errTitle, 0, Qt::AlignCenter);
        errLay->addWidget(errSub, 0, Qt::AlignCenter);
        errLay->addWidget(goProfBtn, 0, Qt::AlignCenter);
        candidateAppsContentLayout->addWidget(errCard);
        return;
    }

    std::vector<Application> apps = m_searchEngine.getApplicationsForCandidate(cand.getIDcandidate());

    // 1. Header Row
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(16);

    QPushButton *backBtn = new QPushButton("← Quay lại trang chủ", candidateAppsPageWidget);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 8px 16px; font-weight: 600; color: #1E1E1E; font-size: 13px;");
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        setActiveNavBtn(btnHomeNav);
        candidatePage->setViewMode(CandidatePage::ViewMode::AllJobs);
        stackedWidget->setCurrentIndex(0);
    });

    QVBoxLayout *headerText = new QVBoxLayout();
    headerText->setSpacing(2);
    QLabel *headerTitle = new QLabel("📄 Đơn ứng tuyển của tôi", candidateAppsPageWidget);
    headerTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #1E1E1E;");
    QLabel *headerSubtitle = new QLabel("Theo dõi tiến độ, trạng thái xét duyệt và phản hồi từ nhà tuyển dụng theo thời gian thực", candidateAppsPageWidget);
    headerSubtitle->setStyleSheet("font-size: 13px; color: #6C757D;");
    headerText->addWidget(headerTitle);
    headerText->addWidget(headerSubtitle);

    headerRow->addWidget(backBtn, 0, Qt::AlignVCenter);
    headerRow->addLayout(headerText, 1);
    headerRow->addStretch();
    candidateAppsContentLayout->addLayout(headerRow);

    // Tính thống kê
    int totalCount = static_cast<int>(apps.size());
    int pendingCount = 0;
    int acceptedCount = 0;
    int rejectedCount = 0;
    for (const auto &a : apps) {
        if (a.getStatus() == "ACCEPTED") acceptedCount++;
        else if (a.getStatus() == "REJECTED") rejectedCount++;
        else pendingCount++;
    }

    // 2. Metrics Bar Row
    QHBoxLayout *metricsRow = new QHBoxLayout();
    metricsRow->setSpacing(16);

    auto makeMetricCard = [this](const QString &count, const QString &label, const QString &numColor) -> QFrame* {
        QFrame *card = new QFrame(candidateAppsPageWidget);
        card->setObjectName("candMetricCard");
        card->setStyleSheet("#candMetricCard { background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 14px 18px; } #candMetricCard QLabel { border: none; background: transparent; }");
        QVBoxLayout *l = new QVBoxLayout(card);
        l->setSpacing(2);
        QLabel *valLbl = new QLabel(count, card);
        valLbl->setStyleSheet(QString("font-size: 24px; font-weight: 800; color: %1; border: none; background: transparent;").arg(numColor));
        QLabel *titleLbl = new QLabel(label, card);
        titleLbl->setStyleSheet("font-size: 12px; color: #6C757D; font-weight: 600; border: none; background: transparent;");
        l->addWidget(valLbl);
        l->addWidget(titleLbl);
        return card;
    };

    metricsRow->addWidget(makeMetricCard(QString::number(totalCount), "Tổng số đơn đã nộp", "#1E1E1E"), 1);
    metricsRow->addWidget(makeMetricCard(QString::number(pendingCount), "⏳ Đang chờ xét duyệt", "#B76E00"), 1);
    metricsRow->addWidget(makeMetricCard(QString::number(acceptedCount), "✓ Đã được chấp nhận", "#1B7F49"), 1);
    metricsRow->addWidget(makeMetricCard(QString::number(rejectedCount), "✕ Đã bị từ chối", "#DC2626"), 1);
    candidateAppsContentLayout->addLayout(metricsRow);

    // 3. Filter tabs bar
    QFrame *filterFrame = new QFrame(candidateAppsPageWidget);
    filterFrame->setObjectName("candFilterFrame");
    filterFrame->setStyleSheet("#candFilterFrame { background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 10px 16px; } #candFilterFrame QLabel { border: none; background: transparent; }");
    QHBoxLayout *filterLayout = new QHBoxLayout(filterFrame);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(10);

    QLabel *filterTitle = new QLabel("Lọc trạng thái:", filterFrame);
    filterTitle->setStyleSheet("font-size: 13px; font-weight: 700; color: #1E1E1E; border: none; background: transparent;");
    filterLayout->addWidget(filterTitle);

    auto makeTabBtn = [filterFrame](const QString &text, bool active) -> QPushButton* {
        QPushButton *btn = new QPushButton(text, filterFrame);
        btn->setCursor(Qt::PointingHandCursor);
        if (active) {
            btn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 700; border: none; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
        } else {
            btn->setStyleSheet("background-color: #F8F4F0; color: #6C757D; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
        }
        return btn;
    };

    QPushButton *tabAll = makeTabBtn(QString("Tất cả (%1)").arg(totalCount), true);
    QPushButton *tabPending = makeTabBtn(QString("⏳ Chờ duyệt (%1)").arg(pendingCount), false);
    QPushButton *tabAccepted = makeTabBtn(QString("✓ Đã nhận (%1)").arg(acceptedCount), false);
    QPushButton *tabRejected = makeTabBtn(QString("✕ Từ chối (%1)").arg(rejectedCount), false);

    filterLayout->addWidget(tabAll);
    filterLayout->addWidget(tabPending);
    filterLayout->addWidget(tabAccepted);
    filterLayout->addWidget(tabRejected);
    filterLayout->addStretch();
    candidateAppsContentLayout->addWidget(filterFrame);

    // 4. List container
    QWidget *cardsContainer = new QWidget(candidateAppsPageWidget);
    cardsContainer->setStyleSheet("background-color: transparent;");
    QVBoxLayout *cardsLayout = new QVBoxLayout(cardsContainer);
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    cardsLayout->setSpacing(12);

    auto renderCards = [this, apps, cardsLayout, cardsContainer](const QString &filterStatus) {
        safeClearLayout(cardsLayout);

        int count = 0;
        for (const auto &app : apps) {
            if (filterStatus != "ALL" && app.getStatus() != filterStatus.toStdString()) {
                continue;
            }
            count++;

            int jobId = app.getIDjob();
            Job job;
            std::string jTitle = "Vị trí #" + std::to_string(jobId);
            std::string compName = "Doanh nghiệp";
            Company comp;
            if (m_data.getJobByID(jobId, job)) {
                jTitle = job.getTitle();
                if (m_data.getCompanyByID(job.getIDcompany(), comp)) {
                    compName = comp.getNameCompany();
                }
            }

            QFrame *card = new QFrame(cardsContainer);
            card->setObjectName("candAppCard");
            card->setStyleSheet("#candAppCard { background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; } #candAppCard QLabel { border: none; background: transparent; }");
            QHBoxLayout *row = new QHBoxLayout(card);
            row->setContentsMargins(18, 14, 18, 14);
            row->setSpacing(18);
            row->setAlignment(Qt::AlignVCenter);

            // Logo
            QString qCompName = QString::fromStdString(compName);
            QString compInitials = StyleHelper::getInitials(qCompName, "DN");
            QLabel *logo = new QLabel(card);
            logo->setAlignment(Qt::AlignCenter);
            logo->setFixedSize(54, 54);
            QString logoPath = StyleHelper::getCompanyLogoPath(comp.getIDcompany());
            logo->setPixmap(StyleHelper::getRoundedBadge(logoPath, compInitials, 54, 12));

            // Job info
            QVBoxLayout *tBox = new QVBoxLayout();
            tBox->setSpacing(4);
            tBox->setAlignment(Qt::AlignVCenter);

            QLabel *jLbl = new QLabel(QString::fromStdString(jTitle), card);
            jLbl->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E; border: none; background: transparent;");

            QHBoxLayout *meta1 = new QHBoxLayout();
            meta1->setSpacing(14);
            QLabel *cNameLbl = new QLabel("🏢 " + qCompName, card);
            cNameLbl->setStyleSheet("font-size: 13px; font-weight: 600; color: #B95334; border: none; background: transparent;");
            QLabel *locLbl = new QLabel("📍 " + QString::fromStdString(job.getLocation().empty() ? "Toàn quốc" : job.getLocation()), card);
            locLbl->setStyleSheet("font-size: 12px; color: #6C757D; border: none; background: transparent;");
            meta1->addWidget(cNameLbl);
            meta1->addWidget(locLbl);
            meta1->addStretch();

            QHBoxLayout *meta2 = new QHBoxLayout();
            meta2->setSpacing(14);
            QLabel *salLbl = new QLabel("💰 " + (job.getSalary() > 0 ? QLocale(QLocale::Vietnamese).toString(job.getSalary()) + " VNĐ" : "Thương lượng"), card);
            salLbl->setStyleSheet("font-size: 12px; color: #1B7F49; font-weight: 600; border: none; background: transparent;");
            QLabel *dateLbl = new QLabel("📅 Nộp: " + QString::fromStdString(app.getAppliedAt()), card);
            dateLbl->setStyleSheet("font-size: 12px; color: #6C757D; border: none; background: transparent;");
            meta2->addWidget(salLbl);
            meta2->addWidget(dateLbl);
            meta2->addStretch();

            tBox->addWidget(jLbl);
            tBox->addLayout(meta1);
            tBox->addLayout(meta2);

            // Match Score Pill
            float appScore = app.getAppropriateLevel();
            bool isHighScore = (appScore >= 70.0f);
            QString scoreText;
            if (appScore <= 0.0f) {
                scoreText = "🎯 Chưa đủ điều kiện";
            } else if (isHighScore) {
                scoreText = QString("🎯 %1% Phù hợp").arg(QString::number(appScore, 'f', 1));
            } else {
                scoreText = QString("🎯 %1% Chưa đạt gợi ý").arg(QString::number(appScore, 'f', 1));
            }
            QLabel *scoreBadge = new QLabel(scoreText, card);
            scoreBadge->setStyleSheet(isHighScore
                ? "background-color: #E3F7EB; color: #1B7F49; font-weight: 700; font-size: 13px; padding: 6px 12px; border-radius: 8px; border: none;"
                : "background-color: #FEF3E2; color: #B76E00; font-weight: 700; font-size: 13px; padding: 6px 12px; border-radius: 8px; border: none;");

            // Status Badge
            QLabel *stBadge = new QLabel(card);
            stBadge->setAlignment(Qt::AlignCenter);
            std::string statusStr = app.getStatus();
            if (statusStr == "ACCEPTED") {
                stBadge->setText("✓ Đã chấp nhận");
                stBadge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-weight: 700; font-size: 12px; border-radius: 8px; padding: 6px 14px;");
            } else if (statusStr == "REJECTED") {
                stBadge->setText("✕ Đã từ chối");
                stBadge->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-weight: 700; font-size: 12px; border-radius: 8px; padding: 6px 14px;");
            } else {
                stBadge->setText("⏳ Đang chờ duyệt");
                stBadge->setStyleSheet("background-color: #FEF3E2; color: #B76E00; font-weight: 700; font-size: 12px; border-radius: 8px; padding: 6px 14px;");
            }

            // Action: Xem chi tiết việc làm
            QPushButton *viewJobBtn = new QPushButton("Xem tin tuyển dụng →", card);
            viewJobBtn->setCursor(Qt::PointingHandCursor);
            viewJobBtn->setStyleSheet("background-color: #FFF4ED; color: #B95334; border: 1px solid #F5D3C7; border-radius: 8px; padding: 8px 16px; font-size: 12px; font-weight: 700;");
            connect(viewJobBtn, &QPushButton::clicked, this, [this, jobId]() {
                onShowJobDetail(jobId);
            });

            row->addWidget(logo, 0, Qt::AlignVCenter);
            row->addLayout(tBox, 1);
            row->addWidget(scoreBadge, 0, Qt::AlignVCenter);
            row->addWidget(stBadge, 0, Qt::AlignVCenter);
            row->addWidget(viewJobBtn, 0, Qt::AlignVCenter);
            cardsLayout->addWidget(card);
        }

        if (count == 0) {
            QFrame *emptyCard = new QFrame(cardsContainer);
            emptyCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 40px;");
            QVBoxLayout *emptyLay = new QVBoxLayout(emptyCard);
            emptyLay->setSpacing(10);
            emptyLay->setAlignment(Qt::AlignCenter);

            QLabel *emptyIcon = new QLabel("📭", emptyCard);
            emptyIcon->setStyleSheet("font-size: 40px;");
            emptyIcon->setAlignment(Qt::AlignCenter);
            QLabel *emptyText = new QLabel("Không tìm thấy đơn ứng tuyển nào trong mục này.", emptyCard);
            emptyText->setStyleSheet("font-size: 14px; font-weight: 600; color: #6C757D;");
            emptyText->setAlignment(Qt::AlignCenter);

            emptyLay->addWidget(emptyIcon);
            emptyLay->addWidget(emptyText);
            cardsLayout->addWidget(emptyCard);
        }
    };

    auto setTabStyles = [=](QPushButton *activeBtn) {
        for (QPushButton *btn : {tabAll, tabPending, tabAccepted, tabRejected}) {
            if (btn == activeBtn) {
                btn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 700; border: none; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
            } else {
                btn->setStyleSheet("background-color: #F8F4F0; color: #6C757D; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
            }
        }
    };

    connect(tabAll, &QPushButton::clicked, this, [=]() { setTabStyles(tabAll); renderCards("ALL"); });
    connect(tabPending, &QPushButton::clicked, this, [=]() { setTabStyles(tabPending); renderCards("PENDING"); });
    connect(tabAccepted, &QPushButton::clicked, this, [=]() { setTabStyles(tabAccepted); renderCards("ACCEPTED"); });
    connect(tabRejected, &QPushButton::clicked, this, [=]() { setTabStyles(tabRejected); renderCards("REJECTED"); });

    renderCards("ALL");
    candidateAppsContentLayout->addWidget(cardsContainer);
    candidateAppsContentLayout->addStretch();
}

void MainWindow::showCompanyApplicationsPage(int jobId, const QString &jobTitle) {
    buildCompanyApplicationsPage(jobId, jobTitle);
    stackedWidget->setCurrentIndex(6);
    setActiveNavBtn(btnAppsNav);
}

void MainWindow::buildCompanyApplicationsPage(int jobId, const QString &jobTitle) {
    safeClearLayout(companyAppsContentLayout);

    Company comp;
    if (!m_data.getCompanyByUserID(currentUserId, comp) || comp.getIDcompany() <= 0) {
        QFrame *errCard = new QFrame(companyAppsPageWidget);
        errCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 32px;");
        QVBoxLayout *errLay = new QVBoxLayout(errCard);
        errLay->setSpacing(12);
        errLay->setAlignment(Qt::AlignCenter);

        QLabel *errTitle = new QLabel("Chưa tìm thấy thông tin doanh nghiệp", errCard);
        errTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
        QLabel *errSub = new QLabel("Vui lòng hoàn tất thông tin công ty trước khi quản lý hồ sơ ứng viên.", errCard);
        errSub->setStyleSheet("font-size: 13px; color: #6C757D;");
        errLay->addWidget(errTitle, 0, Qt::AlignCenter);
        errLay->addWidget(errSub, 0, Qt::AlignCenter);
        companyAppsContentLayout->addWidget(errCard);
        return;
    }

    std::vector<Job> companyJobs;
    m_data.getJobsByCompanyID(comp.getIDcompany(), companyJobs);

    std::vector<Application> allApps;
    m_data.getApplicationsByCompanyID(comp.getIDcompany(), allApps);

    // Filter target apps by jobId
    std::vector<Application> targetApps;
    for (const auto &app : allApps) {
        if (jobId == 0 || app.getIDjob() == jobId) {
            targetApps.push_back(app);
        }
    }

    // 1. Header Row
    QHBoxLayout *headerRow = new QHBoxLayout();
    headerRow->setSpacing(16);

    QPushButton *backBtn = new QPushButton("← Quay lại trang chủ", companyAppsPageWidget);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 8px 16px; font-weight: 600; color: #1E1E1E; font-size: 13px;");
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        setActiveNavBtn(btnHomeNav);
        stackedWidget->setCurrentIndex(1);
    });

    QVBoxLayout *headerText = new QVBoxLayout();
    headerText->setSpacing(2);
    QLabel *headerTitle = new QLabel("📥 Quản lý hồ sơ ứng tuyển", companyAppsPageWidget);
    headerTitle->setStyleSheet("font-size: 22px; font-weight: 800; color: #1E1E1E;");
    QString subText = (jobId > 0)
        ? QString("Xem xét và xét duyệt hồ sơ ứng viên ứng tuyển vị trí: %1").arg(jobTitle)
        : "Đánh giá mức độ phù hợp và ra quyết định tuyển dụng đối với toàn bộ hồ sơ ứng viên";
    QLabel *headerSubtitle = new QLabel(subText, companyAppsPageWidget);
    headerSubtitle->setStyleSheet("font-size: 13px; color: #6C757D;");
    headerText->addWidget(headerTitle);
    headerText->addWidget(headerSubtitle);

    headerRow->addWidget(backBtn, 0, Qt::AlignVCenter);
    headerRow->addLayout(headerText, 1);
    headerRow->addStretch();
    companyAppsContentLayout->addLayout(headerRow);

    // Tính thống kê
    int totalCount = static_cast<int>(targetApps.size());
    int pendingCount = 0;
    int acceptedCount = 0;
    int rejectedCount = 0;
    for (const auto &a : targetApps) {
        if (a.getStatus() == "ACCEPTED") acceptedCount++;
        else if (a.getStatus() == "REJECTED") rejectedCount++;
        else pendingCount++;
    }

    // 2. Metrics Bar Row
    QHBoxLayout *metricsRow = new QHBoxLayout();
    metricsRow->setSpacing(16);

    auto makeMetricCard = [this](const QString &count, const QString &label, const QString &numColor) -> QFrame* {
        QFrame *card = new QFrame(companyAppsPageWidget);
        card->setObjectName("compMetricCard");
        card->setStyleSheet("#compMetricCard { background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 14px 18px; } #compMetricCard QLabel { border: none; background: transparent; }");
        QVBoxLayout *l = new QVBoxLayout(card);
        l->setSpacing(2);
        QLabel *valLbl = new QLabel(count, card);
        valLbl->setStyleSheet(QString("font-size: 24px; font-weight: 800; color: %1; border: none; background: transparent;").arg(numColor));
        QLabel *titleLbl = new QLabel(label, card);
        titleLbl->setStyleSheet("font-size: 12px; color: #6C757D; font-weight: 600; border: none; background: transparent;");
        l->addWidget(valLbl);
        l->addWidget(titleLbl);
        return card;
    };

    metricsRow->addWidget(makeMetricCard(QString::number(totalCount), "Tổng hồ sơ đã nhận", "#1E1E1E"), 1);
    metricsRow->addWidget(makeMetricCard(QString::number(pendingCount), "⏳ Chờ xét duyệt", "#B76E00"), 1);
    metricsRow->addWidget(makeMetricCard(QString::number(acceptedCount), "✓ Đã chấp thuận", "#1B7F49"), 1);
    metricsRow->addWidget(makeMetricCard(QString::number(rejectedCount), "✕ Đã từ chối", "#DC2626"), 1);
    companyAppsContentLayout->addLayout(metricsRow);

    // 3. Filter Bar (Job Selector Combobox + Status Filter Tabs)
    QFrame *filterFrame = new QFrame(companyAppsPageWidget);
    filterFrame->setObjectName("compFilterFrame");
    filterFrame->setStyleSheet("#compFilterFrame { background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 10px 16px; } #compFilterFrame QLabel { border: none; background: transparent; }");
    QHBoxLayout *filterLayout = new QHBoxLayout(filterFrame);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(12);

    QLabel *jobFilterLbl = new QLabel("Lọc theo vị trí:", filterFrame);
    jobFilterLbl->setStyleSheet("font-size: 13px; font-weight: 700; color: #1E1E1E;");
    filterLayout->addWidget(jobFilterLbl);

    QComboBox *jobCombo = new QComboBox(filterFrame);
    jobCombo->setView(new QListView(jobCombo));
    jobCombo->setStyleSheet(
        "QComboBox {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 8px;"
        "   padding: 7px 12px;"
        "   font-size: 13px;"
        "   font-weight: 600;"
        "   color: #1E1E1E;"
        "   min-width: 250px;"
        "}"
        "QComboBox:hover {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #B95334;"
        "}"
        "QComboBox:on, QComboBox:pressed, QComboBox:focus {"
        "   background-color: #FFFFFF;"
        "   color: #1E1E1E;"
        "   border: 1.5px solid #B95334;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   background: transparent;"
        "   width: 24px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #FFFFFF;"
        "   color: #1E1E1E;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 8px;"
        "   padding: 4px;"
        "   outline: none;"
        "   selection-background-color: #FFF4ED;"
        "   selection-color: #B95334;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "   color: #1E1E1E;"
        "   background-color: #FFFFFF;"
        "   padding: 8px 12px;"
        "   min-height: 24px;"
        "   border-radius: 4px;"
        "}"
        "QComboBox QAbstractItemView::item:hover,"
        "QComboBox QAbstractItemView::item:selected {"
        "   background-color: #FFF4ED;"
        "   color: #B95334;"
        "}"
    );
    jobCombo->addItem("Tất cả vị trí tuyển dụng", 0);
    int selectedComboIndex = 0;
    for (size_t i = 0; i < companyJobs.size(); ++i) {
        const auto &j = companyJobs[i];
        jobCombo->addItem(QString::fromStdString(j.getTitle()), j.getIDjob());
        if (j.getIDjob() == jobId) {
            selectedComboIndex = static_cast<int>(i) + 1;
        }
    }
    jobCombo->setCurrentIndex(selectedComboIndex);
    connect(jobCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, jobCombo](int index) {
        int newJobId = jobCombo->itemData(index).toInt();
        QString newTitle = jobCombo->itemText(index);
        buildCompanyApplicationsPage(newJobId, newTitle);
    });
    filterLayout->addWidget(jobCombo);

    QFrame *sep = new QFrame(filterFrame);
    sep->setFrameShape(QFrame::VLine);
    sep->setStyleSheet("background-color: #ECE5DE; margin: 4px 6px;");
    filterLayout->addWidget(sep);

    auto makeTabBtn = [filterFrame](const QString &text, bool active) -> QPushButton* {
        QPushButton *btn = new QPushButton(text, filterFrame);
        btn->setCursor(Qt::PointingHandCursor);
        if (active) {
            btn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 700; border: none; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
        } else {
            btn->setStyleSheet("background-color: #F8F4F0; color: #6C757D; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
        }
        return btn;
    };

    QPushButton *tabAll = makeTabBtn(QString("Tất cả (%1)").arg(totalCount), true);
    QPushButton *tabPending = makeTabBtn(QString("⏳ Chờ duyệt (%1)").arg(pendingCount), false);
    QPushButton *tabAccepted = makeTabBtn(QString("✓ Đã nhận (%1)").arg(acceptedCount), false);
    QPushButton *tabRejected = makeTabBtn(QString("✕ Từ chối (%1)").arg(rejectedCount), false);

    filterLayout->addWidget(tabAll);
    filterLayout->addWidget(tabPending);
    filterLayout->addWidget(tabAccepted);
    filterLayout->addWidget(tabRejected);
    filterLayout->addStretch();
    companyAppsContentLayout->addWidget(filterFrame);

    // 4. Applicants Cards List
    QWidget *cardsContainer = new QWidget(companyAppsPageWidget);
    cardsContainer->setStyleSheet("background-color: transparent;");
    QVBoxLayout *cardsLayout = new QVBoxLayout(cardsContainer);
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    cardsLayout->setSpacing(12);

    auto renderApplicants = [this, targetApps, cardsLayout, cardsContainer, jobId, jobTitle](const QString &filterStatus) {
        safeClearLayout(cardsLayout);

        int count = 0;
        for (const auto &app : targetApps) {
            if (filterStatus != "ALL" && app.getStatus() != filterStatus.toStdString()) {
                continue;
            }
            count++;

            Candidate cand;
            std::string cname = "Ứng viên #" + std::to_string(app.getIDcandidate());
            std::string cmajor = "Chưa cập nhật";
            std::string cuni = "";
            if (m_data.getCandidateByID(app.getIDcandidate(), cand)) {
                cname = cand.getPersonalInfo().fullName;
                cmajor = cand.getProfessionalInfo().major;
                cuni = cand.getProfessionalInfo().school;
            }
            QString qCandName = QString::fromStdString(cname.empty() ? "Ứng viên" : cname);

            Job job;
            std::string jTitle = "Vị trí tuyển dụng";
            if (m_data.getJobByID(app.getIDjob(), job)) {
                jTitle = job.getTitle();
            }

            QFrame *card = new QFrame(cardsContainer);
            card->setObjectName("candAppCard");
            card->setStyleSheet("#candAppCard { background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; } #candAppCard QLabel { border: none; background: transparent; }");
            QHBoxLayout *row = new QHBoxLayout(card);
            row->setContentsMargins(18, 14, 18, 14);
            row->setSpacing(18);
            row->setAlignment(Qt::AlignVCenter);

            // Candidate Avatar
            QString initials = StyleHelper::getInitials(qCandName, "UV");
            QLabel *avatar = new QLabel(card);
            avatar->setAlignment(Qt::AlignCenter);
            avatar->setFixedSize(54, 54);
            int candId = cand.getIDcandidate() > 0 ? cand.getIDcandidate() : app.getIDcandidate();
            QString avPath = StyleHelper::getCandidateAvatarPath(candId);
            avatar->setPixmap(StyleHelper::getCircularAvatar(avPath, initials, 54));

            // Info column
            QVBoxLayout *tBox = new QVBoxLayout();
            tBox->setSpacing(4);
            tBox->setAlignment(Qt::AlignVCenter);

            QLabel *nameLbl = new QLabel(qCandName, card);
            nameLbl->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E; border: none; background: transparent;");

            QHBoxLayout *meta1 = new QHBoxLayout();
            meta1->setSpacing(14);
            QLabel *jobLbl = new QLabel("💼 Vị trí: " + QString::fromStdString(jTitle), card);
            jobLbl->setStyleSheet("font-size: 13px; font-weight: 600; color: #B95334; border: none; background: transparent;");
            QLabel *dateLbl = new QLabel("📅 Nộp: " + QString::fromStdString(app.getAppliedAt()), card);
            dateLbl->setStyleSheet("font-size: 12px; color: #6C757D; border: none; background: transparent;");
            meta1->addWidget(jobLbl);
            meta1->addWidget(dateLbl);
            meta1->addStretch();

            QHBoxLayout *meta2 = new QHBoxLayout();
            meta2->setSpacing(14);
            QLabel *majorLbl = new QLabel("🎓 Ngành: " + QString::fromStdString(cmajor.empty() ? "Chưa cập nhật" : cmajor), card);
            majorLbl->setStyleSheet("font-size: 12px; color: #4A4A4A; border: none; background: transparent;");
            meta2->addWidget(majorLbl);
            if (!cuni.empty()) {
                QLabel *uniLbl = new QLabel("🏫 " + QString::fromStdString(cuni), card);
                uniLbl->setStyleSheet("font-size: 12px; color: #6C757D; border: none; background: transparent;");
                meta2->addWidget(uniLbl);
            }
            meta2->addStretch();

            tBox->addWidget(nameLbl);
            tBox->addLayout(meta1);
            tBox->addLayout(meta2);

            // Match Score Pill
            float appScore = app.getAppropriateLevel();
            bool isHighScore = (appScore >= 70.0f);
            QString scoreText;
            if (appScore <= 0.0f) {
                scoreText = "🎯 Chưa đủ điều kiện";
            } else if (isHighScore) {
                scoreText = QString("🎯 %1% Phù hợp").arg(QString::number(appScore, 'f', 1));
            } else {
                scoreText = QString("🎯 %1% Chưa đạt gợi ý").arg(QString::number(appScore, 'f', 1));
            }
            QLabel *scoreBadge = new QLabel(scoreText, card);
            scoreBadge->setStyleSheet(isHighScore
                ? "background-color: #E3F7EB; color: #1B7F49; font-weight: 700; font-size: 13px; padding: 6px 12px; border-radius: 8px; border: none;"
                : "background-color: #FEF3E2; color: #B76E00; font-weight: 700; font-size: 13px; padding: 6px 12px; border-radius: 8px; border: none;");

            // Status Badge
            QLabel *stBadge = new QLabel(card);
            stBadge->setAlignment(Qt::AlignCenter);
            std::string st = app.getStatus();
            if (st == "ACCEPTED") {
                stBadge->setText("✓ Đã chấp thuận");
                stBadge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-weight: 700; font-size: 12px; border-radius: 8px; padding: 6px 14px;");
            } else if (st == "REJECTED") {
                stBadge->setText("✕ Đã từ chối");
                stBadge->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-weight: 700; font-size: 12px; border-radius: 8px; padding: 6px 14px;");
            } else {
                stBadge->setText("⏳ Đang chờ xét duyệt");
                stBadge->setStyleSheet("background-color: #FEF3E2; color: #B76E00; font-weight: 700; font-size: 12px; border-radius: 8px; padding: 6px 14px;");
            }

            // Buttons layout
            QHBoxLayout *btnsRow = new QHBoxLayout();
            btnsRow->setSpacing(8);

            QPushButton *viewDetailBtn = new QPushButton("👤 Xem hồ sơ", card);
            viewDetailBtn->setCursor(Qt::PointingHandCursor);
            viewDetailBtn->setStyleSheet("background-color: #FFFFFF; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 8px; padding: 8px 14px; font-size: 12px; font-weight: 600;");

            int cId = app.getIDcandidate();
            float cScore = app.getAppropriateLevel();
            QString qTitle = QString::fromStdString(jTitle);
            int appId = app.getIDappli();
            int currJobId = app.getIDjob();
            QString qSt = QString::fromStdString(st);

            connect(viewDetailBtn, &QPushButton::clicked, this, [this, cId, qTitle, cScore, appId, qSt, currJobId, jobId, jobTitle]() {
                companyPage->viewCandidateDetails(cId, qTitle, cScore, appId, qSt, currJobId);
                buildCompanyApplicationsPage(jobId, jobTitle);
            });
            btnsRow->addWidget(viewDetailBtn);

            if (st == "PENDING") {
                QPushButton *acceptBtn = new QPushButton("✓ Chấp thuận", card);
                acceptBtn->setCursor(Qt::PointingHandCursor);
                acceptBtn->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; border: 1px solid #C4EDD4; border-radius: 8px; padding: 8px 14px; font-size: 12px; font-weight: 700;");

                QPushButton *rejectBtn = new QPushButton("✕ Từ chối", card);
                rejectBtn->setCursor(Qt::PointingHandCursor);
                rejectBtn->setStyleSheet("background-color: #FEE2E2; color: #DC2626; border: 1px solid #FCA5A5; border-radius: 8px; padding: 8px 14px; font-size: 12px; font-weight: 700;");

                connect(acceptBtn, &QPushButton::clicked, this, [this, appId, currJobId, qCandName, jobId, jobTitle]() {
                    if (companyPage->processApplicationDecision(appId, currJobId, qCandName, "ACCEPTED", this)) {
                        buildCompanyApplicationsPage(jobId, jobTitle);
                    }
                });

                connect(rejectBtn, &QPushButton::clicked, this, [this, appId, currJobId, qCandName, jobId, jobTitle]() {
                    if (companyPage->processApplicationDecision(appId, currJobId, qCandName, "REJECTED", this)) {
                        buildCompanyApplicationsPage(jobId, jobTitle);
                    }
                });

                btnsRow->addWidget(acceptBtn);
                btnsRow->addWidget(rejectBtn);
            }

            row->addWidget(avatar, 0, Qt::AlignVCenter);
            row->addLayout(tBox, 1);
            row->addWidget(scoreBadge, 0, Qt::AlignVCenter);
            row->addWidget(stBadge, 0, Qt::AlignVCenter);
            row->addLayout(btnsRow);
            cardsLayout->addWidget(card);
        }

        if (count == 0) {
            QFrame *emptyCard = new QFrame(cardsContainer);
            emptyCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 40px;");
            QVBoxLayout *emptyLay = new QVBoxLayout(emptyCard);
            emptyLay->setSpacing(10);
            emptyLay->setAlignment(Qt::AlignCenter);

            QLabel *emptyIcon = new QLabel("📭", emptyCard);
            emptyIcon->setStyleSheet("font-size: 40px;");
            emptyIcon->setAlignment(Qt::AlignCenter);
            QLabel *emptyText = new QLabel("Không có hồ sơ ứng tuyển nào trong danh mục này.", emptyCard);
            emptyText->setStyleSheet("font-size: 14px; font-weight: 600; color: #6C757D;");
            emptyText->setAlignment(Qt::AlignCenter);

            emptyLay->addWidget(emptyIcon);
            emptyLay->addWidget(emptyText);
            cardsLayout->addWidget(emptyCard);
        }
    };

    auto setTabStyles = [=](QPushButton *activeBtn) {
        for (QPushButton *btn : {tabAll, tabPending, tabAccepted, tabRejected}) {
            if (btn == activeBtn) {
                btn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 700; border: none; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
            } else {
                btn->setStyleSheet("background-color: #F8F4F0; color: #6C757D; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
            }
        }
    };

    connect(tabAll, &QPushButton::clicked, this, [=]() { setTabStyles(tabAll); renderApplicants("ALL"); });
    connect(tabPending, &QPushButton::clicked, this, [=]() { setTabStyles(tabPending); renderApplicants("PENDING"); });
    connect(tabAccepted, &QPushButton::clicked, this, [=]() { setTabStyles(tabAccepted); renderApplicants("ACCEPTED"); });
    connect(tabRejected, &QPushButton::clicked, this, [=]() { setTabStyles(tabRejected); renderApplicants("REJECTED"); });

    renderApplicants("ALL");
    companyAppsContentLayout->addWidget(cardsContainer);
    companyAppsContentLayout->addStretch();
}
