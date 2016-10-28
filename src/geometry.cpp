#include "typedef.hpp"
#include "geometry.hpp"

Geometry::Geometry(){}

const multi_index_t &Geometry::Size() const {
  return _size;
}

const multi_real_t &Geometry::Mesh() const {
  return _h;
}