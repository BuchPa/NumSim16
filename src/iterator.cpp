#include "typedef.hpp"
#include "iterator.hpp"

#include "geometry.hpp"

/// Constructs a new Iterator depending on a geometry
// @param geom  Current geometry
Iterator::Iterator(const Geometry *geom)
    : _geom(geom){
  First();
}

/// Constructs a new Iterator on a geometry with a defined starting value
// @param geom  Current geometry
// @param value Starting value
Iterator::Iterator(const Geometry *geom, const index_t &value)
    : _geom(geom), _value(value), _valid(true){}
    
/// Returns the current position value
const index_t &Iterator::Value() const{
  return _value;
}

/// Cast operator to convert Iterators to integers
Iterator::operator const index_t &() const{
  return _value;
}
/// Returns the position coordinates
multi_index_t Iterator::Pos() const{
  multi_index_t pos;
  pos[0] = 0;
  pos[1] = 0;
  return pos; //TODO
}

/// Sets the iterator to the first element
void Iterator::First(){
  _value = 0;
  _valid = true;
  //TODO
}

/// Goes to the next element of the iterator, disables it if position is end
void Iterator::Next(){
  _value += 1;
  if(_value >= _geom->Size()[0]*_geom->Size()[1]){
    _valid = false;
  }
    
  //TODO
}

/// Checks if the iterator still has a valid value
bool Iterator::Valid() const{
  return _valid;
}

/// Returns an Iterator that is located left from this one.
// if we are at the left boundary, the cell sees itself
Iterator Iterator::Left() const{
  return *this; //TODO
}

/// Returns an Iterator that is located right from this one
// If we are at the right boundary, the cell sees itself
Iterator Iterator::Right() const{
  return *this; //TODO
}

/// Returns an Iterator that is located above this one
// If we are at the upper domain boundary, the cell sees itself
Iterator Iterator::Top() const{
  return *this; //TODO
}

/// Returns an Iterator that is located below this one
// If we are at the lower domain boundary, the cell sees itself
Iterator Iterator::Down() const{
  return *this; //TODO
}

/***************************************************************************
 *                            INTERIOR ITERATOR                            *
 ***************************************************************************/

/// Construct a new InteriorIterator
InteriorIterator::InteriorIterator(const Geometry *geom)
    : Iterator(geom){
  First();
}

/// Sets the iterator to the first element
void InteriorIterator::First(){
  //TODO
}
/// Goes to the next element of the iterator, disables it if position is end
void InteriorIterator::Next(){
  //TODO
}


/***************************************************************************
 *                            BOUNDARY ITERATOR                            *
 ***************************************************************************/

/// Constructs a new BoundaryIterator
BoundaryIterator::BoundaryIterator(const Geometry *geom)
    : Iterator(geom){
  First();
}

/// Sets the boundary to iterate
//  @param boundary Boundary to iterate
void BoundaryIterator::SetBoundary(const index_t &boundary){
  _boundary = boundary;
}

/// Sets the iterator to the first element
void BoundaryIterator::First(){
  //TODO
}

/// Goes to the next element of the iterator, disables it if position is end
void BoundaryIterator::Next(){
  //TODO
}