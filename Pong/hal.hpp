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
    using Media = SDL_Surface;
    using UniqueMediaPtr = std::unique_ptr<Media, decltype(delete_sdl_surface)>;
    using Event = SDL_Event;
    using EventType = uint32_t;
    using DrawCallback = std::function<void()>;

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

    [[nodiscard]] std::optional<Event> get_next_event() const;

    void refresh_window() const;

    void shutdown() { shutdown_ = true; }

    std::filesystem::path base_path() const;
    std::filesystem::path image_path() const;

    void add_draw_callback(DrawCallback fun);

    [[nodiscard]] UniqueMediaPtr
    load_media(std::filesystem::path const& media_path) const;

    void show_centered_image(Media* media) const;

private:
    mutable std::optional<std::filesystem::path> base_path_{};
    std::vector<DrawCallback> draw_callbacks_{};
    bool shutdown_{false};

    gsl::not_null<SDL_Window*> window_;
    gsl::not_null<SDL_Surface*> screen_surface_;

    static SDL_Window* create_window();
    static SDL_Surface* create_surface(SDL_Window* window);

    [[nodiscard]] std::pair<SDL_Rect, SDL_Rect>
    compute_centered_boundary_rects(Media* media) const;
};


template <typename Hal>
concept HardwareAbstractionLayer = requires(
    Hal hal, Hal const hal_const, typename Hal::Event e, typename Hal::Media media,
    std::filesystem::path path, double time)
{
    // clang-format off
    std::default_initializable<Hal>;
    std::equality_comparable<typename Hal::EventType>;
    std::indirectly_readable<typename Hal::UniqueMediaPtr>;
    std::indirectly_writable<typename Hal::Media, typename Hal::UniqueMediaPtr>;

    { Hal::quit_event } -> std::convertible_to<typename Hal::EventType>;

    { Hal::event_type(e) } -> std::same_as<typename Hal::EventType>;
    { Hal::get_time_in_ms() } -> std::same_as<double>;
    Hal::delay(time);

    hal.init();
    { hal_const.get_next_event() } -> std::same_as<std::optional<typename Hal::Event>>;
    hal_const.refresh_window();
    hal.shutdown();

    { hal_const.base_path() } -> std::same_as<std::filesystem::path>;

    { hal_const.load_media(path) } -> std::same_as<typename Hal::UniqueMediaPtr>;
    hal_const.show_centered_image(&media);
    // clang-format on
};

static_assert(HardwareAbstractionLayer<SdlHal>);