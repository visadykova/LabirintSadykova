/*
 * Map.h
 *
 *  Created on: 1 нояб. 2021 г.
 *      Author: unyuu
 */

#ifndef MAP_H_
#define MAP_H_

#include <vector>
#include <cmath>

class MapEntity
{
protected:
	double _x{}, _y{};
	int _id{};
public:
	MapEntity(double x, double y, int id)
	:_x(x), _y(y), _id(id){}

	int id() const {return _id;}
	double x() const {return _x;}
	double y() const {return _y;}
};

class Map
{
protected:
	int _width {}, _height {};
	double _start_x {}, _start_y {}, _start_dir {};
	std::vector<int> _data {};
	std::vector<MapEntity> _entities{};
public:
	Map(const char *filename);
	virtual ~Map() = default;
	const std::vector<MapEntity>& entities() const
	{
		return _entities;
	}

	int    width()     const { return _width; }
	int    height()    const { return _height; }
	double start_x()   const { return _start_x; }
	double start_y()   const { return _start_y; }
	double start_dir() const { return _start_dir; }

	int cell(int x, int y)
	{
		if (x < 0 or x >= _width or y < 0 or y >= _height)
			return 1;
		return _data[y * _width + x];
	}

	int cell(double x, double y)
	{
		return cell(int(floor(x)), int(floor(y)));
	}

	bool is_space(int x, int y)
	{
			return cell(x, y) == 0;
	}

	bool is_space(double x, double y)
	{
		return is_space(int(floor(x)), int(floor(y)));
	}
};

#endif /* MAP_H_ */
