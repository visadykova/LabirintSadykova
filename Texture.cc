/*
 * texture.cc
 *
 *  Created on: 8 дек. 2021 г.
 *      Author: норд
 */

#include <SDL2/SDL_image.h>
#include <stdexcept>
#include "Texture.h"

Texture::Texture(std::shared_ptr<SDL_Renderer>& ren,
        const char *filename)
: _renderer(ren)
{
    _texture = std::shared_ptr<SDL_Texture>(
            IMG_LoadTexture(ren.get(), filename),
            SDL_DestroyTexture);
    if (_texture == nullptr)
        throw std::runtime_error(
                std::string("Не могу загрузить текстуру: ")+
                std::string(SDL_GetError()));
    SDL_QueryTexture(_texture.get(),
            nullptr, nullptr, &_width, &_height);
}

void Texture::draw_col(int col, int y, int h, double s)
{
    SDL_Rect src { int(floor(s * _width)), 0, 1, _height };
    SDL_Rect dst {col, y, 1, h };
    SDL_RenderCopy(_renderer.get(), _texture.get(), &src, &dst);

}
