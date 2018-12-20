#include "Model.h"

const std::string &MODEL::Relationship::getUsername() const {
    return username;
}

void MODEL::Relationship::setUsername(const std::string &username) {
    Relationship::username = username;
}

const std::string &MODEL::Relationship::getFriend_username() const {
    return friend_username;
}

void MODEL::Relationship::setFriend_username(const std::string &friend_username) {
    Relationship::friend_username = friend_username;
}

const std::string &MODEL::Relationship::getRemark_name() const {
    return remark_name;
}

void MODEL::Relationship::setRemark_name(const std::string &remark_name) {
    Relationship::remark_name = remark_name;
}


MODEL::Relationship::Relationship(const std::string &username, const std::string &friend_username,
                                  const std::string &remark_name) : username(username),
                                                                    friend_username(
                                                                            friend_username),
                                                                    remark_name(remark_name) {}

MODEL::Relationship::Relationship(const std::string &username) {}
