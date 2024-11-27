#include "user.h"



void User::Create(int eid,QString eusername, bool estatus){
    id=eid;
    username=eusername;
    status=estatus;
}

int User::GetId(){
    return id;
}

QString User::GetUsername(){
    return username;
}

void User::SetUsername(QString eusername){
    username=eusername;
    return;
}

bool User::GetStatus(){
    return status;
}

void User::SetStatus(bool estatus){
    status=estatus;
    return;
}
