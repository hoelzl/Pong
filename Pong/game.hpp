#pragma once

#include <gsl/gsl>

#include "hal.hpp"

template <HardwareAbstractionLayer Hal>
class Game
{
public:
    Game() = default;
    Game(Game const& other) = delete;
    Game(Game&& other) noexcept = delete;
    Game& operator=(Game const& other) = delete;
    Game& operator=(Game&& other) noexcept = delete;
    virtual ~Game() = default;

    void init();

    typename Hal::UniqueMediaPtrType
    load_media(std::filesystem::path const& media_path);

    void run_event_loop();

    void show_media(typename Hal::MediaType* media) const;
    void draw() const;

    [[nodiscard]] bool quit() const { return quit_; }
    void quit(bool quit) { quit_ = quit; }

private:
    Hal device_{};

    bool quit_{false};
    uint32_t last_tick_time_{};

    void wait_for_tick();
};

template <HardwareAbstractionLayer Hal>
void Game<Hal>::init()
{
    device_.init();
}

template <HardwareAbstractionLayer Hal>
typename Hal::UniqueMediaPtrType
Game<Hal>::load_media(std::filesystem::path const& media_path)
{
    return device_.load_media(media_path);
}

template <HardwareAbstractionLayer Hal>
void Game<Hal>::run_event_loop()
{
    while (!quit_) {
        wait_for_tick();
        // VC++ cannot infer the type for event, even though it should be able to...
        while (std::optional<typename Hal::Event> event = device_.get_next_event()) {
            if (Hal::event_type(*event) == Hal::quit_event) {
                quit_ = true;
            }
        }
    }
}

template <HardwareAbstractionLayer Hal>
void Game<Hal>::show_media(typename Hal::MediaType* media) const
{
    device_.show_media(media);
}

template <HardwareAbstractionLayer Hal>
void Game<Hal>::draw() const
{
    device_.draw();
}

template <HardwareAbstractionLayer Hal>
void Game<Hal>::wait_for_tick()
{
    double const current_tick_time = Hal::get_time_in_ms();
    if (auto const time_to_next_tick = current_tick_time - last_tick_time_;
        time_to_next_tick > 0) {
        Hal::delay(time_to_next_tick);
    }
    last_tick_time_ = SDL_GetTicks();
}
