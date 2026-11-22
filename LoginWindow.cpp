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
#include <QListView>
#include <QScrollArea>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QGraphicsDropShadowEffect>
#include <QDir>
#include <QFile>

namespace {
class CompactStackedWidget : public QStackedWidget {
public:
    explicit CompactStackedWidget(QWidget *parent = nullptr) : QStackedWidget(parent) {}
    QSize sizeHint() const override {
        if (currentWidget()) {
            return currentWidget()->sizeHint();
        }
        return QStackedWidget::sizeHint();
    }
    QSize minimumSizeHint() const override {
        if (currentWidget()) {
            return currentWidget()->minimumSizeHint();
        }
        return QStackedWidget::minimumSizeHint();
    }
};

class ChevronComboBox : public QComboBox {
public:
    explicit ChevronComboBox(QWidget *parent = nullptr) : QComboBox(parent) {}
protected:
    void paintEvent(QPaintEvent *event) override {
        QComboBox::paintEvent(event);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const int arrowX = width() - 18;
        const int arrowY = height() / 2;
        const QColor color = (underMouse() || hasFocus()) ? QColor("#B95334") : QColor("#718096");

        QPen pen(color, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);

        const QPointF points[3] = {
            QPointF(arrowX - 5.5, arrowY - 3.0),
            QPointF(arrowX, arrowY + 3.0),
            QPointF(arrowX + 5.5, arrowY - 3.0)
        };
        p.drawPolyline(points, 3);
    }
};
}

LoginWindow::LoginWindow(Data &data, QWidget *parent)
    : QWidget(parent), m_data(data), m_currentTheme(ThemeStyle::HiringWorkspace),
      illustrationLabel(nullptr), rightContainer(nullptr), cardFrame(nullptr), cardShadow(nullptr),
      logoLabel(nullptr), subtitleLabel(nullptr),
      emailLabel(nullptr), pwdLabel(nullptr), loginEmailEdit(nullptr), loginPasswordEdit(nullptr),
      loginStatusLabel(nullptr), loginButton(nullptr), noAccountLabel(nullptr), switchToRegBtn(nullptr),
      regEmailLbl(nullptr), regPwdLbl(nullptr), regConfirmLbl(nullptr), regRoleLbl(nullptr),
      regEmailEdit(nullptr), regPasswordEdit(nullptr), regConfirmPasswordEdit(nullptr), regRoleCombo(nullptr),
      regStatusLabel(nullptr), regButton(nullptr), hasAccountLabel(nullptr), switchToLoginBtn(nullptr),
      formStackedWidget(nullptr) {
    setWindowTitle("JobMatch — Đăng nhập");
    resize(1080, 630);
    setMinimumSize(920, 540);
    setupUI();
    applyTheme(m_currentTheme);
}

