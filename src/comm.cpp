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
  this->copyRightBoundary();
  MPI_Barrier(MPI_COMM_WORLD);
  this->copyTopBoundary();
  MPI_Barrier(MPI_COMM_WORLD);
  this->copyBottomBoundary();
}

bool Communicator::isLeft () const{
  return _tidx[0] == 0;
}

bool Communicator::isRight () const{
  return _tidx[0] == _tdim[0] - 1;
}

bool Communicator::isTop () const{
  return _tidx[1] == _tdim[1] - 1;
}

bool Communicator::isBottom () const{
  return _tidx[1] == 0;
}

bool Communicator::copyLeftBoundary (Grid* grid) const{
  real_t* buffer = grid->GetLeftBoundary();
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
      grid->WriteRightBoundary(buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  buffer = grid->GetLeftBoundary();

  if (!this->EvenOdd()) {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteRightBoundary(buffer);
    }
  }

  return true;
}

bool Communicator::copyRightBoundary (Grid* grid) const{
  real_t* buffer = grid->GetRightBoundary();
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteLeftBoundary(buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  buffer = grid->GetRightBoundary();

  if (!this->EvenOdd()) {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteLeftBoundary(buffer);
    }
  }

  return true;
}
bool Communicator::copyTopBoundary (Grid* grid) const{
  real_t* buffer = grid->GetTopBoundary();
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteBottomBoundary(buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  buffer = grid->GetTopBoundary();

  if (!this->EvenOdd()) {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteBottomBoundary(buffer);
    }
  }

  return true;
}

bool Communicator::copyBottomBoundary (Grid* grid) const{
  real_t* buffer = grid->GetBottomBoundary();
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteTopBoundary(buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  buffer = grid->GetBottomBoundary();

  if (!this->EvenOdd()) {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Send(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Recv(buffer, sizeof(buffer)/sizeof(*buffer), MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteTopBoundary(buffer);
    }
  }

  return true;
}

index_t GetLeftNeighbor() {
  return _tdim[0] * _tidx[1] + _tidx[0] - 1;
}

index_t GetRightNeighbor() {
  return _tdim[0] * _tidx[1] + _tidx[0] + 1;
}

index_t GetTopNeighbor() {
  return _tdim[0] * _tidx[1] + _tidx[0] + _tdim[0];
}

index_t GetBottomNeighbor() {
  return _tdim[0] * _tidx[1] + _tidx[0] - _tdim[0];
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