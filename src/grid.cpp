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
  
  // Save shorthand variables for inverted mesh width
  _sh_im0 = 1.0 / _geom->Mesh()[0];
  _sh_im1 = 1.0 / _geom->Mesh()[1];

  // Init data with zeros
  this->Initialize(real_t(0.0));
}

Grid::Grid(const Geometry *geom, const multi_real_t &offset)
    : _geom(geom), _offset(offset) {
  
  // Calculate grid size and create data
  const multi_index_t size = _geom->Size();
  _data = new real_t[(size[0])*(size[1])];
  
  // Save shorthand variables for inverted mesh width
  _sh_im0 = 1.0 / _geom->Mesh()[0];
  _sh_im1 = 1.0 / _geom->Mesh()[1];
  
  // Init data with zeros
  this->Initialize(real_t(0.0));
}

Grid::~Grid(){
  delete[] _data;
}

void Grid::Initialize(const real_t &value) {
  // Init array with fixed value
  Iterator it(_geom);
  
  for (it.First();it.Valid();it.Next()){
    this->Cell(it) = value;
  }
}

real_t &Grid::Cell(const index_t &it) {
  return _data[it];
}

const real_t &Grid::Cell(const index_t &it) const {
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
  #ifndef USE_OPTIMIZATIONS
  return (this->Cell(it) - this->Cell(it.Left())) / _geom->Mesh()[0];
  #endif

  #ifdef USE_OPTIMIZATIONS
  return (_data[it] - _data[it.Left()]) * _sh_im0;
  #endif
}

real_t Grid::dx_r(const Iterator &it) const{
  #ifndef USE_OPTIMIZATIONS
  return (this->Cell(it.Right()) - this->Cell(it)) / _geom->Mesh()[0];
  #endif

  #ifdef USE_OPTIMIZATIONS
  return (_data[it.Right()] - _data[it]) * _sh_im0;
  #endif
}

real_t Grid::dy_l(const Iterator &it) const{
  #ifndef USE_OPTIMIZATIONS
  return (this->Cell(it) - this->Cell(it.Down())) / _geom->Mesh()[1];
  #endif

  #ifdef USE_OPTIMIZATIONS
  return (_data[it] - _data[it.Down()]) * _sh_im1;
  #endif
}

real_t Grid::dy_r(const Iterator &it) const{
  #ifndef USE_OPTIMIZATIONS
  return (this->Cell(it.Top()) - this->Cell(it)) / _geom->Mesh()[1];
  #endif

  #ifdef USE_OPTIMIZATIONS
  return (_data[it.Top()] - _data[it]) * _sh_im1;
  #endif
}

real_t Grid::dxx(const Iterator &it) const{
  #ifndef USE_OPTIMIZATIONS
  return (this->Cell(it.Right()) + this->Cell(it.Left()) - this->Cell(it) - this->Cell(it))
    / (_geom->Mesh()[0] * _geom->Mesh()[0]);
  #endif

  #ifdef USE_OPTIMIZATIONS
  return (_data[it.Right()] + _data[it.Left()] - _data[it] - _data[it]) * _sh_im0 * _sh_im0;
  #endif
}

real_t Grid::dyy(const Iterator &it) const{
  #ifndef USE_OPTIMIZATIONS
  return (this->Cell(it.Top()) + this->Cell(it.Down()) - this->Cell(it) - this->Cell(it))
    / (_geom->Mesh()[1] * _geom->Mesh()[1]);
  #endif

  #ifdef USE_OPTIMIZATIONS
  return (_data[it.Top()] + _data[it.Down()] - _data[it] - _data[it]) * _sh_im1 * _sh_im1;
  #endif
}

