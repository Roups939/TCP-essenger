#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // App slots
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    QString getLocalIp();

    // Client slots
    void on_connected(); // установка соединения
    void on_disconnected(); // разрыв соединения
    void on_bytesWritten(qint64 bytes); // отправка данных

    // Server slots
    void incommingConnection();
    void on_readyRead(); // получение данных

    void on_disconnect_Buton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *client;
    QTcpServer *server;
    QTcpSocket *connectedUser;
    void setupDatabase(const QString &ip);
        void saveMessage(const QString &ip, const QString &message);
        QStringList loadMessageHistory(const QString &ip);
        QSqlDatabase db; // Объявление базы данных
};
#endif // MAINWINDOW_H
