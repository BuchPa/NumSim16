#include "typedef.hpp"
#include "geometry.hpp"

Geometry::Geometry(){
  _size   = {128, 128};
  _length = {1.0, 1.0};
  
  for (uint32_t i = 0; i < 2; ++i)
    _h[i] = _length[i]/_size[i];
}

const multi_index_t &Geometry::Size() const {
  return _size;
}

const multi_real_t &Geometry::Mesh() const {
  return _h;
}

const multi_real_t &Geometry::Length() const{
  return _length;
}