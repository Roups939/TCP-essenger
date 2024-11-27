#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtSql>
#include <QSet>

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
    void on_connected();
    void on_disconnected();
    void on_bytesWritten(qint64 bytes);

    // Server slots
    void incommingConnection();
    void on_readyRead();

    void on_disconnect_Buton_clicked();

    void on_label_9_linkActivated(const QString &link);

    void on_lineEdit_windowIconTextChanged(const QString &iconText);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *client;
    QTcpServer *server;
    QTcpSocket *connectedUser;
    void setupDatabase(const QString &ip);
        void saveMessage(const QString &ip, const QString &message);
        QStringList loadMessageHistory(const QString &ip);
        QSqlDatabase db;
};
#endif // MAINWINDOW_H
