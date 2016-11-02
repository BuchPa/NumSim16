#include "typedef.hpp"
#include "iterator.hpp"

#include "geometry.hpp"

/// Constructs a new Iterator on a geometry with a defined starting value
// @param geom  Current geometry
// @param value Starting value
Iterator::Iterator(const Geometry *geom, const index_t &value)
    : _geom(geom), _value(value), _itmax(_geom->Size()[0]*_geom->Size()[1]-1), _itmin(0){
  this->UpdateValid();
}

/// Constructs a new Iterator depending on a geometry
// @param geom  Current geometry
Iterator::Iterator(const Geometry *geom)
    : Iterator(geom, index_t(0)){}
    
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
  pos[0] = _value % _geom->Size()[0];
  pos[1] = (int)(_value / _geom->Size()[0]);
  return pos;
}

/// Sets the iterator to the first element
void Iterator::First(){
  _value = _itmin;
  this->UpdateValid();
}

/// Goes to the next element of the iterator, disables it if position is end
void Iterator::Next(){
  // Increment _value and check if valid
  _value ++;
  this->UpdateValid();
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

/// Checks if current value is valid
void Iterator::UpdateValid(){
  if((_value <= _itmax)&&(_value >= _itmin)){
    _valid = true;
  }else{
    _valid = false;
  }
}

/// Tests Iterator in a dummy run
void Iterator::TestRun(){
  while(this->Valid()){
//     printf("%d, ", _value);
    printf("%d: [%d,%d]\n", _value, this->Pos()[0], this->Pos()[1]);
    this->Next();
  }
  printf("\n");
}

/***************************************************************************
 *                            INTERIOR ITERATOR                            *
 ***************************************************************************/

/// Construct a new InteriorIterator
// @param geom  Current geometry
InteriorIterator::InteriorIterator(const Geometry *geom)
    : Iterator(geom){
  // Set itermax / itermin for InteriorIterator
  _itmax = (_geom->Size()[0]*(_geom->Size()[1]-1)-2);
  _itmin = (_geom->Size()[0]+1);
  // Set to first element
  this->First();
}

/// Goes to the next element of the iterator, disables it if position is end
void InteriorIterator::Next(){
  _value ++;
  if((_value + 1) % _geom->Size()[0] == 0){
    _value += 2;
  }
  this->UpdateValid();
}

/***************************************************************************
 *                            BOUNDARY ITERATOR                            *
 ***************************************************************************/

/// Constructs a new BoundaryIterator
//  @param geom  Current geometry
//  @param boundary Boundary to iterate
//  1: Border Bottom
//  2: Border Right
//  3: Border Top
//  4: Border Left
BoundaryIterator::BoundaryIterator(const Geometry *geom, const index_t &boundary)
    : Iterator(geom){
  this->SetBoundary(boundary);
}

/// Sets the boundary to iterate
//  @param boundary Boundary to iterate
//  1: Border Bottom
//  2: Border Right
//  3: Border Top
//  4: Border Left
void BoundaryIterator::SetBoundary(const index_t &boundary){
  _boundary = boundary;
  // Call First
  this->First();
}

/// Sets the iterator to the first element
void BoundaryIterator::First(){
  switch(_boundary){
    case 1:
      _itmin =   _geom->Size()[0]+1;
      _itmax = 2*_geom->Size()[0]-2;
      break;
    case 2:
      _itmin = 2*_geom->Size()[0]-2;
      _itmax =   _geom->Size()[0]*(_geom->Size()[1]-1)-2;
      break;
    case 3:
      _itmin =   _geom->Size()[0]*(_geom->Size()[1]-2)+1;
      _itmax =   _geom->Size()[0]*(_geom->Size()[1]-1)-2;
      break;
    case 4:
      _itmin =   _geom->Size()[0]+1;
      _itmax =   _geom->Size()[0]*(_geom->Size()[1]-2)+1;
      break;
    default:
      throw std::runtime_error(std::string("Failed to operate with current boundary value: " + std::to_string(_value)));
      break;
  }
  _value = _itmin;
  this->UpdateValid();
}

/// Goes to the next element of the iterator, disables it if position is end
void BoundaryIterator::Next(){
  switch(_boundary){
    case 1:
      _value ++;
      break;
    case 2:
      _value += _geom->Size()[0];
      break;
    case 3:
      _value ++;
      break;
    case 4:
      _value += _geom->Size()[0];
      break;
    default:
      throw std::runtime_error(std::string("Failed to operate with current boundary value: "+ std::to_string(_value)));
      break;
  }
  this->UpdateValid();
}