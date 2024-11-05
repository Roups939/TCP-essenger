#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "message.h"
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

    // Hey server, tell me about you.
    QString line = ui->message_field->text();
    QByteArray data = line.toUtf8();

    // Задание №1: организовать проверку установленного соединения
    // (если соединение уже установлено, то выполнить отправку данных и вывести хорошее сообщение qDebug-ом)
    // (если соединение не было установлено, то данные не отправлять и поругаться qDebug-ом)
    // либо заблокировать кнопку "Send" и включать её только при наличии соединения
    client->write(data);
    }
    else
    {
        qDebug() << "Disconnected!";
    }


}

void MainWindow::on_connected()
{
    // Задание №2:
    // описать эту функцию, чтобы пользователь видел, что соеденение с кем-то установлено
    ui -> status_label -> setText("Status: Connected!");
    ui -> sendButton -> setEnabled(true);
}

void MainWindow::on_disconnected()
{
    // Задание №3:
    // описать эту функцию, чтобы пользователь видел, что соеденение с кем-то разовано
    ui -> status_label -> setText("Status: Disconnected!");
}

void MainWindow::on_bytesWritten(qint64 bytes)
{
    // Задание №4:
    // описать эту функцию, чтобы пользователь видел, что данные успешно отправлены
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
    QObject * object = QObject::sender(); // далее и ниже до цикла идет преобразования "отправителя сигнала" в сокет, дабы извлечь данные
    if (!object) {
        return;
    }
    qDebug() << "[1]";
    QTcpSocket * socket = static_cast<QTcpSocket *>(object);
    QByteArray arr =  socket->readAll();
    QString line = QString(arr);
    ui->chat_window->append(line);
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
