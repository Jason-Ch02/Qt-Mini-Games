#include "widget.h"
#include "index.h"
#include "leaderboardclient.h"
#include <QApplication>
#include <QIcon>
#include <QCoreApplication>
#include <QFile>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString appDir = QCoreApplication::applicationDirPath();
    QString iconPath = appDir + "/head.png";
    if (QFile::exists(iconPath))
        a.setWindowIcon(QIcon(iconPath));

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    LeaderboardClient::setServerUrl("https://qt-mini-games-production.up.railway.app");

    Index *loginWindow = new Index();
    Widget *gameWindow = new Widget();
    gameWindow->hide();

    QObject::connect(loginWindow, &Index::loginSuccess,
                     [&](const QString &username, const QString &userid) {
                         gameWindow->setCurrentUser(username, userid);
                         gameWindow->show();
                         loginWindow->close();
                     });

    loginWindow->show();

    return a.exec();
}
