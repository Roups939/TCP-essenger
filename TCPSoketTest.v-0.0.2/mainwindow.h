#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <openssl/evp.h>
#include <openssl/rand.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString encrypt(const QString &plaintext, const QString &key) {
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        QByteArray ciphertext(plaintext.length() + 16, 0); // Allocate enough space for padding
        int len;

        unsigned char iv[16]; // Initialization vector
        RAND_bytes(iv, sizeof(iv)); // Generate a random IV
        memcpy(ciphertext.data(), iv, sizeof(iv)); // Store IV at the beginning

        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.toUtf8().data(), iv);
        EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(ciphertext.data() + sizeof(iv)), &len,
                          (unsigned char *)plaintext.toUtf8().data(), plaintext.length());
        int ciphertext_len = len;

        EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(ciphertext.data() + sizeof(iv) + len), &len);
        ciphertext_len += len;

        EVP_CIPHER_CTX_free(ctx);
        return QString::fromUtf8(ciphertext.constData(), ciphertext_len + sizeof(iv));
    }

    QString decrypt(const QString &ciphertext, const QString &key) {
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        QByteArray decrypted(ciphertext.length(), 0);
        int len;

        const unsigned char *data = reinterpret_cast<const unsigned char *>(ciphertext.toUtf8().data());
        unsigned char iv[16];
        memcpy(iv, data, sizeof(iv)); // Extract IV from the beginning

        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.toUtf8().data(), iv);
        EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(decrypted.data()), &len,
                          data + sizeof(iv), ciphertext.length() - sizeof(iv));
        int decrypted_len = len;

        EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(decrypted.data() + len), &len);
        decrypted_len += len;

        EVP_CIPHER_CTX_free(ctx);
        return QString::fromUtf8(decrypted.constData(), decrypted_len);
    }


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

    void on_message_field_cursorPositionChanged(int arg1, int arg2);

private:
    Ui::MainWindow *ui;
    QTcpSocket *client;


    QTcpServer *server;
    QTcpSocket *connectedUser;
};
#endif // MAINWINDOW_H
