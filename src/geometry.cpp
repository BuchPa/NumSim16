#include "typedef.hpp"
#include "geometry.hpp"

/// Constructs a 128 x 128 driven cavity geometry:
// driven cavity with 128 x 128 grid, no-slip boundary conditions
Geometry::Geometry(){
  // Init number of cells in each dimension
  _size[0] = 8;
  _size[1] = 8;
  
  // Init length of driven cavity
  _length[0] = 1.0;
  _length[1] = 1.0;
  
  // Calculate cell width/height
  _h[0] = _length[0]/_size[0];
  _h[1] = _length[1]/_size[1];
  
  // Set _size to size INCL ghost layers
  _size[0] += 2;
  _size[1] += 2;
  
  // Set boundary values for upper boundary (u=1, v=0, p=0)
  _velocity[0] = 1.0;
  _velocity[1] = 0.0;
  
  // Unused in driven cavity problem
  _pressure    = 0.0;
}

/// Loads a geometry from a file
//  @param file File path as char array
void Geometry::Load(const char *file){
  //TODO
}

/// Returns the number of cells in each dimension
const multi_index_t &Geometry::Size() const {
  return _size;
}
/// Returns the length of the domain
const multi_real_t &Geometry::Mesh() const {
  return _h;
}
/// Returns the meshwidth
const multi_real_t &Geometry::Length() const{
  return _length;
}

/// Updates the velocity field u
//  @param  u The velocity field u in x direction
void Geometry::Update_U(Grid *u) const{
  //TODO
}
/// Updates the velocity field v
//  @param  v The velocity field v in y direction
void Geometry::Update_V(Grid *v) const{
  //TODO
}
/// Updates the pressure field p
//  @param  p The pressure field
void Geometry::Update_P(Grid *p) const{
  //TODO
}