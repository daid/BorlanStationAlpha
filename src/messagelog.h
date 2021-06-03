#pragma once

#include <vector>
#include <string>
#include <string_view>


class MessageLog
{
public:
    void add(std::string_view msg);

    //Get the latest seen message. Offset: 0=latest, 1=one before that, etc...
    std::string_view back(int offset);
    bool has_new_messages();
    std::string_view new_message();
    void confirm_new_message();
private:
    std::vector<std::string> seen_messages;
    std::vector<std::string> new_messages;
};

extern MessageLog mlog;