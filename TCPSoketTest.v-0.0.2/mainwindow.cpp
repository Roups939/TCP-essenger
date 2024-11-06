#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "message.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new QTcpSocket(this);
    server = new QTcpServer(this);

    // connect(sender, signal, receiver, function);
    connect(client, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(client, SIGNAL(bytesWritten(qint64)), this, SLOT(on_bytesWritten(qint64)));

    connect(server, SIGNAL(newConnection()), this, SLOT(incommingConnection()));





}

MainWindow::~MainWindow()
{
    delete client;
    delete server;
    delete ui;

}

QString MainWindow::getLocalIp()
{
    // Getting local IP from all IP
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
        {
             return address.toString();
        }
    }
}

void MainWindow::on_connectButton_clicked()
{
    QString host = ui->host_field->text();
    int port = ui->port_field->value();
    client->connectToHost(host, port);

    if(!client->waitForConnected(5000))
    {
        qDebug() << "Error: " << client->errorString();
    }
}

void MainWindow::on_sendButton_clicked()
{
    if(client->state() == client->ConnectedState)
    {
        qDebug() << "Connected!";

        QString line = ui->message_field->text();
        QString key = "01234567890123456789012345678901"; // 32 байта для AES-256

        // Шифрование сообщения
        QString encryptedMessage = encrypt(line, key);
        QByteArray data = encryptedMessage.toUtf8();

        client->write(data);
    }
    else
    {
        qDebug() << "Disconnected!";
    }
}

void MainWindow::on_connected()
{

    ui -> status_label -> setText("Status: Connected!");
    ui -> sendButton -> setEnabled(true);
}

void MainWindow::on_disconnected()
{

    ui -> status_label -> setText("Status: Disconnected!");
}

void MainWindow::on_bytesWritten(qint64 bytes)
{

    if (bytes == -1)
    {
        qDebug() << "No data was sent";
    }
    else
    {
        ui -> message_status -> setText("Message sent successfully");
    }

}

void MainWindow::incommingConnection()
{
    QTcpSocket * socket = server->nextPendingConnection(); // получаем сокет нового входящего подключения
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState))); // делаем обработчик изменения статуса сокета
    connect(socket, SIGNAL(readyRead()), this, SLOT(on_readyRead())); // подключаем входящие сообщения от вещающего на наш обработчик
    qDebug() << "this one is server";
}

void MainWindow::on_readyRead()
{
    QObject *object = QObject::sender();
    if (!object) {
        return;
    }
    qDebug() << "[1]";
    QTcpSocket *socket = static_cast<QTcpSocket *>(object);
    QByteArray arr = socket->readAll();

    QString key = "01234567890123456789012345678901"; // 32 байта для AES-256
    QString decryptedMessage = decrypt(QString(arr), key);

    ui->chat_window->append(decryptedMessage);
}

void MainWindow::on_startButton_clicked()
{
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

    int port = ui->port_field_2->value();
    server->listen(QHostAddress::Any, port);
}

void MainWindow::on_stopButton_clicked()
{
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    server->disconnect();
}

void MainWindow::on_disconnect_Buton_clicked()
{
    client->disconnectFromHost();
    //ui -> sendButton -> setEnabled(false);
}



