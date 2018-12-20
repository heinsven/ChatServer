#include "UserDao.h"
#include "Model.h"

USER::User *UserDao::getUser(std::string username) {
    string sql = "select * from t_user where username = '" + username + "'";
    cout << sql << endl;
    USER::User *user = nullptr;
    int result = recordSet->ExecuteSQL(sql.c_str());
    if (result == 1) {
        CRecord record = (*recordSet)[0];
        user = new USER::User(record["username"], record["password"], record["nickname"], record["status"]);
    }

    return user;
}

USER::User *UserDao::getUser(std::string username, std::string password) {
    string sql = "select * from t_user where username = '" + username + "' and password = '" + password + "'";
    cout << sql << endl;
    USER::User *user = nullptr;
    int result = recordSet->ExecuteSQL(sql.c_str());
    if (result == 1) {
        CRecord record = (*recordSet)[0];
        user = new USER::User(record["username"], record["password"], record["nickname"], record["status"]);
    }

    return user;
}

std::list<MODEL::Relationship *> UserDao::getUserFriends(USER::User *user) {
    string sql = "select * from t_relationship where username = '";
    sql.append(user->getUsername() + "'");
    cout << sql << endl;
    list<MODEL::Relationship *> relationships;
    int result = recordSet->ExecuteSQL(sql.c_str());
    for (int i = 0; i < result; i++) {
        CRecord record = (*recordSet)[i];
        MODEL::Relationship *t_relationship = nullptr;
        t_relationship = new MODEL::Relationship(record["username"], record["friend_username"], record["remark_name"]);
        relationships.push_back(t_relationship);
    }
    return relationships;
}

int UserDao::addRelationship(std::string username, std::string friend_username) {
    string sql = "insert into t_relationship values('";
    sql.append(username + "','");
    sql.append(friend_username + "','')");
    cout << sql << endl;

    int result = dataBase->ExecQuery(sql);

    return result;
}

int UserDao::rmRelationship(std::string username, std::string friend_username) {
    string sql = "delete from t_relationship where username = '";
    sql.append(username + "' and friend_username = '");
    sql.append(friend_username + "'");
    cout << sql << endl;

    int result = dataBase->ExecQuery(sql);

    return result;
}

int UserDao::editRemark(string &username, string &friend_username, string &remark) {
    string sql = "update t_relationship set remark_name = '";
    sql.append(remark + "' where username = '");
    sql.append(username + "' and friend_username = '");
    sql.append(friend_username + "'");
    cout << sql << endl;

    int result = dataBase->ExecQuery(sql);

    return result;
}






