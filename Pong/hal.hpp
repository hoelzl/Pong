#pragma once

#include <SDL.h>
#include <filesystem>
#include <gsl/gsl>
#include <optional>

class GraphicsDeviceException : public std::exception
{
    using exception::exception;
};

class CouldNotInitGraphicsDevice final : public GraphicsDeviceException
{
    using GraphicsDeviceException::GraphicsDeviceException;
};

class CouldNotCreateWindow final : public GraphicsDeviceException
{
    using GraphicsDeviceException::GraphicsDeviceException;
};

class CouldNotGetDrawingSurface final : public GraphicsDeviceException
{
    using GraphicsDeviceException::GraphicsDeviceException;
};

constexpr auto screen_width = 1024;
constexpr auto screen_height = 786;

constexpr auto frame_time_in_ms = 13.333;

inline auto constexpr delete_sdl_surface
    = [](SDL_Surface* surface) { SDL_FreeSurface(surface); };

class SdlHal final
{
public:
    using MediaType = SDL_Surface;
    using UniqueMediaPtrType = std::unique_ptr<MediaType, decltype(delete_sdl_surface)>;
    using Event = SDL_Event;
    using EventType = uint32_t;

    static constexpr EventType quit_event = SDL_QUIT;

    static constexpr EventType event_type(Event event) { return event.type; }

    static double get_time_in_ms() { return SDL_GetTicks(); }
    static void delay(double time_in_ms);

    SdlHal();

    SdlHal(SdlHal const& other) = delete;
    SdlHal& operator=(SdlHal const& other) = delete;

    // TODO: Figure out correct semantics for move operations.
    SdlHal(SdlHal&& other) noexcept = delete;
    SdlHal& operator=(SdlHal&& other) noexcept = delete;

    ~SdlHal();

    void init();

    [[nodiscard]] UniqueMediaPtrType
    load_media(std::filesystem::path const& media_path) const;

    void show_media(MediaType* media) const;

    [[nodiscard]] std::optional<Event> get_next_event() const;
    void draw() const;

    void shutdown() { shutdown_ = true; }

private:
    bool shutdown_{false};

    gsl::not_null<SDL_Window*> window_;
    gsl::not_null<SDL_Surface*> screen_surface_;

    static SDL_Window* create_window();
    static SDL_Surface* create_surface(SDL_Window* window);

    [[nodiscard]] std::pair<SDL_Rect, SDL_Rect>
    compute_centered_boundary_rects(MediaType* media) const;
};


template <typename Hal>
concept HardwareAbstractionLayer = requires(
    Hal hal, Hal const hal_const, typename Hal::Event e, typename Hal::MediaType media,
    std::filesystem::path path)
{
    // clang-format off
    std::default_initializable<Hal>;
    std::equality_comparable<typename Hal::EventType>;
    { Hal::event_type(e) } -> std::same_as<typename Hal::EventType>;
    hal.init();
    { hal_const.load_media(path) } -> std::same_as<typename Hal::UniqueMediaPtrType>;
    hal_const.show_media(&media);
    { hal_const.get_next_event() } -> std::same_as<std::optional<typename Hal::Event>>;
    hal_const.draw();
    hal.shutdown();
    // clang-format on
};

static_assert(HardwareAbstractionLayer<SdlHal>);