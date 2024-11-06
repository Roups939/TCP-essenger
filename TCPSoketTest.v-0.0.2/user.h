#ifndef USER_H
#define USER_H
#include <QString>


class User
{
public:
    void Create(int eid,QString eusername, bool estatus);
    int GetId();
    QString GetUsername();
    void SetUsername(QString eusername);
    bool GetStatus();
    void SetStatus(bool estatus);

private:
    int id;
    bool status;//вместо онлайн
    QString username;
};

#endif // USER_H
