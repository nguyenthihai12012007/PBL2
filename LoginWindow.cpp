#include "LoginWindow.h"
#include "StyleHelper.h"
#include "CustomMessageBox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include <QFrame>

LoginWindow::LoginWindow(Data &data, QWidget *parent)
    : QWidget(parent), m_data(data) {
    setWindowTitle("JobMatch C++ — Đăng nhập");
    setFixedSize(460, 560);
    setupUI();
}

void LoginWindow::setupUI() {
    // Root layout centering the card
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(30, 30, 30, 30);
    rootLayout->setAlignment(Qt::AlignCenter);

    // Card frame
    QFrame *cardFrame = new QFrame(this);
    cardFrame->setObjectName("cardFrame");
    cardFrame->setStyleSheet(
        "#cardFrame {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 12px;"
        "   padding: 24px;"
        "}"
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setSpacing(16);

    // Header Branding
    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);

    QLabel *logoLabel = new QLabel("JobMatch C++", this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("font-size: 24px; font-weight: 800; color: #B95334; letter-spacing: 0.5px;");

    QLabel *subtitleLabel = new QLabel("Kết nối tài năng với cơ hội việc làm", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 13px; color: #6C757D;");

    headerLayout->addWidget(logoLabel);
    headerLayout->addWidget(subtitleLabel);
    cardLayout->addLayout(headerLayout);

    // Form Stacked Widget (Page 0: Login, Page 1: Register)
    formStackedWidget = new QStackedWidget(this);

    // ==========================================
    // PAGE 0: LOGIN FORM
    // ==========================================
    QWidget *loginPage = new QWidget(this);
    QVBoxLayout *loginPageLayout = new QVBoxLayout(loginPage);
    loginPageLayout->setContentsMargins(0, 10, 0, 0);
    loginPageLayout->setSpacing(14);

    QFormLayout *loginForm = new QFormLayout();
    loginForm->setLabelAlignment(Qt::AlignLeft);
    loginForm->setSpacing(12);

    loginEmailEdit = new QLineEdit(this);
    loginEmailEdit->setPlaceholderText("Nhập email tài khoản đã có");

    loginPasswordEdit = new QLineEdit(this);
    loginPasswordEdit->setPlaceholderText("Nhập mật khẩu");
    loginPasswordEdit->setEchoMode(QLineEdit::Password);

    loginForm->addRow("Email:", loginEmailEdit);
    loginForm->addRow("Mật khẩu:", loginPasswordEdit);
    loginPageLayout->addLayout(loginForm);

    loginStatusLabel = new QLabel(this);
    loginStatusLabel->setStyleSheet("color: #DC2626; font-size: 12px; font-weight: 600; padding: 4px;");
    loginStatusLabel->setAlignment(Qt::AlignCenter);
    loginStatusLabel->setWordWrap(true);
    loginStatusLabel->setVisible(false);
    loginPageLayout->addWidget(loginStatusLabel);

    loginButton = new QPushButton("Đăng nhập hệ thống", this);
    loginButton->setProperty("class", "primaryButton");
    loginButton->setCursor(Qt::PointingHandCursor);
    loginPageLayout->addWidget(loginButton);

    QHBoxLayout *switchRegLayout = new QHBoxLayout();
    switchRegLayout->setAlignment(Qt::AlignCenter);
    QLabel *noAccountLabel = new QLabel("Chưa có tài khoản?", this);
    noAccountLabel->setStyleSheet("color: #6A625A; font-size: 12px;");
    QPushButton *switchToRegBtn = new QPushButton("Đăng ký ngay", this);
    switchToRegBtn->setStyleSheet("background: transparent; border: none; color: #B95334; font-weight: 700; font-size: 12px;");
    switchToRegBtn->setCursor(Qt::PointingHandCursor);
    switchRegLayout->addWidget(noAccountLabel);
    switchRegLayout->addWidget(switchToRegBtn);
    loginPageLayout->addLayout(switchRegLayout);

    formStackedWidget->addWidget(loginPage);

    // ==========================================
    // PAGE 1: REGISTER FORM
    // ==========================================
    QWidget *regPage = new QWidget(this);
    QVBoxLayout *regPageLayout = new QVBoxLayout(regPage);
    regPageLayout->setContentsMargins(0, 10, 0, 0);
    regPageLayout->setSpacing(12);

    QFormLayout *regForm = new QFormLayout();
    regForm->setLabelAlignment(Qt::AlignLeft);
    regForm->setSpacing(10);

    regEmailEdit = new QLineEdit(this);
    regEmailEdit->setPlaceholderText("Nhập địa chỉ email mới");

    regPasswordEdit = new QLineEdit(this);
    regPasswordEdit->setPlaceholderText("Tối thiểu 6 ký tự");
    regPasswordEdit->setEchoMode(QLineEdit::Password);

    regConfirmPasswordEdit = new QLineEdit(this);
    regConfirmPasswordEdit->setPlaceholderText("Nhập lại mật khẩu");
    regConfirmPasswordEdit->setEchoMode(QLineEdit::Password);

    regRoleCombo = new QComboBox(this);
    regRoleCombo->addItem("Ứng viên tìm việc", "CANDIDATE");
    regRoleCombo->addItem("Doanh nghiệp tuyển dụng", "COMPANY");

    regForm->addRow("Email:", regEmailEdit);
    regForm->addRow("Mật khẩu:", regPasswordEdit);
    regForm->addRow("Xác nhận MK:", regConfirmPasswordEdit);
    regForm->addRow("Loại tài khoản:", regRoleCombo);
    regPageLayout->addLayout(regForm);

    regStatusLabel = new QLabel(this);
    regStatusLabel->setStyleSheet("color: #DC2626; font-size: 12px; font-weight: 600;");
    regStatusLabel->setAlignment(Qt::AlignCenter);
    regStatusLabel->setWordWrap(true);
    regStatusLabel->setVisible(false);
    regPageLayout->addWidget(regStatusLabel);

    regButton = new QPushButton("Đăng ký tài khoản", this);
    regButton->setProperty("class", "primaryButton");
    regButton->setCursor(Qt::PointingHandCursor);
    regPageLayout->addWidget(regButton);

    QHBoxLayout *switchLoginLayout = new QHBoxLayout();
    switchLoginLayout->setAlignment(Qt::AlignCenter);
    QLabel *hasAccountLabel = new QLabel("Đã có tài khoản?", this);
    hasAccountLabel->setStyleSheet("color: #6A625A; font-size: 12px;");
    QPushButton *switchToLoginBtn = new QPushButton("Đăng nhập", this);
    switchToLoginBtn->setStyleSheet("background: transparent; border: none; color: #B95334; font-weight: 700; font-size: 12px;");
    switchToLoginBtn->setCursor(Qt::PointingHandCursor);
    switchLoginLayout->addWidget(hasAccountLabel);
    switchLoginLayout->addWidget(switchToLoginBtn);
    regPageLayout->addLayout(switchLoginLayout);

    formStackedWidget->addWidget(regPage);

    cardLayout->addWidget(formStackedWidget);
    rootLayout->addWidget(cardFrame);

    // Connections
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(switchToRegBtn, &QPushButton::clicked, this, &LoginWindow::onSwitchToRegister);
    connect(regButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(switchToLoginBtn, &QPushButton::clicked, this, &LoginWindow::onSwitchToLogin);
    connect(loginPasswordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
    connect(loginEmailEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onSwitchToRegister() {
    loginStatusLabel->setVisible(false);
    formStackedWidget->setCurrentIndex(1);
    setWindowTitle("JobSystem - Đăng ký tài khoản");
}

void LoginWindow::onSwitchToLogin() {
    regStatusLabel->setVisible(false);
    formStackedWidget->setCurrentIndex(0);
    setWindowTitle("JobSystem - Đăng nhập");
}

void LoginWindow::onLoginClicked() {
    std::string email = loginEmailEdit->text().trimmed().toStdString();
    std::string password = loginPasswordEdit->text().toStdString();

    // Input validation
    if (email.empty()) {
        loginStatusLabel->setText("Vui lòng nhập địa chỉ email!");
        loginStatusLabel->setVisible(true);
        loginEmailEdit->setFocus();
        return;
    }

    if (password.empty()) {
        loginStatusLabel->setText("Vui lòng nhập mật khẩu!");
        loginStatusLabel->setVisible(true);
        loginPasswordEdit->setFocus();
        return;
    }

    // Authenticate with real data via Data::findUserByEmail
    User user;
    if (!m_data.findUserByEmail(email, user)) {
        loginStatusLabel->setText("Tài khoản email không tồn tại trong hệ thống!");
        loginStatusLabel->setVisible(true);
        loginEmailEdit->setFocus();
        return;
    }

    if (user.getPassword() != password) {
        loginStatusLabel->setText("Mật khẩu không chính xác. Vui lòng thử lại!");
        loginStatusLabel->setVisible(true);
        loginPasswordEdit->clear();
        loginPasswordEdit->setFocus();
        return;
    }

    loginStatusLabel->setVisible(false);

    int userId = user.getIDuser();
    QString userEmail = QString::fromStdString(user.getEmail());
    QString userRole = QString::fromStdString(user.getRole());

    // Emit authenticated signal with real user information
    emit loginSuccess(userId, userEmail, userRole);
}

void LoginWindow::onRegisterClicked() {
    std::string email = regEmailEdit->text().trimmed().toStdString();
    std::string password = regPasswordEdit->text().toStdString();
    std::string confirm = regConfirmPasswordEdit->text().toStdString();
    std::string role = regRoleCombo->currentData().toString().toStdString();

    if (email.empty()) {
        regStatusLabel->setText("Vui lòng nhập email đăng ký!");
        regStatusLabel->setVisible(true);
        regEmailEdit->setFocus();
        return;
    }

    if (email.find('@') == std::string::npos || email.find('.') == std::string::npos) {
        regStatusLabel->setText("Định dạng email không hợp lệ!");
        regStatusLabel->setVisible(true);
        regEmailEdit->setFocus();
        return;
    }

    if (password.length() < 6) {
        regStatusLabel->setText("Mật khẩu phải có ít nhất 6 ký tự!");
        regStatusLabel->setVisible(true);
        regPasswordEdit->setFocus();
        return;
    }

    if (password != confirm) {
        regStatusLabel->setText("Xác nhận mật khẩu không khớp!");
        regStatusLabel->setVisible(true);
        regConfirmPasswordEdit->setFocus();
        return;
    }

    // Check if email already exists in users.txt
    if (m_data.emailExists(email)) {
        regStatusLabel->setText("Email này đã được sử dụng. Vui lòng chọn email khác!");
        regStatusLabel->setVisible(true);
        regEmailEdit->setFocus();
        return;
    }

    int newId = m_data.getNextID("users.txt");
    User newUser(newId, email, password, role);

    if (m_data.registerUser(newUser)) {
        regStatusLabel->setVisible(false);
        CustomMessageBox::success(
            this,
            "Đăng ký thành công",
            QString("Tài khoản [%1] (Vai trò: %2) đã được tạo thành công vào hệ thống!\n"
                    "Bây giờ bạn có thể đăng nhập ngay.")
            .arg(QString::fromStdString(email), QString::fromStdString(role))
        );

        loginEmailEdit->setText(QString::fromStdString(email));
        loginPasswordEdit->clear();
        loginPasswordEdit->setFocus();
        onSwitchToLogin();
    } else {
        regStatusLabel->setText("Không thể lưu tài khoản vào cơ sở dữ liệu users.txt!");
        regStatusLabel->setVisible(true);
    }
}

void LoginWindow::clearInputs() {
    loginPasswordEdit->clear();
    loginStatusLabel->setVisible(false);
    regEmailEdit->clear();
    regPasswordEdit->clear();
    regConfirmPasswordEdit->clear();
    regStatusLabel->setVisible(false);
}
