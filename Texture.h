/*
 * Texture.h
 *
 *  Created on: 8 дек. 2021 г.
 *      Author: админ
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL2/SDL.h>
#include <memory>

class Texture
{
protected:
	std::shared_ptr<SDL_Renderer> _renderer;
	std::shared_ptr<SDL_Texture> _texture;
	int _width {}, _height {};
public:
	Texture(std::shared_ptr<SDL_Renderer>& ren,
			const char *filename) ;
	virtual ~Texture() = default;

	void  draw_col(int col, int y, int h, double s);

	int height() const {return _height; }
	int width() const {return _width; }
};

#endif /* TEXTURE_H_ */
