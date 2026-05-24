#ifndef INDEX_H
#define INDEX_H

#include <QWidget>      // Qt基础窗口类
#include <QPushButton>  // 按钮控件
#include <QLineEdit>    // 输入框控件
#include <QLabel>       // 标签控件
#include <QMessageBox>  // 消息框控件

// 登录窗口类：负责用户登录、用户信息校验、用户入库
class Index : public QWidget {
    Q_OBJECT // Qt信号槽机制必需的宏
public:
    // 构造函数：parent为父窗口（默认空）
    explicit Index(QWidget *parent = nullptr);

signals:
    // 登录成功信号：传递用户名和用户ID给游戏窗口
    void loginSuccess(const QString &username, const QString &userid);

private slots:
    // 登录按钮点击槽函数：处理登录逻辑
    void onLoginClicked();
    // 退出按钮点击槽函数：关闭登录窗口
    void onExitClicked();

private:
    // 界面控件声明
    QLabel *nameLabel;   // 姓名标签
    QLabel *idLabel;     // ID标签
    QLineEdit *nameEdit; // 姓名输入框
    QLineEdit *idEdit;   // ID输入框
    QPushButton *loginBtn; // 登录按钮
    QPushButton *exitBtn;  // 退出按钮


};

#endif
