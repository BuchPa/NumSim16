#include "typedef.hpp"
#include "iterator.hpp"

#include "geometry.hpp"

Iterator::Iterator(const Geometry *geom, const index_t &value)
    : _geom(geom), _value(value), _itmax(_geom->Size()[0]*_geom->Size()[1]-1), _itmin(0){
  this->UpdateValid();
}

Iterator::Iterator(const Geometry *geom)
    : Iterator(geom, index_t(0)){}
    
const index_t &Iterator::Value() const{
  return _value;
}

Iterator::operator const index_t &() const{
  return _value;
}

multi_index_t Iterator::Pos() const{
  multi_index_t pos;
  pos[0] = _value % _geom->Size()[0];
  pos[1] = (int)(_value / _geom->Size()[0]);
  return pos;
}

void Iterator::First(){
  _value = _itmin;
  this->UpdateValid();
}

void Iterator::Next(){
  _value ++;
  this->UpdateValid();
}

bool Iterator::Valid() const{
  return _valid;
}

Iterator Iterator::Left() const{
  
  index_t pos;
  
  // Check, left border reached
  if(_value % _geom->Size()[0] == 0){
    pos = _value;
  }else{
    pos = _value-1;
  }
  
  Iterator it(_geom, pos);
  
  // Check valid, if not let the user know
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}

Iterator Iterator::Right() const{
  
  index_t pos;
  
  // Check, right border reached
  if((_value+1) % _geom->Size()[0] == 0){
    pos = _value;
  }else{
    pos = _value+1;
  }
  
  Iterator it(_geom, pos);
  
  // Check valid, if not let the user know
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}

Iterator Iterator::Top() const{
  
  index_t pos = _value + _geom->Size()[0];
  
  // Check, upper border reached
  if(pos / _geom->Size()[0] >= _geom->Size()[1]){
    pos = _value;
  }
  
  Iterator it(_geom, pos);
  
  // Check valid, if not let the user know
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}

Iterator Iterator::Down() const{
  
  index_t pos;
  
  // Check, lower border reached
  if(_value < _geom->Size()[0]){
    pos = _value;
  }else{
    pos = _value - _geom->Size()[0];
  }
  
  Iterator it(_geom, pos);
  
  // Check valid, if not reset to value
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}

void Iterator::UpdateValid(){
  if((_value <= _itmax)&&(_value >= _itmin)){
    _valid = true;
  }else{
    _valid = false;
  }
}

void Iterator::TestRun(const bool printNeighbours){
  if(printNeighbours){
    while(this->Valid()){
      this->printNeighbours();
      this->Next();
    }
  }else{
    while(this->Valid()){
      mprintf("%d: [%d,%d]\n", _value, this->Pos()[0], this->Pos()[1]);
      this->Next();
    }
  }
  mprintf("\n");
}

void Iterator::printNeighbours() const{
  Iterator l = this->Left();
  Iterator r = this->Right();
  Iterator t = this->Top();
  Iterator d = this->Down();
  mprintf("%d: [%d,%d], L_%d: [%d,%d], R_%d: [%d,%d], T_%d: [%d,%d], D_%d: [%d,%d]\n", 
    this->Value(), this->Pos()[0], this->Pos()[1],
    l.Value()   , l.Pos()[0]   , l.Pos()[1]   ,
    r.Value()   , r.Pos()[0]   , r.Pos()[1]   ,
    t.Value()   , t.Pos()[0]   , t.Pos()[1]   ,
    d.Value()   , d.Pos()[0]   , d.Pos()[1]   );
}

/***************************************************************************
 *                            INTERIOR ITERATOR                            *
 ***************************************************************************/

InteriorIterator::InteriorIterator(const Geometry *geom)
    : Iterator(geom){
  // Set itermax / itermin for InteriorIterator
  _itmax = (_geom->Size()[0]*(_geom->Size()[1]-1)-2);
  _itmin = (_geom->Size()[0]+1);
  // Set to first element
  this->First();
}

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

BoundaryIterator::BoundaryIterator(const Geometry *geom, const index_t &boundary)
    : Iterator(geom){
  this->SetBoundary(boundary);
}

void BoundaryIterator::SetBoundary(const index_t &boundary){
  _boundary = boundary;
  this->First();
}

void BoundaryIterator::First(){
  switch(_boundary){
    case 1:
      _itmin = 0;
      _itmax = _geom->Size()[0]-1;
      break;
    case 2:
      _itmin = _geom->Size()[0]-1;
      _itmax = _geom->Size()[0]*(_geom->Size()[1])-1;
      break;
    case 3:
      _itmin = _geom->Size()[0]*(_geom->Size()[1]-1);
      _itmax = _geom->Size()[0]*(_geom->Size()[1]  )-1;
      break;
    case 4:
      _itmin = 0;
      _itmax = _geom->Size()[0]*(_geom->Size()[1]-1);
      break;
    default:
      throw std::runtime_error(std::string("Failed to operate with current boundary value: " + std::to_string(_value)));
      break;
  }
  _value = _itmin;
  this->UpdateValid();
}

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

Iterator BoundaryIterator::CornerBottomLeft(){
  this->SetBoundary(1);
  index_t pos = _itmin;
  
  Iterator it(_geom, pos);
  
  // Check valid, if not let the user know
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}

Iterator BoundaryIterator::CornerBottomRight(){
  this->SetBoundary(1);
  index_t pos = _itmax;
  
  Iterator it(_geom, pos);
  
  // Check valid, if not let the user know
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}

Iterator BoundaryIterator::CornerTopLeft(){
  this->SetBoundary(3);
  index_t pos = _itmin;
  
  Iterator it(_geom, pos);
  
  // Check valid, if not let the user know
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}

Iterator BoundaryIterator::CornerTopRight(){
  this->SetBoundary(3);
  index_t pos = _itmax;
  
  Iterator it(_geom, pos);
  
  // Check valid, if not let the user know
  if(!it.Valid()){
    throw std::runtime_error(std::string("Invalid neighbour created: " + std::to_string(it.Value())));
  }
  
  return it;
}