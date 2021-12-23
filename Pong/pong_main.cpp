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
        // Bad hack...
        auto const base_path = path{SDL_GetBasePath()};
        auto const image_dir = base_path / path{"Images"};
        auto const image_path = image_dir / path{"table_tennis.bmp"};
        auto const paddle_image = game.load_media(image_path);

        if (paddle_image) {
            game.show_media(paddle_image.get());
        }

        game.draw();
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