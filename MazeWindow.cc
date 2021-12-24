/*
 * MazeWindow.cc
 *
 *  Created on: 1 нояб. 2021 г.
 *      Author: unyuu
 */
#include <algorithm>
#include "MazeWindow.h"

MazeWindow::MazeWindow()
: Window(WINDOW_WIDTH, WINDOW_HEIGHT)
{
	SDL_SetRenderDrawBlendMode(_renderer.get(), SDL_BLENDMODE_BLEND);
	_map = std::make_shared<Map>("level01.txt");
	_player.spawn(_map);
}

void MazeWindow::draw_minimap()
{
	for (int row = 0; row < _map->height(); ++row) {
		for (int col = 0; col < _map->width(); ++col) {
			SDL_Rect cell_rect { col * MINIMAP_CELL_SIZE, row
			        * MINIMAP_CELL_SIZE,
			                     MINIMAP_CELL_SIZE, MINIMAP_CELL_SIZE };
			if (_map->is_space(col, row)) {
				SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 63, 127);
			} else {
				SDL_SetRenderDrawColor(_renderer.get(), 196, 196, 196, 127);
			}
			SDL_RenderFillRect(_renderer.get(), &cell_rect);
		}
	}
}

void MazeWindow::draw_player()
{
	SDL_Rect player_rect {
	        int(_player.x() * MINIMAP_CELL_SIZE - PLAYER_DOT_SIZE / 2), int(
	                _player.y() * MINIMAP_CELL_SIZE - PLAYER_DOT_SIZE / 2),
	        PLAYER_DOT_SIZE, PLAYER_DOT_SIZE };
	SDL_SetRenderDrawColor(_renderer.get(), 255, 63, 63, 255);
	SDL_RenderFillRect(_renderer.get(), &player_rect);
	int x1 = int(_player.x() * MINIMAP_CELL_SIZE);
	int y1 = int(_player.y() * MINIMAP_CELL_SIZE);
	int x2 = int(x1 + cos(_player.dir()) * PLAYER_ARROW_SIZE);
	int y2 = int(y1 + sin(_player.dir()) * PLAYER_ARROW_SIZE);
	SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 0, 255);
	SDL_RenderDrawLine(_renderer.get(), x1, y1, x2, y2);
}

void MazeWindow::cast_ray(double dx, double dy, double &rx, double &ry) {
	//бросить лучик
	do {
		rx += dx;
		ry += dy;
	} while (_map->is_space(rx, ry));
}

