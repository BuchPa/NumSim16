#include "typedef.hpp"
#include "iterator.hpp"

#include "geometry.hpp"

Iterator::Iterator(const index_t &value, const index_t &xmax, const index_t &ymax)
    : _value(value), _xmax(xmax), _ymax(ymax), _itmax(xmax*ymax-1), _itmin(0){
  this->UpdateValid();
}

Iterator::Iterator(const Geometry *geom, const index_t &value)
    : _value(value), _xmax(geom->Size()[0]), _ymax(geom->Size()[1]), _itmax(geom->Size()[0]*geom->Size()[1]-1), _itmin(0){
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
  pos[0] = _value % _xmax;
  pos[1] = (int)(_value / _xmax);
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
  return Iterator(_value % _xmax == 0 ? _value : _value-1, _xmax, _ymax);
}

Iterator Iterator::Right() const{
  return Iterator((_value+1) % _xmax == 0 ? _value : _value+1, _xmax, _ymax);
}

Iterator Iterator::Top() const{
  index_t pos = _value + _xmax;
  return Iterator(pos / _xmax >= _ymax ? _value : pos, _xmax, _ymax);
}

Iterator Iterator::Down() const{
  return Iterator(_value < _xmax ? _value : _value - _xmax, _xmax, _ymax);
}

void Iterator::UpdateValid(){
  _valid = _value <= _itmax && _value >= _itmin;
}

void Iterator::TestRun(const bool printNeighbours){
  if(printNeighbours){
    while(this->Valid()){
      this->printNeighbours();
      this->Next();
    }
  }else{
    while(this->Valid()){
      printf("%d: [%d,%d]\n", _value, this->Pos()[0], this->Pos()[1]);
      this->Next();
    }
  }
  printf("\n");
}

void Iterator::printNeighbours() const{
  Iterator l = this->Left();
  Iterator r = this->Right();
  Iterator t = this->Top();
  Iterator d = this->Down();
  printf("%d: [%d,%d], L_%d: [%d,%d], R_%d: [%d,%d], T_%d: [%d,%d], D_%d: [%d,%d]\n", 
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
  _itmax = (_xmax*(_ymax-1)-2);
  _itmin = (_xmax+1);
  // Set to first element
  this->First();
}

void InteriorIterator::Next(){
  _value ++;
  if((_value + 1) % _xmax == 0){
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

const index_t &BoundaryIterator::Boundary() const{
  return _boundary;
}

void BoundaryIterator::First(){
  switch(_boundary){
    case 1:
      _itmin = 0;
      _itmax = _xmax-1;
      break;
    case 2:
      _itmin = _xmax-1;
      _itmax = _xmax*(_ymax)-1;
      break;
    case 3:
      _itmin = _xmax*(_ymax-1);
      _itmax = _xmax*(_ymax  )-1;
      break;
    case 4:
      _itmin = 0;
      _itmax = _xmax*(_ymax-1);
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
      _value += _xmax;
      break;
    case 3:
      _value ++;
      break;
    case 4:
      _value += _xmax;
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
  
  Iterator it(pos, _xmax, _ymax);
  
  return it;
}

Iterator BoundaryIterator::CornerBottomRight(){
  this->SetBoundary(1);
  index_t pos = _itmax;
  
  Iterator it(pos, _xmax, _ymax);
  
  return it;
}

Iterator BoundaryIterator::CornerTopLeft(){
  this->SetBoundary(3);
  index_t pos = _itmin;
  
  Iterator it(pos, _xmax, _ymax);
  
  return it;
}

Iterator BoundaryIterator::CornerTopRight(){
  this->SetBoundary(3);
  index_t pos = _itmax;
  
  Iterator it(pos, _xmax, _ymax);
  
  return it;
}

ObstacleIterator::ObstacleIterator(const Geometry *geom) : 
  InteriorIterator(geom), _cells(geom->GetCells()) {
  ObstacleIterator::First();
}

void ObstacleIterator::First() {
  InteriorIterator::First();
  if (_cells[_value] == CellType::Fluid)
    ObstacleIterator::Next();
}

void ObstacleIterator::Next() {
  InteriorIterator::Next();
  while (this->Valid()) {
    if (_cells[_value] == CellType::Fluid) {
      InteriorIterator::Next();
    } else {
      break;
    }
  }
}