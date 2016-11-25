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
  
  //TODO Set _size / _bsize in geometry according to number of processes
}

Communicator::~Communicator (){
  MPI_Finalize();
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
  real_t sumval;
  MPI_Allreduce(&val, &sumval, 1, MPI_REAL_TYPE, MPI_SUM, MPI_COMM_WORLD);
  return sumval;
}
real_t Communicator::gatherMin (const real_t& val) const{
  real_t minval;
  MPI_Allreduce(&val, &minval, 1, MPI_REAL_TYPE, MPI_MIN, MPI_COMM_WORLD);
  return minval;
}
real_t Communicator::gatherMax (const real_t& val) const{
  real_t maxval;
  MPI_Allreduce(&val, &maxval, 1, MPI_REAL_TYPE, MPI_MAX, MPI_COMM_WORLD);
  return maxval;
}

void Communicator::copyBoundary (Grid* grid) const{
  this->copyLeftBoundary();
  MPI_Barrier(MPI_COMM_WORLD);
}

const bool Communicator::isLeft () const{
  return _tidx[0] == 0;
}
const bool Communicator::isRight () const{
  return _tidx[0] == _tdim[0] - 1;
}
const bool Communicator::isTop () const{
  return _tidx[1] == _tdim[1] - 1;
}
const bool Communicator::isBottom () const{
  return _tidx[1] == 0;
}

const bool Communicator::isMaster() const{
  return _rank == 0;
}

const void collect(Grid* fullgrid, Grid* partial) const{
  // Master collects
  if (this->isMaster()) {
    // MPI_Gather
  }else{
  // Rest sends
    
  }
}

bool Communicator::copyLeftBoundary (Grid* grid) const{
  real_t[] buffer = grid->GetLeftBoundary();
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteLeftBoundary(buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (!this->EvenOdd()) {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
    }
  }
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