#include "comm.hpp"

Communicator::Communicator (int* argc, char*** argv){
  //TODO
}
Communicator::~Communicator (){
  //TODO
}

const multi_index_t& Communicator::ThreadIdx () const{
  //TODO
  multi_index_t pos;
  pos[0] = 0;
  pos[1] = 0;
  return pos;
}
const multi_index_t& Communicator::ThreadDim () const{
  //TODO
  multi_index_t pos;
  pos[0] = 0;
  pos[1] = 0;
  return pos;
}

const int& Communicator::ThreadNum () const{
  //TODO
  return int(0);
}
const int& Communicator::ThreadCnt () const{
  //TODO
  return int(0);
}

const bool& Communicator::EvenOdd () const{
  //TODO
  return true;
}

real_t Communicator::gatherSum (const real_t& val) const{
  //TODO
  return real_t(0.0);
}
real_t Communicator::gatherMin (const real_t& val) const{
  //TODO
  return real_t(0.0);
}
real_t Communicator::gatherMax (const real_t& val) const{
  //TODO
  return real_t(0.0);
}

void Communicator::copyBoundary (Grid* grid) const{
  //TODO
}

const bool Communicator::isLeft () const{
  //TODO
  return true;
}
const bool Communicator::isRight () const{
  //TODO
  return true;
}
const bool Communicator::isTop () const{
  //TODO
  return true;
}
const bool Communicator::isBottom () const{
  //TODO
  return true;
}

bool Communicator::copyLeftBoundary (Grid* grid) const{
  //TODO
  return true;
}
bool Communicator::copyRightBoundary (Grid* grid) const{
  //TODO
  return true;
}
bool Communicator::copyTopBoundary (Grid* grid) const{
  //TODO
  return true;
}
bool Communicator::copyBottomBoundary (Grid* grid) const{
  //TODO
  return true;
}