void MazeWindow::render()
{
	SDL_Rect sky_rect   { 0, 0,          width(), height()/2 };
	SDL_Rect floor_rect { 0, height()/2, width(), height()/2 };
	SDL_SetRenderDrawColor(_renderer.get(), 190, 213, 255, 255);
	SDL_RenderFillRect(_renderer.get(), &sky_rect);
	SDL_SetRenderDrawColor(_renderer.get(), 106, 106, 106, 255);
	SDL_RenderFillRect(_renderer.get(), &floor_rect);

	std::vector<double> depth_buffer(width());
	double alpha = _player.dir();
	static constexpr double EPS = 0.00001;
	double px = _player.x();
	double py = _player.y();
	double Ds = width() / 2.0 / tan(_player.fov()/2.0); // Расстояние до экрана
	double Hw = 0.5; // Высота стены в клетках
	for (int col = 0; col < width(); ++col) {
		double h;
		double rx, ry;
		double dx, dy;
		double Dw, Dv, Dh;
		double Sw, Sv, Sh;
		int    Tw, Tv, Th;
		double gamma = atan2(col - width()/2, Ds);
		double beta  = alpha + gamma;

		if (cos(beta) > EPS) { // в вертикальную вправо
			rx = floor(px) + EPS;
			dx = 1.0;
			dy = tan(beta);
			ry = py - (px - rx) * dy;


			// бросить лучик
			cast_ray(dx, dy, rx, ry);

			Dv = hypot(rx - px, ry - py);
			Sv = ry - floor(ry);
			Tv = _map->cell(rx, ry);

		} else if (cos(beta) < -EPS) { // в вертикальную влево
			rx = ceil(px) - EPS;
			dx = -1.0;
			dy = tan(-beta);
			ry = py - (rx - px) * dy;

			//бросить лучик
			cast_ray(dx, dy, rx, ry);

			Dv = hypot(rx - px, ry - py);
			Sv = ceil(ry) - ry;
			Tv = _map->cell(rx, ry);
		} else { // вдоль вертикальных линий
			Dv = INFINITY;
			Sv = 0.0;
			Tv = 1;
		}

		// Вычислить Dh
		if(sin(beta)>EPS){
			dy = 1.0;
			dx = 1.0 / tan(beta);
			ry = floor(py)+EPS;
			rx = px-(py-ry)*dx;
			cast_ray(dx, dy, rx, ry);
			Dh = hypot(rx-px, ry-py);
			Sh= ceil(rx) - rx;
			Th = _map->cell(rx, ry);
		}else if (sin(beta)< -EPS){
			dy = -1.0;
			dx = 1.0 / tan(-beta);
			ry = ceil(py)-EPS;
			rx = px - (ry - py)*dx;
			cast_ray(dx, dy, rx, ry);
			Dh = hypot(rx-px, ry-py);
			Sh = rx - floor (rx);
			Th = _map->cell(rx, ry);
		}else {
			Dh = INFINITY;
			Sh = 0.0;
			Th = 1;
		}

		// Вычислить Dw

		if(Dv < Dh){
			Dw = Dv;
			Sw = Sv;
			Tw = Tv;
		}else {
			Dw = Dh;
			Sw = Sh;
			Tw = Th;
		}
		// Вычислить h

		h = Ds * Hw / Dw / cos(gamma);
		depth_buffer[col] = Dw;

		// Нарисовать столбец
//		SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 192, 255);
//		SDL_RenderDrawLine(_renderer.get(),
//					col,
	//				height()/2-int(h/2),
	//				col,
//					height()/2+int(h/2));
	auto t = _wall_texture;
	if (Tw == 2) t = _wall2_texture;

	t.draw_col(
			col,
			height()/2-int(h/2),
			int(h),
			Sw);

	}


	struct EntIdx{
		int index;
		double dist;
	};
	auto ents = _map ->entities();
	std::vector<EntIdx> indices(ents.size());
	for(unsigned i = 0; i < indices.size(); ++i){
		indices[i].index = i;
		indices[i].dist = hypot(
				ents[i].x() - px, ents[i].y() - py);
	}
	// здесь сортируем
	std::sort(std::begin(indices), std::end(indices),
			[&](const EntIdx &a, const EntIdx &b) -> int{
		return a.dist > b.dist;
	});
	for (auto && idx: indices){
		auto e = ents[idx.index];
		double dist = hypot(e.y() - py, e.x() - px);
		double beta = atan2(e.y() - py, e.x() - px);
		double gamma = beta - alpha;
		if(cos(gamma)<= EPS) continue;
		double h = Ds * Hw / dist / cos(gamma);
		int rw = int(_plant.width() * h / _plant.height());
		int rh = int(h);
		int rx = int (width() / 2 + Ds * tan(gamma) - rw/2);
		int ry = (height() / 2 - rh/2);
		int col;
		double tcol;
		for (col = rx, tcol = 0.0;
				tcol <= 1.0;
				++col, tcol += 1.0 / rw){
			if(col < 0 or col >= width()) continue;
			if(dist < depth_buffer[col]){
				depth_buffer[col] = dist;
				_plant.draw_col(col, ry, h, tcol);
			}
		}
	}

	draw_minimap();
	draw_player();
}

void MazeWindow::handle_keys(const Uint8 *keys)
{
	if (keys[SDL_SCANCODE_E]) _player.turn_right();
	if (keys[SDL_SCANCODE_Q]) _player.turn_left();
	if (keys[SDL_SCANCODE_W]) _player.walk_forward();
	if (keys[SDL_SCANCODE_S]) _player.walk_backward();
	if (keys[SDL_SCANCODE_D]) _player.shift_right();
	if (keys[SDL_SCANCODE_A]) _player.shift_left();
}
