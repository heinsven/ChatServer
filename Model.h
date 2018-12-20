#ifndef CHAT_MODEL_H
#define CHAT_MODEL_H

#include <string>
#include "Socket.h"

namespace FACTORY {
    class Factory;
}

namespace SOCKET {
    class Socket;
}


namespace MODEL {
    class Relationship {
    public:
        const std::string &getUsername() const;

        void setUsername(const std::string &username);

        const std::string &getFriend_username() const;

        void setFriend_username(const std::string &friend_username);

        const std::string &getRemark_name() const;

        void setRemark_name(const std::string &remark_name);


        Relationship(const std::string &username, const std::string &friend_username, const std::string &remark_name);

        Relationship(const std::string &username);

        ~Relationship() {

        }

    private:
        std::string username;
        std::string friend_username;
        std::string remark_name;
    };

    struct arg_add_friend {
        int sockfd;
        std::string friend_username;
        FACTORY::Factory *chat;
    };

    struct arg_remove_friend {
        int sockfd;
        std::string friend_username;
        FACTORY::Factory *chat;
    };

    struct arg_edit_remark {
        int sockfd;
        std::string username;
        std::string friend_username;
        std::string remark;
        FACTORY::Factory *chat;
    };

    struct arg_logout {
        int sockfd;
        FACTORY::Factory *chat;
        SOCKET::Socket *socket;
    };

    struct arg_send_text {
        int sockfd;
        std::string send_to;
        std::string recv_from;
        std::string text;
        FACTORY::Factory *chat;
    };

    struct arg_send_file {
        int sockfd;
        std::string send_to;
        std::string recv_from;
        FACTORY::Factory *chat;
        SOCKET::Socket *socket;
    };

}


#endif //CHAT_MODEL_H