void LoginWindow::setupUI() {
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 32, 0);
    rootLayout->setSpacing(0);

    rootLayout->addStretch(1);

    rightContainer = new QWidget(this);
    rightContainer->setFixedWidth(560);
    rightContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rightContainer->setStyleSheet("background-color: transparent; border: none;");

    QVBoxLayout *rightRootLayout = new QVBoxLayout(rightContainer);
    rightRootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(rightContainer);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("background-color: transparent;");

    QVBoxLayout *centerVLayout = new QVBoxLayout(scrollContent);
    centerVLayout->setContentsMargins(16, 12, 16, 12);
    centerVLayout->setAlignment(Qt::AlignCenter);

    cardFrame = new QFrame(scrollContent);
    cardFrame->setObjectName("cardFrame");
    cardFrame->setFixedWidth(510);

    cardShadow = new QGraphicsDropShadowEffect(cardFrame);
    cardFrame->setGraphicsEffect(cardShadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(16);

    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(4);

    logoLabel = new QLabel("JobMatch", this);
    logoLabel->setAlignment(Qt::AlignCenter);

    subtitleLabel = new QLabel("Kết nối tài năng với cơ hội việc làm", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(logoLabel);
    headerLayout->addWidget(subtitleLabel);
    cardLayout->addLayout(headerLayout);

    formStackedWidget = new CompactStackedWidget(cardFrame);
    formStackedWidget->setFixedWidth(450);
    formStackedWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

    QWidget *loginPage = new QWidget(formStackedWidget);
    loginPage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    QVBoxLayout *loginPageLayout = new QVBoxLayout(loginPage);
    loginPageLayout->setContentsMargins(0, 4, 0, 0);
    loginPageLayout->setSpacing(16);
    loginPageLayout->setAlignment(Qt::AlignTop);

    QFormLayout *loginForm = new QFormLayout();
    loginForm->setContentsMargins(0, 0, 0, 0);
    loginForm->setLabelAlignment(Qt::AlignLeft);
    loginForm->setVerticalSpacing(16);
    loginForm->setHorizontalSpacing(16);

    loginEmailEdit = new QLineEdit(loginPage);
    loginEmailEdit->setPlaceholderText("Nhập email tài khoản đã có");
    loginEmailEdit->setFixedHeight(56);

    loginPasswordEdit = new QLineEdit(loginPage);
    loginPasswordEdit->setPlaceholderText("Nhập mật khẩu");
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setFixedHeight(56);

    emailLabel = new QLabel("Email:", loginPage);
    pwdLabel = new QLabel("Mật khẩu:", loginPage);

    loginForm->addRow(emailLabel, loginEmailEdit);
    loginForm->addRow(pwdLabel, loginPasswordEdit);
    loginPageLayout->addLayout(loginForm);

    loginStatusLabel = new QLabel(loginPage);
    loginStatusLabel->setStyleSheet("color: #DC2626; font-size: 16px; font-weight: 600; padding: 2px;");
    loginStatusLabel->setAlignment(Qt::AlignCenter);
    loginStatusLabel->setWordWrap(true);
    loginStatusLabel->setVisible(false);
    loginPageLayout->addWidget(loginStatusLabel);

    loginButton = new QPushButton("Đăng nhập hệ thống", loginPage);
    loginButton->setObjectName("loginButton");
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setFixedWidth(360);

    QHBoxLayout *loginBtnLayout = new QHBoxLayout();
    loginBtnLayout->setContentsMargins(0, 6, 0, 6);
    loginBtnLayout->setAlignment(Qt::AlignCenter);
    loginBtnLayout->addWidget(loginButton);
    loginPageLayout->addLayout(loginBtnLayout);

    QHBoxLayout *switchRegLayout = new QHBoxLayout();
    switchRegLayout->setContentsMargins(0, 8, 0, 0);
    switchRegLayout->setSpacing(8);
    switchRegLayout->setAlignment(Qt::AlignCenter);
    noAccountLabel = new QLabel("Chưa có tài khoản?", loginPage);
    switchToRegBtn = new QPushButton("Đăng ký ngay", loginPage);
    switchToRegBtn->setObjectName("switchToRegBtn");
    switchToRegBtn->setCursor(Qt::PointingHandCursor);
    switchRegLayout->addWidget(noAccountLabel);
    switchRegLayout->addWidget(switchToRegBtn);
    loginPageLayout->addLayout(switchRegLayout);

    formStackedWidget->addWidget(loginPage);

    QWidget *regPage = new QWidget(formStackedWidget);
    regPage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    QVBoxLayout *regPageLayout = new QVBoxLayout(regPage);
    regPageLayout->setContentsMargins(0, 4, 0, 0);
    regPageLayout->setSpacing(12);
    regPageLayout->setAlignment(Qt::AlignTop);

    QFormLayout *regForm = new QFormLayout();
    regForm->setContentsMargins(0, 0, 0, 0);
    regForm->setLabelAlignment(Qt::AlignLeft);
    regForm->setVerticalSpacing(12);
    regForm->setHorizontalSpacing(16);

    regEmailEdit = new QLineEdit(regPage);
    regEmailEdit->setPlaceholderText("Nhập địa chỉ email mới");
    regEmailEdit->setFixedHeight(52);

    regPasswordEdit = new QLineEdit(regPage);
    regPasswordEdit->setPlaceholderText("Tối thiểu 6 ký tự");
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regPasswordEdit->setFixedHeight(52);

    regConfirmPasswordEdit = new QLineEdit(regPage);
    regConfirmPasswordEdit->setPlaceholderText("Nhập lại mật khẩu");
    regConfirmPasswordEdit->setEchoMode(QLineEdit::Password);
    regConfirmPasswordEdit->setFixedHeight(52);

    regRoleCombo = new ChevronComboBox(regPage);
    regRoleCombo->setObjectName("regRoleCombo");

    QListView *roleListView = new QListView(regRoleCombo);
    roleListView->setObjectName("roleComboListView");

    QPalette pal = roleListView->palette();
    pal.setColor(QPalette::Base, Qt::white);
    pal.setColor(QPalette::Window, Qt::white);
    pal.setColor(QPalette::Text, QColor("#1E293B"));
    pal.setColor(QPalette::Highlight, QColor("#B95334"));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    roleListView->setPalette(pal);

    roleListView->setStyleSheet(
        "QListView#roleComboListView {"
        "   background-color: #FFFFFF;"
        "   color: #1E293B;"
        "   border: 2px solid #B95334;"
        "   border-radius: 12px;"
        "   padding: 6px;"
        "   outline: none;"
        "   selection-background-color: #F8EBE6;"
        "   selection-color: #B95334;"
        "}"
        "QListView#roleComboListView::item {"
        "   min-height: 48px;"
        "   padding: 10px 16px;"
        "   border-radius: 8px;"
        "   color: #1E293B;"
        "   background-color: #FFFFFF;"
        "   font-size: 20px;"
        "   font-weight: 600;"
        "}"
        "QListView#roleComboListView::item:hover {"
        "   background-color: #F8EBE6;"
        "   color: #B95334;"
        "}"
        "QListView#roleComboListView::item:selected {"
        "   background-color: #B95334;"
        "   color: #FFFFFF;"
        "   font-weight: 700;"
        "}"
    );
    regRoleCombo->setView(roleListView);

    regRoleCombo->addItem("Ứng viên tìm việc", "CANDIDATE");
    regRoleCombo->addItem("Doanh nghiệp", "COMPANY");
    regRoleCombo->setFixedHeight(52);
    regRoleCombo->setFixedWidth(248);

    regEmailLbl = new QLabel("Email:", regPage);
    regPwdLbl = new QLabel("Mật khẩu:", regPage);
    regConfirmLbl = new QLabel("Xác nhận MK:", regPage);
    regRoleLbl = new QLabel("Loại tài khoản:", regPage);

    regForm->addRow(regEmailLbl, regEmailEdit);
    regForm->addRow(regPwdLbl, regPasswordEdit);
    regForm->addRow(regConfirmLbl, regConfirmPasswordEdit);
    regForm->addRow(regRoleLbl, regRoleCombo);
    regPageLayout->addLayout(regForm);

    regStatusLabel = new QLabel(regPage);
    regStatusLabel->setStyleSheet("color: #DC2626; font-size: 16px; font-weight: 600; padding: 2px;");
    regStatusLabel->setAlignment(Qt::AlignCenter);
    regStatusLabel->setWordWrap(true);
    regStatusLabel->setVisible(false);
    regPageLayout->addWidget(regStatusLabel);

    regButton = new QPushButton("Đăng ký tài khoản", regPage);
    regButton->setObjectName("regButton");
    regButton->setCursor(Qt::PointingHandCursor);
    regButton->setFixedWidth(360);

    QHBoxLayout *regBtnLayout = new QHBoxLayout();
    regBtnLayout->setContentsMargins(0, 6, 0, 6);
    regBtnLayout->setAlignment(Qt::AlignCenter);
    regBtnLayout->addWidget(regButton);
    regPageLayout->addLayout(regBtnLayout);

    QHBoxLayout *switchLoginLayout = new QHBoxLayout();
    switchLoginLayout->setContentsMargins(0, 8, 0, 0);
    switchLoginLayout->setSpacing(8);
    switchLoginLayout->setAlignment(Qt::AlignCenter);
    hasAccountLabel = new QLabel("Đã có tài khoản?", regPage);
    switchToLoginBtn = new QPushButton("Đăng nhập", regPage);
    switchToLoginBtn->setObjectName("switchToLoginBtn");
    switchToLoginBtn->setCursor(Qt::PointingHandCursor);
    switchLoginLayout->addWidget(hasAccountLabel);
    switchLoginLayout->addWidget(switchToLoginBtn);
    regPageLayout->addLayout(switchLoginLayout);

    formStackedWidget->addWidget(regPage);

    cardLayout->addWidget(formStackedWidget);

    centerVLayout->addStretch(1);
    centerVLayout->addWidget(cardFrame, 0, Qt::AlignCenter);
    centerVLayout->addStretch(1);

    scrollArea->setWidget(scrollContent);
    rightRootLayout->addWidget(scrollArea);

    rootLayout->addWidget(rightContainer, 0, Qt::AlignRight);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(switchToRegBtn, &QPushButton::clicked, this, &LoginWindow::onSwitchToRegister);
    connect(regButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(switchToLoginBtn, &QPushButton::clicked, this, &LoginWindow::onSwitchToLogin);
    connect(loginPasswordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
    connect(loginEmailEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
    connect(regConfirmPasswordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onRegisterClicked);
}

void LoginWindow::applyTheme(ThemeStyle style) {
    m_currentTheme = style;

    QString illPath = "avatars/login_bg.png";

    if (!QFile::exists(illPath)) {
        illPath = QDir(StyleHelper::getProjectRootDir()).filePath(illPath);
    }
    if (!QFile::exists(illPath)) {
        illPath = "avatars/login_illustration.png";
        if (!QFile::exists(illPath)) {
            illPath = QDir(StyleHelper::getProjectRootDir()).filePath("avatars/login_illustration.png");
        }
    }
    if (QFile::exists(illPath)) {
        m_illustrationPixmap.load(illPath);
    }

    const QString cardStyle =
        "#cardFrame {"
        "   background-color: rgba(255, 255, 255, 0.95);"
        "   border: 1.5px solid rgba(220, 205, 192, 0.90);"
        "   border-radius: 24px;"
        "   padding: 32px 30px;"
        "}";

    if (cardFrame) cardFrame->setStyleSheet(cardStyle);

    if (cardShadow) {
        cardShadow->setColor(QColor(80, 50, 30, 80));
        cardShadow->setBlurRadius(42);
        cardShadow->setOffset(0, 10);
    }

    if (logoLabel) {
        logoLabel->setStyleSheet("font-size: 52px; font-weight: 800; color: #B95334; letter-spacing: 0.5px;");
    }

    if (subtitleLabel) {
        subtitleLabel->setStyleSheet("font-size: 21px; font-weight: 600; color: #5A626A; margin-top: 4px;");
    }

    const QString lblStyle = "font-size: 22px; font-weight: 700; color: #1E293B;";
    if (emailLabel) emailLabel->setStyleSheet(lblStyle);
    if (pwdLabel) pwdLabel->setStyleSheet(lblStyle);
    if (regEmailLbl) regEmailLbl->setStyleSheet(lblStyle);
    if (regPwdLbl) regPwdLbl->setStyleSheet(lblStyle);
    if (regConfirmLbl) regConfirmLbl->setStyleSheet(lblStyle);
    if (regRoleLbl) regRoleLbl->setStyleSheet(lblStyle);

    const QString inputStyle =
        "QLineEdit {"
        "   background-color: #FFFFFF;"
        "   border: 2px solid #C4B9AD;"
        "   border-radius: 11px;"
        "   padding: 0px 18px;"
        "   font-size: 21px;"
        "   color: #1E1E1E;"
        "}"
        "QLineEdit:focus {"
        "   border: 2.5px solid #B95334;"
        "}";

    const QString comboStyle =
        "QComboBox {"
        "   background-color: #FFFFFF;"
        "   border: 2px solid #C4B9AD;"
        "   border-radius: 11px;"
        "   padding-left: 14px;"
        "   padding-right: 32px;"
        "   font-size: 21px;"
        "   color: #1E1E1E;"
        "}"
        "QComboBox:hover {"
        "   border: 2px solid #B95334;"
        "}"
        "QComboBox:focus {"
        "   border: 2.5px solid #B95334;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: center right;"
        "   width: 32px;"
        "   border: none;"
        "   background: transparent;"
        "}"
        "QComboBox::down-arrow {"
        "   image: none;"
        "   width: 0px;"
        "   height: 0px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #FFFFFF;"
        "   color: #1E293B;"
        "   border: 2px solid #B95334;"
        "   border-radius: 12px;"
        "   padding: 6px;"
        "   outline: none;"
        "   selection-background-color: #F8EBE6;"
        "   selection-color: #B95334;"
        "   font-size: 20px;"
        "   font-weight: 600;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "   min-height: 48px;"
        "   padding: 10px 16px;"
        "   border-radius: 8px;"
        "   color: #1E293B;"
        "   background-color: #FFFFFF;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "   background-color: #F8EBE6;"
        "   color: #B95334;"
        "}"
        "QComboBox QAbstractItemView::item:selected {"
        "   background-color: #B95334;"
        "   color: #FFFFFF;"
        "   font-weight: 700;"
        "}";

    const QString buttonStyle =
        "QPushButton {"
        "   background-color: #B95334;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 11px;"
        "   font-size: 22px;"
        "   font-weight: 800;"
        "   min-height: 54px;"
        "   max-height: 54px;"
        "   padding: 0px 28px;"
        "}"
        "QPushButton:hover { background-color: #A3462A; }"
        "QPushButton:pressed { background-color: #8C3920; }";

    if (loginEmailEdit) loginEmailEdit->setStyleSheet(inputStyle);
    if (loginPasswordEdit) loginPasswordEdit->setStyleSheet(inputStyle);
    if (regEmailEdit) regEmailEdit->setStyleSheet(inputStyle);
    if (regPasswordEdit) regPasswordEdit->setStyleSheet(inputStyle);
    if (regConfirmPasswordEdit) regConfirmPasswordEdit->setStyleSheet(inputStyle);
    if (regRoleCombo) regRoleCombo->setStyleSheet(comboStyle);

    if (loginButton) loginButton->setStyleSheet(buttonStyle);
    if (regButton) regButton->setStyleSheet(buttonStyle);

    if (noAccountLabel) noAccountLabel->setStyleSheet("color: #5A626A; font-size: 20px;");
    if (switchToRegBtn) switchToRegBtn->setStyleSheet("background: transparent; border: none; color: #B95334; font-weight: 700; font-size: 20px;");
    if (hasAccountLabel) hasAccountLabel->setStyleSheet("color: #5A626A; font-size: 20px;");
    if (switchToLoginBtn) switchToLoginBtn->setStyleSheet("background: transparent; border: none; color: #B95334; font-weight: 700; font-size: 20px;");

    update();
}

void LoginWindow::updateIllustration() {
    update();
}

void LoginWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const int w = width();
    const int h = height();

    QLinearGradient bgGrad(0, 0, w, 0);
    bgGrad.setColorAt(0.00, QColor("#F8F4EB"));
    bgGrad.setColorAt(0.40, QColor("#F8F4EB"));
    bgGrad.setColorAt(0.70, QColor("#FAF4EB"));
    bgGrad.setColorAt(1.00, QColor("#FFFFFF"));
    painter.fillRect(rect(), bgGrad);

    if (!m_illustrationPixmap.isNull()) {
        int illH = qMax(h, int(h * 1.14));
        int illW = (m_illustrationPixmap.width() * illH) / m_illustrationPixmap.height();
        int illY = (h - illH) / 2;
        int illX = 0;

        painter.drawPixmap(illX, illY, illW, illH, m_illustrationPixmap);
    }

    const qreal midX = w - 550;
    QPainterPath wave1;
    wave1.moveTo(midX - 80, 0);
    wave1.cubicTo(midX + 90, h * 0.32, midX - 60, h * 0.68, midX + 80, h);
    wave1.lineTo(w, h);
    wave1.lineTo(w, 0);
    wave1.closeSubpath();

    QLinearGradient waveGrad1(midX - 80, 0, w, 0);
    waveGrad1.setColorAt(0.00, QColor(255, 255, 255, 90));
    waveGrad1.setColorAt(0.35, QColor(255, 255, 255, 185));
    waveGrad1.setColorAt(1.00, QColor(255, 255, 255, 245));
    painter.fillPath(wave1, waveGrad1);

    QPainterPath wave2;
    wave2.moveTo(midX, 0);
    wave2.cubicTo(midX + 75, h * 0.36, midX - 30, h * 0.65, midX + 50, h);
    wave2.lineTo(w, h);
    wave2.lineTo(w, 0);
    wave2.closeSubpath();

    QLinearGradient waveGrad2(midX, 0, w, 0);
    waveGrad2.setColorAt(0.00, QColor(255, 255, 255, 125));
    waveGrad2.setColorAt(0.30, QColor(255, 255, 255, 225));
    waveGrad2.setColorAt(1.00, QColor(255, 255, 255, 255));
    painter.fillPath(wave2, waveGrad2);

    QPainterPath contour;
    contour.moveTo(midX, 0);
    contour.cubicTo(midX + 75, h * 0.36, midX - 30, h * 0.65, midX + 50, h);
    QPen contourPen(QColor(185, 83, 52, 45));
    contourPen.setWidthF(1.5);
    painter.strokePath(contour, contourPen);

    QRadialGradient glow(midX + 20, h * 0.35, 280);
    glow.setColorAt(0.0, QColor(245, 220, 205, 60));
    glow.setColorAt(1.0, QColor(245, 220, 205, 0));
    painter.fillRect(rect(), glow);

    const QColor nodeColor(185, 83, 52, 60);
    QPen netPen(QColor(185, 83, 52, 35), 1.2, Qt::DashLine);
    painter.setPen(netPen);

    QPointF p1(midX - 25, h * 0.16);
    QPointF p2(midX + 35, h * 0.26);
    QPointF p3(midX - 5, h * 0.40);
    QPointF p4(midX + 45, h * 0.70);
    QPointF p5(midX - 15, h * 0.82);

    painter.drawLine(p1, p2);
    painter.drawLine(p2, p3);
    painter.drawLine(p3, p4);
    painter.drawLine(p4, p5);

    painter.setPen(Qt::NoPen);
    auto drawNode = [&](const QPointF &pt, qreal r, const QColor &c) {
        painter.setBrush(QColor(255, 255, 255, 220));
        painter.drawEllipse(pt, r, r);
        painter.setBrush(c);
        painter.drawEllipse(pt, r * 0.5, r * 0.5);
    };

    drawNode(p1, 5.0, nodeColor);
    drawNode(p2, 6.5, QColor(185, 83, 52, 90));
    drawNode(p3, 4.5, nodeColor);
    drawNode(p4, 6.0, QColor(70, 95, 120, 80));
    drawNode(p5, 5.0, nodeColor);
}

void LoginWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateIllustration();
}

void LoginWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F11) {
        if (isFullScreen()) {
            showMaximized();
        } else {
            showFullScreen();
        }
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        showMaximized();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void LoginWindow::onSwitchToRegister() {
    loginStatusLabel->setVisible(false);
    formStackedWidget->setCurrentIndex(1);
    formStackedWidget->updateGeometry();
    if (cardFrame) {
        cardFrame->updateGeometry();
        cardFrame->adjustSize();
    }
    setWindowTitle("JobMatch — Đăng ký tài khoản");
}

void LoginWindow::onSwitchToLogin() {
    regStatusLabel->setVisible(false);
    formStackedWidget->setCurrentIndex(0);
    formStackedWidget->updateGeometry();
    if (cardFrame) {
        cardFrame->updateGeometry();
        cardFrame->adjustSize();
    }
    setWindowTitle("JobMatch — Đăng nhập");
}

void LoginWindow::onLoginClicked() {
    std::string email = loginEmailEdit->text().trimmed().toStdString();
    std::string password = loginPasswordEdit->text().toStdString();

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
    if (loginPasswordEdit) loginPasswordEdit->clear();
    if (loginStatusLabel) loginStatusLabel->setVisible(false);
    if (regEmailEdit) regEmailEdit->clear();
    if (regPasswordEdit) regPasswordEdit->clear();
    if (regConfirmPasswordEdit) regConfirmPasswordEdit->clear();
    if (regRoleCombo) regRoleCombo->setCurrentIndex(0);
    if (regStatusLabel) regStatusLabel->setVisible(false);
}
