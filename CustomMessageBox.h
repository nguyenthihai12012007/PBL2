#ifndef CUSTOM_MESSAGE_BOX_H
#define CUSTOM_MESSAGE_BOX_H

#include <QDialog>
#include <QString>

class CustomMessageBox : public QDialog {
    Q_OBJECT

public:
    enum IconType {
        Information,
        Success,
        Warning,
        Critical,
        Question
    };

    explicit CustomMessageBox(QWidget *parent = nullptr,
                              const QString &title = "",
                              const QString &message = "",
                              IconType icon = Information,
                              bool isQuestion = false,
                              const QString &yesText = "Đồng ý",
                              const QString &noText = "Hủy bỏ");
    ~CustomMessageBox() override = default;

    static void information(QWidget *parent, const QString &title, const QString &message);
    static void success(QWidget *parent, const QString &title, const QString &message);
    static void warning(QWidget *parent, const QString &title, const QString &message);
    static void critical(QWidget *parent, const QString &title, const QString &message);
    static bool question(QWidget *parent, const QString &title, const QString &message,
                         const QString &yesText = "Đồng ý",
                         const QString &noText = "Hủy bỏ");

private:
    void setupUI(const QString &title,
                 const QString &message,
                 IconType icon,
                 bool isQuestion,
                 const QString &yesText,
                 const QString &noText);
};

#endif // CUSTOM_MESSAGE_BOX_H
