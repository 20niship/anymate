#pragma once

#include <functional>
#include <assert.h>

namespace Magi::Filter{

template<typename T>
void _edge_preserve_horizontal(T *image, const int width, const int height, const float alpha, const float deltaZ, const int _holes_filling_radius = 10);

template <typename T>
void _edge_preserve_vertical(void * image_data,  const int width, const int height, const float alpha, const float deltaZ);

template<typename T>
void edge_preserve(T *img, const int width, const int height, const int itr, const float alpha, const float deltaZ, const int _holes_filling_radius = 10);

template<typename T>
void holes_fill_from_left(T *img, const int width, const int height);

template<typename T>
void holes_fill_from_right(T *img, const int width, const int height);

template<typename T>
void holes_fill_farest(T* image_data, int width, int height);

template<typename T>
void holes_fill_nearest(T* image_data, int width, int height);

} //namespace Magi


