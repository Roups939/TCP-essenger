#ifndef MESSAGE_H
#define MESSAGE_H
#include "mes.h"
#include "QDateTime"
class message
{
public:
    message();
    int getid();
    mes sender;
    QString Getcontent();
    QDateTime GetdateTime();
    void setid(int eid);
    void setcontent();


    private:
    int id;
    QString content;
    QDateTime dateTime;
};


#endif // MESSAGE_H
