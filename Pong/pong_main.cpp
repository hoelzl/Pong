#include <SDL.h>
#include <filesystem>
#include <iostream>

#include "pong.hpp"

using std::filesystem::path;

constexpr auto could_not_init_graphics_device = 1;
constexpr auto could_not_create_window = 2;
constexpr auto could_not_create_drawing_surface = 3;

int main(int argc, char* argv[])
{
    try {
        auto game = PongGame{};
        game.init();

        auto const image_path = game.hal().image_path() / path{"table_tennis.bmp"};
        // ReSharper disable once CppTooWideScope
        auto const paddle_image = game.load_media(image_path);

        if (paddle_image) {
            game.add_draw_callback([&game, &paddle_image]() {
                game.show_centered_image(paddle_image.get());
            });
        }

        game.run_event_loop();
        return 0;
    }
    catch (CouldNotInitGraphicsDevice const& err) {
        std::cerr << "Could not initialize SDL: " << err.what() << "\n";
        return could_not_init_graphics_device;
    }
    catch (CouldNotCreateWindow const& err) {
        std::cerr << "Could not create window: " << err.what() << "\n";
        return could_not_create_window;
    }
    catch (CouldNotGetDrawingSurface const& err) {
        std::cerr << "Could not get surface from window: " << err.what() << "\n";
        return could_not_create_drawing_surface;
    }
}