real_t Grid::DC_udu_x(const Iterator &it, const real_t &alpha) const {
  #ifndef USE_OPTIMIZATIONS
  real_t ft = pow((this->Cell(it) + this->Cell(it.Right())), 2.0)
    - pow((this->Cell(it.Left()) + this->Cell(it)), 2.0);
  real_t st = fabs(this->Cell(it) + this->Cell(it.Right())) * (this->Cell(it) - this->Cell(it.Right()))
    - fabs(this->Cell(it.Left()) + this->Cell(it)) * (this->Cell(it.Left()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[0];
  #endif

  #ifdef USE_OPTIMIZATIONS
  real_t ft = pow((_data[it] + _data[it.Right()]), 2.0)
    - pow(_data[it.Left()] + _data[it], 2.0);
  real_t st = fabs(_data[it] + _data[it.Right()]) * (_data[it] - _data[it.Right()])
    - fabs(_data[it.Left()] + _data[it]) * (_data[it.Left()] - _data[it]);
  return (0.25 * (ft + alpha * st)) * _sh_im0;
  #endif
}

real_t Grid::DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const {
  #ifndef USE_OPTIMIZATIONS
  real_t ft = (v->Cell(it) + v->Cell(it.Right())) * (this->Cell(it) + this->Cell(it.Top()))
    - (v->Cell(it.Down()) + v->Cell(it.Right().Down())) * (this->Cell(it.Down()) + this->Cell(it));
  real_t st = fabs(v->Cell(it) + v->Cell(it.Right())) * (this->Cell(it) - this->Cell(it.Top()))
    - fabs(v->Cell(it.Down()) + v->Cell(it.Right().Down())) * (this->Cell(it.Down()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[1];
  #endif

  #ifdef USE_OPTIMIZATIONS
  real_t ft = (v->Cell(it) + v->Cell(it.Right())) * (_data[it] + _data[it.Top()])
    - (v->Cell(it.Down()) + v->Cell(it.Right().Down())) * (_data[it.Down()] + _data[it]);
  real_t st = fabs(v->Cell(it) + v->Cell(it.Right())) * (_data[it] - _data[it.Top()])
    - fabs(v->Cell(it.Down()) + v->Cell(it.Right().Down())) * (_data[it.Down()] - _data[it]);
  return (0.25 * (ft + alpha * st)) * _sh_im1;
  #endif
}

real_t Grid::DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const {
  #ifndef USE_OPTIMIZATIONS
  real_t ft = (this->Cell(it) + this->Cell(it.Right())) * (u->Cell(it) + u->Cell(it.Top()))
    - (this->Cell(it.Left()) + this->Cell(it)) * (u->Cell(it.Left()) + u->Cell(it.Left().Top()));
  real_t st = fabs(u->Cell(it) + u->Cell(it.Top())) * (this->Cell(it) - this->Cell(it.Right()))
    - fabs(u->Cell(it.Left()) + u->Cell(it.Left().Top())) * (this->Cell(it.Left()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[0];
  #endif

  #ifdef USE_OPTIMIZATIONS
  real_t ft = (_data[it] + _data[it.Right()]) * (u->Cell(it) + u->Cell(it.Top()))
    - (_data[it.Left()] + _data[it]) * (u->Cell(it.Left()) + u->Cell(it.Left().Top()));
  real_t st = fabs(u->Cell(it) + u->Cell(it.Top())) * (_data[it] - _data[it.Right()])
    - fabs(u->Cell(it.Left()) + u->Cell(it.Left().Top())) * (_data[it.Left()] - _data[it]);
  return (0.25 * (ft + alpha * st)) * _sh_im0;
  #endif
}

real_t Grid::DC_vdv_y(const Iterator &it, const real_t &alpha) const {
  #ifndef USE_OPTIMIZATIONS
  real_t ft = pow((this->Cell(it) + this->Cell(it.Top())), 2.0)
    - pow((this->Cell(it.Down()) + this->Cell(it)), 2.0);
  real_t st = fabs(this->Cell(it) + this->Cell(it.Top())) * (this->Cell(it) - this->Cell(it.Top()))
    - fabs(this->Cell(it.Down()) + this->Cell(it)) * (this->Cell(it.Down()) - this->Cell(it));
  return (0.25 * (ft + alpha * st)) / _geom->Mesh()[1];
  #endif

  #ifdef USE_OPTIMIZATIONS
  real_t ft = pow(_data[it] + _data[it.Top()], 2.0)
    - pow(_data[it.Down()] + _data[it], 2.0);
  real_t st = fabs(_data[it] + _data[it.Top()]) * (_data[it] - _data[it.Top()])
    - fabs(_data[it.Down()] + _data[it]) * (_data[it.Down()] - _data[it]);
  return (0.25 * (ft + alpha * st)) * _sh_im1;
  #endif
}

real_t Grid::DC_dCu_x(const Iterator &it, const real_t &gamma, const Grid *u) const {
  #ifndef USE_OPTIMIZATIONS
  real_t ft = u->Cell(it) * 0.5 * (this->Cell(it.Right()) + this->Cell(it))
    - u->Cell(it.Left()) * 0.5 * (this->Cell(it) + this->Cell(it.Left()));
  real_t st = fabs(u->Cell(it)) * 0.5 * (this->Cell(it) - this->Cell(it.Right()))
    - fabs(u->Cell(it.Left())) * 0.5 * (this->Cell(it.Left()) - this->Cell(it));
  return (ft + gamma * st) / _geom->Mesh()[0];
  #endif

  #ifdef USE_OPTIMIZATIONS
  real_t ft = u->Cell(it) * 0.5 * (_data[it.Right()] + _data[it])
    - u->Cell(it.Left()) * 0.5 * (_data[it] + _data[it.Left()]);
  real_t st = fabs(u->Cell(it)) * 0.5 * (_data[it] - _data[it.Right()])
    - fabs(u->Cell(it.Left())) * 0.5 * (_data[it.Left()] - _data[it]);
  return (ft + gamma * st) * _sh_im0;
  #endif
}

real_t Grid::DC_dCv_y(const Iterator &it, const real_t &gamma, const Grid *v) const {
  #ifndef USE_OPTIMIZATIONS
  real_t ft = v->Cell(it) * 0.5 * (this->Cell(it.Top()) + this->Cell(it))
    - v->Cell(it.Down()) * 0.5 * (this->Cell(it) + this->Cell(it.Down()));
  real_t st = fabs(v->Cell(it)) * 0.5 * (this->Cell(it) - this->Cell(it.Top()))
    - fabs(v->Cell(it.Down())) * 0.5 * (this->Cell(it.Down()) - this->Cell(it));
  return (ft + gamma * st) / _geom->Mesh()[1];
  #endif

  #ifdef USE_OPTIMIZATIONS
  real_t ft = v->Cell(it) * 0.5 * (_data[it.Top()] + _data[it])
    - v->Cell(it.Down()) * 0.5 * (_data[it] + _data[it.Down()]);
  real_t st = fabs(v->Cell(it)) * 0.5 * (_data[it] - _data[it.Top()])
    - fabs(v->Cell(it.Down())) * 0.5 * (_data[it.Down()] - _data[it]);
  return (ft + gamma * st) * _sh_im1;
  #endif
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
    if((it % _geom->Size()[0]) == 0) printf("\n");
    printf("%6.2f", this->Cell(it));
  }
  printf("\n");
}