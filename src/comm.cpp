#include "comm.hpp"
#include <mpich/mpi.h>

Communicator::Communicator (int* argc, char*** argv){
  // Initialize MPI and pass all parameters down to all threads
  MPI_Init(argc, argv);
  
  // Get the rank of the current thread
  MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
  // Get total number of threads
  MPI_Comm_size(MPI_COMM_WORLD, &_size);

  this->SetDimensions();
}

Communicator::~Communicator (){
  //TODO
}

const multi_index_t& Communicator::ThreadIdx () const{
  return _tidx;
}
const multi_index_t& Communicator::ThreadDim () const{
  return _tdim;
}

const int& Communicator::ThreadNum () const{
  return _rank;
}
const int& Communicator::ThreadCnt () const{
  return _size;
}

const bool& Communicator::EvenOdd () const{
  bool odd_dim1 = _tdim[0]%2;
  bool odd_dim2 = _tdim[1]%2;
  
  return odd_dim1 ^ odd_dim2;
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

void Communicator::SetDimensions() {
  switch (_size) {
    case 4:
      switch(_rank) {
        case 0:
          _tidx = {0,0};
          break;

        case 1:
          _tidx = {1,0};
          break;

        case 2:
          _tidx = {0,1};
          break;

        case 3:
          _tidx = {1,1};
          break;

        default:
          throw std::runtime_error(std::string("Invalid process number: " + std::to_string(_rank)));
      }
      break;

    case 2:  
      switch(_rank) {
        case 0:
          _tidx = {0,0};
          break;

        case 1:
          _tidx = {1,0};
          break;

        default:
          throw std::runtime_error(std::string("Invalid process number: " + std::to_string(_rank)));
      }
      break;

    default:
      throw std::runtime_error(std::string("Invalid number of processes: " + std::to_string(_size)));
  }
}