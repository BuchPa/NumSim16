#include "grid.hpp"

#include "geometry.hpp"
#include "iterator.hpp"

#include <cmath>     // std::fabs

using namespace std;

/// Returns the value for the hat function at the given position on the unit
///  square. This function provides weights for each corner value that,
///  multiplied in sum by the values, interpolate the value based on the four
///  corner values.
///
/// @param mode int Which mode of the four-part hat function is evaluated. The
///    lower left corner is 1, lower right is 2, upper left is 3
///   and upper right is 4
///  @param pos multi_real_t The position to evaluate. It assumed this is
///    relative to the unit square
real_t hat(const int &mode, const multi_real_t &pos) {
  switch(mode) {
    case 1:
      return pos[0] * pos[1] - pos[1] - pos[0] + 1;
    case 2:
      return -pos[0] * pos[1] + pos[0];
    case 3:
      return -pos[0] * pos[1] + pos[1];
    case 4:
      return pos[0] * pos[1];
  }
  return 0.0;
}

Grid::Grid(const Geometry *geom)
    : _geom(geom) {
  
  // Set offset to default
  _offset[0] = real_t(0.0);
  _offset[1] = real_t(0.0);
  
  // Calculate grid size and create data
  const multi_index_t size = _geom->Size();
  _data = new real_t[(size[0])*(size[1])];
  
  // Init data with zeros
  this->Initialize(real_t(0.0));
}

Grid::Grid(const Geometry *geom, const multi_real_t &offset)
    : _geom(geom), _offset(offset) {
  
  // Calculate grid size and create data
  const multi_index_t size = _geom->Size();
  _data = new real_t[(size[0])*(size[1])];
  _bufferX = new real_t[(size[0])];
  _bufferY = new real_t[(size[1])];
  
  // Init data with zeros
  this->Initialize(real_t(0.0));
}

Grid::~Grid(){
  delete[] _data;
  delete[] _bufferX;
  delete[] _bufferY;
}

void Grid::Initialize(const real_t &value) {
  // Init array with fixed value
  Iterator it(_geom);
  
  for (it.First();it.Valid();it.Next()){
    this->Cell(it) = value;
  }
}

real_t &Grid::Cell(const Iterator &it) {
  return _data[it];
}

const real_t &Grid::Cell(const Iterator &it) const {
  return _data[it];
}

real_t Grid::Interpolate(const multi_real_t &pos) const {
  multi_real_t innerpos = {
    min(_geom->Length()[0], max(0.0, pos[0])) - _offset[0],
    min(_geom->Length()[1], max(0.0, pos[1])) - _offset[1]
  };

  // Clamp to a grid point (lower left corner)
  multi_index_t clamp = {
    (index_t)(floor(innerpos[0] / _geom->Mesh()[0]) + 1),
    (index_t)(floor(innerpos[1] / _geom->Mesh()[1]) + 1)
  };
  // Calculate position within unit square spanned by the four grid points
  multi_real_t modpos = {
    innerpos[0] / _geom->Mesh()[0] - clamp[0] + 1,
    innerpos[1] / _geom->Mesh()[1] - clamp[1] + 1
  };
  
  // Calculate interpolated value by weighing the value of each corner
  // by the weight given by the hat function for that corner
  Iterator it = Iterator(_geom, clamp[1] * _geom->Size()[0] + clamp[0]);
  return this->Cell(it) * hat(1, modpos)
    + this->Cell(it.Right()) * hat(2, modpos)
    + this->Cell(it.Top()) * hat(3, modpos)
    + this->Cell(it.Top().Right()) * hat(4, modpos);

}

real_t Grid::dx_l(const Iterator &it) const{
  return (this->Cell(it) - this->Cell(it.Left())) / _geom->Mesh()[0];
}

real_t Grid::dx_r(const Iterator &it) const{
  return (this->Cell(it.Right()) - this->Cell(it)) / _geom->Mesh()[0];
}

real_t Grid::dy_l(const Iterator &it) const{
  return (this->Cell(it) - this->Cell(it.Down())) / _geom->Mesh()[1];
}

real_t Grid::dy_r(const Iterator &it) const{
  return (this->Cell(it.Top()) - this->Cell(it)) / _geom->Mesh()[1];
}

real_t Grid::dxx(const Iterator &it) const{
  return (this->Cell(it.Right()) + this->Cell(it.Left()) - this->Cell(it) - this->Cell(it))
    / (_geom->Mesh()[0] * _geom->Mesh()[0]);
}

real_t Grid::dyy(const Iterator &it) const{
  return (this->Cell(it.Top()) + this->Cell(it.Down()) - this->Cell(it) - this->Cell(it))
    / (_geom->Mesh()[1] * _geom->Mesh()[1]);
}

