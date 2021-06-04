#pragma once

#include "view.h"

#include <optional>


class MenuView : public View
{
public:
    MenuView(std::string_view title, const std::vector<std::string>& options);

    void draw(Frontend& frontend) override;
    std::optional<int> get_result(Frontend& frontend);
private:
    std::string title;
    std::vector<std::string> options;
    int selected_index{0};
};
