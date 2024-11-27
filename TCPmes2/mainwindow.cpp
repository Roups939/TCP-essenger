#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "message.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QHostAddress>
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new QTcpSocket(this);
    server = new QTcpServer(this);
    connect(client, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(client, SIGNAL(bytesWritten(qint64)), this, SLOT(on_bytesWritten(qint64)));
    connect(server, SIGNAL(newConnection()), this, SLOT(incommingConnection()));
    loadNickname();
    loadIP();
}

MainWindow::~MainWindow()
{
    delete client;
    delete server;
    delete ui;
}

QString MainWindow::getLocalIp()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
        {
             return address.toString();
        }
    }
    return QString();
}

void MainWindow::setupDatabase(const QString &ip) {
    QString dbName = ip + ".db";
    db = QSqlDatabase::addDatabase("QSQLITE", "connection_" + ip);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY AUTOINCREMENT, content TEXT)");
}

void MainWindow::saveMessage(const QString &ip, const QString &message) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO messages (content) VALUES (:content)");
    query.bindValue(":content", message);
    query.exec();
}

QStringList MainWindow::loadMessageHistory(const QString &ip) {
    QStringList messages;
    QSqlQuery query(db);
    query.exec("SELECT content FROM messages");
    while (query.next()) {
        messages << query.value(0).toString();
    }
    return messages;
}

void MainWindow::on_connectButton_clicked() {

    ui->chat_window->setText("");
    QString host = ui->host_field->text();
    int port = ui->port_field->value();
    client->connectToHost(host, port);

    if(!client->waitForConnected(5000))
    {
        ui->message_status->setText("Error: ");
        qDebug() << "Error: " << client->errorString();
    }
    QString IP = ui->host_field->text();
    saveIP(IP);
    ui->message_status->setText("Ip saved");


}

void MainWindow::on_sendButton_clicked() {
    if(client->state() == client->ConnectedState) {
        QString line = ui->message_field->text();
        QString line1 = ui->lineEdit->text();
        line = line1 + ": " + line;
        ui->chat_window->append(line);
        saveMessage(client->peerAddress().toString(), line);
        QByteArray data = line.toUtf8();
        client->write(data);
        ui->message_field->setText("");
    } else {
        ui->message_status->setText("Disconnected!");
        qDebug() << "Disconnected!";
    }
}

void MainWindow::on_connected() {
    QString ip = client->peerAddress().toString();
    setupDatabase(ip);

    QStringList messageHistory = loadMessageHistory(ip);
    for (const QString &msg : messageHistory) {
        ui->chat_window->append(msg);
    }

    ui->status_label->setText("Status: Connected!");
    ui->sendButton->setEnabled(true);
}

void MainWindow::on_disconnected() {
    ui->status_label->setText("Status: Disconnected!");
}

void MainWindow::on_bytesWritten(qint64 bytes) {
    if (bytes == -1) {
        ui->message_status->setText("No data was sent");
        qDebug() << "No data was sent";
    } else {
        ui->message_status->setText("Message sent successfully");
    }
}

void MainWindow::incommingConnection() {
    QTcpSocket *socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    ui->message_status->setText("Client connected");
    qDebug() << "Client connected";
}

void MainWindow::on_readyRead() {
    QObject *object = QObject::sender();
    if (!object) {
        return;
    }
    QTcpSocket *socket = static_cast<QTcpSocket *>(object);
    QByteArray arr = socket->readAll();
    QString line = QString(arr);
    ui->chat_window->append(line);
    saveMessage(client->peerAddress().toString(), line);
}

void MainWindow::on_startButton_clicked()
{
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

    int port = ui->port_field_2->value();
    if (server->listen(QHostAddress::Any, port)) {
        ui->message_status->setText("Server started on port: " + QString::number(port));
    } else {
        ui->message_status->setText("Server failed to start");
    }
}

void MainWindow::on_stopButton_clicked()
{
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    server->close();
    ui->message_status->setText("Server stopped");
}

void MainWindow::on_disconnect_Buton_clicked()
{
    client->disconnectFromHost();
    ui->sendButton->setEnabled(false);
}

void MainWindow::on_pushButton_clicked()
{
    QString ip = getLocalIp();

        QString dbName = ip + ".db";

        if (db.isOpen()) {
            db.close();
        }

        if (QFile::remove(dbName)) {
            ui->message_status->setText("database deleted: " + dbName);
            qDebug() << "database deleted:" << dbName;
        } else {
            ui->message_status->setText("errore database deleted: " + dbName);
            qDebug() << "errore database deleted:" << dbName;
        }
}

void MainWindow::saveNickname(const QString &nickname) {
    QFile file("nickname.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << nickname;
        file.close();
    }
}

void MainWindow::loadNickname() {
    QFile file("nickname.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString nickname = in.readLine();
        ui->lineEdit->setText(nickname);
        file.close();
    }
}
void MainWindow::on_lineEdit_windowIconTextChanged(const QString &iconText)
{

}

void MainWindow::on_pushButton_2_clicked()
{
    QString nickname = ui->lineEdit->text();
    saveNickname(nickname);
    ui->message_status->setText("nickname saved");
}
void MainWindow::saveIP(const QString &IP) {
    QFile file("IP_S.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << IP;
        file.close();
    }
}

void MainWindow::loadIP() {
    QFile file("IP_S.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString IP = in.readLine();
        ui->host_field->setText(IP);
        file.close();
    }
}