real_t Grid::DC_udu_x(const Iterator &it, const real_t &alpha) const {
  real_t ft = pow((this->Cell(it) + this->Cell(it.Right())), 2.0)
    - pow((this->Cell(it.Left()) + this->Cell(it)), 2.0);
  real_t st = fabs(this->Cell(it) + this->Cell(it.Right())) * (this->Cell(it) - this->Cell(it.Right()))
    - fabs(this->Cell(it.Left()) + this->Cell(it)) * (this->Cell(it.Left()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[0];
}

real_t Grid::DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const {
  real_t ft = (v->Cell(it) + v->Cell(it.Right())) * (this->Cell(it) + this->Cell(it.Top()))
    - (v->Cell(it.Down()) + v->Cell(it.Right().Down())) * (this->Cell(it.Down()) + this->Cell(it));
  real_t st = fabs(v->Cell(it) + v->Cell(it.Right())) * (this->Cell(it) - this->Cell(it.Top()))
    - fabs(v->Cell(it.Down()) + v->Cell(it.Right().Down())) * (this->Cell(it.Down()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[1];
}

real_t Grid::DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const {
  real_t ft = (this->Cell(it) + this->Cell(it.Right())) * (u->Cell(it) + u->Cell(it.Top()))
    - (this->Cell(it.Left()) + this->Cell(it)) * (u->Cell(it.Left()) + u->Cell(it.Left().Top()));
  real_t st = fabs(u->Cell(it) + u->Cell(it.Top())) * (this->Cell(it) - this->Cell(it.Right()))
    - fabs(u->Cell(it.Left()) + u->Cell(it.Left().Top())) * (this->Cell(it.Left()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[0];
}

real_t Grid::DC_vdv_y(const Iterator &it, const real_t &alpha) const {
  real_t ft = pow((this->Cell(it) + this->Cell(it.Top())), 2.0)
    - pow((this->Cell(it.Down()) + this->Cell(it)), 2.0);
  real_t st = fabs(this->Cell(it) + this->Cell(it.Top())) * (this->Cell(it) - this->Cell(it.Top()))
    - fabs(this->Cell(it.Down()) + this->Cell(it)) * (this->Cell(it.Down()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[1];
}

real_t Grid::Max() const{
  // Create iterator and cycle _data
  Iterator it(_geom);
  real_t res = _data[0];
  for (it.First();it.Valid();it.Next())
    if (_data[it] > res) res = _data[it];
  return res;
}

real_t Grid::Min() const{
  // Create iterator and cycle _data
  Iterator it(_geom);
  real_t res = _data[0];
  for (it.First();it.Valid();it.Next())
    if (_data[it] < res) res = _data[it];
  return res;
}

real_t Grid::AbsMax() const{
  // Create iterator and cycle _data
  Iterator it(_geom);
  real_t res = fabs(_data[0]);
  for (it.First();it.Valid();it.Next())
    if (fabs(_data[it]) > res) res = fabs(_data[it]);
  return res;
}

real_t *Grid::Data(){
  return _data;
}

void Grid::Print() const{
  // Cycle field with Iterator and print
  Iterator it(_geom);
  
  for (it.First();it.Valid();it.Next()){
    if((it % _geom->Size()[0]) == 0) mprintf("\n");
    mprintf("%6.2f", this->Cell(it));
  }
  mprintf("\n");
}

real_t* Grid::GetLeftBoundary(bool offset) const {
  BoundaryIterator it(_geom, 4);
  index_t idx = 0;

  while (it.Valid()) {
    if (offset) {
      _bufferY[idx++] = this->Cell(it.Right());
    } else {
      _bufferY[idx++] = this->Cell(it);
    }
    it.Next();
  }

  return _bufferY;
}

real_t* Grid::GetRightBoundary(bool offset) const {
  BoundaryIterator it(_geom, 2);
  index_t idx = 0;

  while (it.Valid()) {
    if (offset) {
      _bufferY[idx++] = this->Cell(it.Left());
    } else {
      _bufferY[idx++] = this->Cell(it);
    }
    it.Next();
  }

  return _bufferY;
}

real_t* Grid::GetTopBoundary(bool offset) const {
  BoundaryIterator it(_geom, 3);
  index_t idx = 0;

  while (it.Valid()) {
    if (offset) {
      _bufferX[idx++] = this->Cell(it.Down());
    } else {
      _bufferX[idx++] = this->Cell(it); 
    }
    it.Next();
  }

  return _bufferX;
}

real_t* Grid::GetBottomBoundary(bool offset) const {
  BoundaryIterator it(_geom, 4);
  index_t idx = 0;

  while (it.Valid()) {
    if (offset) {
      _bufferX[idx++] = this->Cell(it.Top());
    } else {
      _bufferX[idx++] = this->Cell(it);
    }
    it.Next();
  }

  return _bufferY;
}

void Grid::WriteLeftBoundary(real_t* data) {
  BoundaryIterator it(_geom, 4);
  index_t idx = 0;

  while (it.Valid()) {
    this->Cell(it) = data[idx++];
    it.Next();
  }
}

void Grid::WriteRightBoundary(real_t* data) {
  BoundaryIterator it(_geom, 2);
  index_t idx = 0;

  while (it.Valid()) {
    this->Cell(it) = data[idx++];
    it.Next();
  }
}

void Grid::WriteTopBoundary(real_t* data) {
  BoundaryIterator it(_geom, 3);
  index_t idx = 0;

  while (it.Valid()) {
    this->Cell(it) = data[idx++];
    it.Next();
  }
}

void Grid::WriteBottomBoundary(real_t* data) {
  BoundaryIterator it(_geom, 1);
  index_t idx = 0;

  while (it.Valid()) {
    this->Cell(it) = data[idx++];
    it.Next();
  }
}