
#include "hal.hpp"

#include <filesystem>
#include <utility>

using std::filesystem::path, std::filesystem::exists;

SdlHal::SdlHal() : window_{create_window()}, screen_surface_{create_surface(window_)} {}

SdlHal::~SdlHal()
{
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void SdlHal::delay(double time_in_ms)
{
    if (time_in_ms > 0.0) {
        SDL_Delay(static_cast<uint32_t>(time_in_ms));
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void SdlHal::init() { SDL_UpdateWindowSurface(window_); }

// ReSharper disable once CppMemberFunctionMayBeStatic
std::optional<SdlHal::Event> SdlHal::get_next_event() const
{
    Event event{};
    if (SDL_PollEvent(&event) != 0) {
        return event;
    }
    return {};
}

void SdlHal::refresh_window() const
{
    SDL_FillRect(
        screen_surface_, nullptr,
        SDL_MapRGB(screen_surface_->format, 0xFF, 0xFF, 0xFF));

    for (auto& fun : draw_callbacks_) {
        if (fun) {
            fun();
        }
    }
    SDL_UpdateWindowSurface(window_);
}

// ReSharper disable once CppMemberFunctionMayBeStatic

std::filesystem::path SdlHal::base_path() const
{
    if (!base_path_.has_value()) {
        base_path_ = {SDL_GetBasePath()};
    }
    return *base_path_;
}

std::filesystem::path SdlHal::image_path() const
{
    return base_path() / std::filesystem::path{"Images"};
}

void SdlHal::add_draw_callback(DrawCallback fun)
{
    draw_callbacks_.push_back(std::move(fun));
}

SdlHal::UniqueMediaPtr SdlHal::load_media(std::filesystem::path const& media_path) const
{

    if (exists(media_path)) {
        auto* media_surface = SDL_LoadBMP(media_path.string().c_str());
        return {media_surface, delete_sdl_surface};
    }
    return {};
}

void SdlHal::show_centered_image(Media* media) const
{
    if (media) {
        auto [media_rect, screen_rect] = compute_centered_boundary_rects(media);
        SDL_BlitSurface(media, &media_rect, screen_surface_, &screen_rect);
    }
}

SDL_Window* SdlHal::create_window()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw CouldNotInitGraphicsDevice{SDL_GetError()};
    }

    auto* window = SDL_CreateWindow(
        "Pong!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width,
        screen_height, SDL_WINDOW_RESIZABLE);

    if (!window) {
        throw CouldNotCreateWindow{SDL_GetError()};
    }

    return window;
}

SDL_Surface* SdlHal::create_surface(SDL_Window* window)
{
    auto* screen_surface = SDL_GetWindowSurface(window);
    if (!screen_surface) {
        throw CouldNotGetDrawingSurface{SDL_GetError()};
    }
    return screen_surface;
}


std::pair<SDL_Rect, SDL_Rect>
SdlHal::compute_centered_boundary_rects(Media* media) const
{
    SDL_Rect media_rect, screen_rect;
    SDL_GetClipRect(media, &media_rect);
    SDL_GetClipRect(screen_surface_, &screen_rect);
    auto const delta_y = screen_rect.h - media_rect.h;
    screen_rect.y += delta_y / 2;
    screen_rect.h -= delta_y / 2;
    auto const delta_x = screen_rect.w - media_rect.w;
    screen_rect.x += delta_x / 2;
    screen_rect.w -= delta_x / 2;
    return {media_rect, screen_rect};
}
