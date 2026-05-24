#include "index.h"
#include <QDebug>

Index::Index(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("登录界面");
    this->setFixedSize(400, 300);

    nameLabel = new QLabel("姓名：", this);
    idLabel = new QLabel("ID：", this);
    nameEdit = new QLineEdit(this);
    idEdit = new QLineEdit(this);
    loginBtn = new QPushButton("登录", this);
    exitBtn = new QPushButton("退出", this);

    nameLabel->setGeometry(80, 60, 60, 30);
    nameEdit->setGeometry(140, 60, 180, 30);
    idLabel->setGeometry(80, 110, 60, 30);
    idEdit->setGeometry(140, 110, 180, 30);
    loginBtn->setGeometry(100, 180, 80, 35);
    exitBtn->setGeometry(220, 180, 80, 35);

    connect(loginBtn, &QPushButton::clicked, this, &Index::onLoginClicked);
    connect(exitBtn, &QPushButton::clicked, this, &Index::close);
}

void Index::onLoginClicked()
{
    QString name = nameEdit->text().trimmed();
    QString id = idEdit->text().trimmed();
    if (name.isEmpty() || id.isEmpty()) {
        QMessageBox::warning(this, "提示", "姓名和ID不能为空！");
        return;
    }
    emit loginSuccess(name, id);
    this->close();
}

// 退出按钮点击槽函数：关闭登录窗口（实际已通过connect绑定到close，此函数为冗余定义）
void Index::onExitClicked()
{
    this->close();
}
