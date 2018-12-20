#ifndef CHAT_USER_H
#define CHAT_USER_H

#include <string>
#include <vector>
#include <list>
#include "UserInfo.h"

namespace USER {
    class User {

    public:
        static pthread_mutex_t mutex;
        static pthread_cond_t cond;
        static std::list<USER::User> onlineUsers;
        static std::list<int> onlineSockets;

        User(const std::string &username, const std::string &password, const std::string &nickname,
             const std::string &status);

        User();

        virtual ~User();

        const std::string &getUsername() const;

        void setUsername(const std::string &username);

        const std::string &getPassword() const;

        void setPassword(const std::string &password);

        const std::string &getNickname() const;

        void setNickname(const std::string &nickname);

        const std::string &getStatus() const;

        void setStatus(const std::string &status);

        void addOnlineFriend(int socket, User user);

        void removeOnlineFriend(User user);

        User *getOnlineFriend(std::string username);

        int operator[](User user);

        bool operator==(const USER::User &user);

        static void initSignal();

        static void addOnlineUsers(int socket, User user);

        static void removeOnlineUsers(User user);


    private:
        std::string username;
        std::string password;
        std::string nickname;
        std::string status;

        struct UserInfo *userInfo;

    };
}


#endif //CHAT_USER_H
