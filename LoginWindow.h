#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QWidget>
#include <QPixmap>
#include "class.h"

class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QStackedWidget;
class QFrame;
class QGraphicsDropShadowEffect;
class QResizeEvent;
class QKeyEvent;

enum class ThemeStyle {
    HiringWorkspace = 3        // Mẫu 3: Bàn làm việc tuyển dụng & Laptop (Giao diện chính thức duy nhất)
};

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(Data &data, QWidget *parent = nullptr);
    ~LoginWindow() override = default;

    void applyTheme(ThemeStyle style = ThemeStyle::HiringWorkspace);
    ThemeStyle currentTheme() const { return m_currentTheme; }

signals:
    void loginSuccess(int userId, const QString &email, const QString &role);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onSwitchToRegister();
    void onSwitchToLogin();

private:
    void setupUI();
    void clearInputs();
    void updateIllustration();

    Data &m_data;
    ThemeStyle m_currentTheme;

    // Illustration
    QLabel *illustrationLabel;
    QPixmap m_illustrationPixmap;

    // Card frame & effect
    QWidget *rightContainer;
    QFrame *cardFrame;
    QGraphicsDropShadowEffect *cardShadow;

    // Branding labels
    QLabel *logoLabel;
    QLabel *subtitleLabel;

    // Login Form widgets
    QLabel *emailLabel;
    QLabel *pwdLabel;
    QLineEdit *loginEmailEdit;
    QLineEdit *loginPasswordEdit;
    QLabel *loginStatusLabel;
    QPushButton *loginButton;
    QLabel *noAccountLabel;
    QPushButton *switchToRegBtn;

    // Register Form widgets
    QLabel *regEmailLbl;
    QLabel *regPwdLbl;
    QLabel *regConfirmLbl;
    QLabel *regRoleLbl;
    QLineEdit *regEmailEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *regConfirmPasswordEdit;
    QComboBox *regRoleCombo;
    QLabel *regStatusLabel;
    QPushButton *regButton;
    QLabel *hasAccountLabel;
    QPushButton *switchToLoginBtn;

    // Container stacked widget for smooth switching
    QStackedWidget *formStackedWidget;
};

#endif // LOGIN_WINDOW_H
