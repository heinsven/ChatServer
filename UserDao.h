#ifndef CHAT_USERDAO_H
#define CHAT_USERDAO_H


#include "User.h"
#include "MyDB.h"
#include "Model.h"

using namespace zlb_mysql;

class UserDao {
public:
    UserDao() {
        dataBase = new CDataBase();
        dataBase->Connect("localhost", "root", "root", "chat");
        recordSet = new CRecordSet();
        recordSet->GetRecordSet(dataBase->Get_Mysql());
    }

    ~UserDao() {
        delete dataBase;
        delete recordSet;
    }

    USER::User* getUser(std::string username);

    USER::User* getUser(std::string username, std::string password);

    std::list<MODEL::Relationship*> getUserFriends(USER::User* user);

    int addRelationship(std::string username,std::string friend_username);

    int rmRelationship(std::string username,std::string friend_username);

    int editRemark(string &username, string &friend_username, string &remark);


private:
    CDataBase *dataBase;

    CRecordSet *recordSet;

};


#endif //CHAT_USERDAO_H
