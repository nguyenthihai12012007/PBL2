#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QWidget>
#include "class.h"

class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QStackedWidget;

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(Data &data, QWidget *parent = nullptr);
    ~LoginWindow() override = default;

signals:
    void loginSuccess(int userId, const QString &email, const QString &role);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onSwitchToRegister();
    void onSwitchToLogin();

private:
    void setupUI();
    void clearInputs();

    Data &m_data;

    // Login Form widgets
    QLineEdit *loginEmailEdit;
    QLineEdit *loginPasswordEdit;
    QLabel *loginStatusLabel;
    QPushButton *loginButton;

    // Register Form widgets
    QLineEdit *regEmailEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *regConfirmPasswordEdit;
    QComboBox *regRoleCombo;
    QLabel *regStatusLabel;
    QPushButton *regButton;

    // Container stacked widget for smooth switching
    QStackedWidget *formStackedWidget;
};

#endif // LOGIN_WINDOW_H
