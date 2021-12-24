/*
 * MazeWindow.h
 *
 *  Created on: 1 нояб. 2021 г.
 *      Author: unyuu
 */

#ifndef MAZEWINDOW_H_
#define MAZEWINDOW_H_

#include <memory>
#include "Window.h"
#include "Map.h"
#include "Player.h"
#include "Texture.h"

class MazeWindow final : public Window
{
private:
	std::shared_ptr<Map> _map {};
	Player _player;
	Texture _wall_texture {_renderer, "wall.png"};
	Texture _wall2_texture {_renderer, "wall2.png"};
	Texture _plant {_renderer, "plant.png"};

	static constexpr int WINDOW_WIDTH = 1100;
	static constexpr int WINDOW_HEIGHT = 800;

	static constexpr int MINIMAP_CELL_SIZE = 64;
	static constexpr int PLAYER_DOT_SIZE = 11;
	static constexpr int PLAYER_ARROW_SIZE = 40;

	void draw_minimap();
	void draw_player();
	void cast_ray(double dx, double dy, double &rx, double &ry);

protected:
	virtual void render() override;
	virtual void handle_keys(const Uint8 *keys) override;

public:
	MazeWindow();
	virtual ~MazeWindow() = default;
};

#endif /* MAZEWINDOW_H_ */
