#include "blueprint.h"
#include "components.h"


static void load_blueprints_from(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    std::string contents;
    size_t size;
    do {
        char buffer[1024];
        size = fread(buffer, 1, sizeof(buffer), f);
        contents.append(buffer, size);
    } while(size);
    fclose(f);

    std::string_view input_data{contents};
    auto skip_whitespace = [&input_data]() {
        auto n = input_data.find_first_not_of(" \r\n\t");
        if (n != std::string_view::npos) {
            input_data = input_data.substr(n);
        } else if (input_data.find_first_of(" \r\n\t") != std::string_view::npos) {
            input_data = {};
        }
    };
    auto read_string = [&input_data, &skip_whitespace]() -> std::string_view {
        auto n = input_data.find_first_of(" \r\n\t{}");
        if (n == std::string_view::npos) n = input_data.size();
        auto result = input_data.substr(0, n);
        input_data = input_data.substr(n);
        skip_whitespace();
        return result;
    };
    auto expect = [&input_data, &skip_whitespace](char c) -> bool {
        if (input_data.empty() || input_data[0] != c) return false;
        input_data = input_data.substr(1);
        skip_whitespace();
        return true;
    };

    skip_whitespace();
    while(true) {
        auto blueprint_name = read_string();
        if (!expect('{')) break;

        auto blueprint = engine.create();

        while(true) {
            auto component_key = read_string();
            if (!expect('{')) break;
            std::vector<std::string_view> args;
            std::unordered_map<std::string_view, std::string_view> kwargs;
            while(true) {
                if (input_data[0] == '}') {
                    break;
                }
                auto data_key = read_string();
                if (input_data[0] == '{') {
                    auto n = input_data.find('}');
                    auto value = input_data.substr(1, n - 1);
                    kwargs[data_key] = value;
                    input_data = input_data.substr(n + 1);
                    skip_whitespace();
                } else {
                    args.push_back(data_key);
                }
            }
            if (!expect('}')) break;

            //TODO: Create component with parameters
        }
        if (!expect('}')) break;
    }
    if (!input_data.empty())
        printf("Failed to parse %s, data left: %s (%d)\n", filename, std::string(input_data).c_str(), input_data[0]);
}

void load_blueprints()
{
    load_blueprints_from("resource/items.txt");
}
