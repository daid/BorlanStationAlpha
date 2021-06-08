#pragma once


namespace ecs {

template<typename T, size_t TURNS> class System
{
public:
    static_assert(TURNS > 0, "TURNS needs to be above zero.");

    void turn()
    {
        if (delay) {
            delay--;
        } else {
            delay = TURNS - 1;
            static_cast<T*>(this)->run();
        }
    }

private:
    int delay{TURNS > 1 ? 1 : 0};
};

template<typename... SYSTEMS> class Systems
{
public:
    void turn()
    {
        (std::get<SYSTEMS>(systems).turn(), ...);
    }
private:
    std::tuple<SYSTEMS...> systems;
};

}
