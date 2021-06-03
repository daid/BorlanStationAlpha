#include "messagelog.h"


MessageLog mlog;


void MessageLog::add(std::string_view msg)
{
    new_messages.emplace_back(msg);
}

//Get the latest seen message. Offset: 0=latest, 1=one before that, etc...
std::string_view MessageLog::back(int offset)
{
    if (offset >= int(seen_messages.size()))
        return "";
    return *(seen_messages.end() - offset - 1);
}

bool MessageLog::has_new_messages()
{
    return new_messages.size() > 0;
}

std::string_view MessageLog::new_message()
{
    if (new_messages.empty())
        return "";
    return new_messages[0];
}

void MessageLog::confirm_new_message()
{
    seen_messages.emplace_back(std::move(new_messages.front()));
    new_messages.erase(new_messages.begin());
}
