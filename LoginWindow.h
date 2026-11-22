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
    HiringWorkspace = 3       
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

    QLabel *illustrationLabel;
    QPixmap m_illustrationPixmap;

    QWidget *rightContainer;
    QFrame *cardFrame;
    QGraphicsDropShadowEffect *cardShadow;

    QLabel *logoLabel;
    QLabel *subtitleLabel;

    QLabel *emailLabel;
    QLabel *pwdLabel;
    QLineEdit *loginEmailEdit;
    QLineEdit *loginPasswordEdit;
    QLabel *loginStatusLabel;
    QPushButton *loginButton;
    QLabel *noAccountLabel;
    QPushButton *switchToRegBtn;

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

    QStackedWidget *formStackedWidget;
};

#endif 
