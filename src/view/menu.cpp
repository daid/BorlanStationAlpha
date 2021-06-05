#include "menu.h"
#include "format.h"

static const Color fg{1, 1, 1};
static const Color bg{0, 0, 0};


MenuView::MenuView(std::string_view _title, const std::vector<std::string>& _options)
: title(_title), options(_options)
{
}

void MenuView::draw(Frontend& frontend)
{
    size_t width = title.size();
    for(const auto& option : options)
        width = std::max(width, option.size());
    Vector2i size = Vector2i{width, options.size()};
    Vector2i position = (frontend.get_draw_size() - Vector2i{15, 0} - size) / 2;
    
    frontend.fill(position - Vector2i{2, 2}, size + Vector2i{4, 4}, ' ', fg, bg);

    frontend.fill(position - Vector2i{2, 2}, {1, size.y + 4}, '|', fg, bg);
    frontend.fill(position + Vector2i{size.x + 1, -2}, {1, size.y + 4}, '|', fg, bg);
    frontend.fill(position - Vector2i{2, 2}, {size.x + 4, 1}, '-', fg, bg);
    frontend.fill(position + Vector2i{-2, size.y + 1}, {size.x + 4, 1}, '-', fg, bg);

    frontend.draw(position.x, position.y - 2, title, fg);

    int idx = 0;
    for(const auto& option : options) {
        if (idx == selected_index) {
            frontend.fill(position, {size.x, 1}, ' ', bg, fg);
            frontend.draw(position.x, position.y++, format("@: @", char('a' + idx), option), bg, fg);
        } else {
            frontend.draw(position.x, position.y++, format("@: @", char('a' + idx), option), fg);
        }
        idx++;
    }
}

std::optional<int> MenuView::get_result(Frontend& frontend)
{
    while(true) {
        View::draw_views(frontend);

        auto input = frontend.get_input();
        if (input == INPUT_UP) selected_index = (selected_index + options.size() - 1) % options.size();
        if (input == INPUT_DOWN) selected_index = (selected_index + 1) % options.size();
        if (input == '\r') return selected_index;
        if (input == INPUT_QUIT) return {};
        if (input == INPUT_CANCEL) return {};
        if (input >= 'a' && input < 'a' + options.size()) return input - 'a';
    }
}
