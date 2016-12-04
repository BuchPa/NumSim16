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

  bool odd_dim1 = _tidx[0]%2;
  bool odd_dim2 = _tidx[1]%2;
  
  _evenodd = odd_dim1 ^ odd_dim2;
}

Communicator::~Communicator (){
  MPI_Barrier(MPI_COMM_WORLD);
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
  return _evenodd;
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
  this->copyLeftBoundary(grid);
  MPI_Barrier(MPI_COMM_WORLD);
  this->copyRightBoundary(grid);
  MPI_Barrier(MPI_COMM_WORLD);
  this->copyTopBoundary(grid);
  MPI_Barrier(MPI_COMM_WORLD);
  this->copyBottomBoundary(grid);
  MPI_Barrier(MPI_COMM_WORLD);
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

bool Communicator::isMaster() const{
  return _rank == 0;
}

bool Communicator::copyLeftBoundary (Grid* grid) const{
  arrayBuffer aBuffer = grid->GetLeftBoundary(true);
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteRightBoundary(aBuffer.buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  aBuffer = grid->GetLeftBoundary(true);

  if (!this->EvenOdd()) {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteRightBoundary(aBuffer.buffer);
    }
  }

  return true;
}

bool Communicator::copyRightBoundary (Grid* grid) const{
  arrayBuffer aBuffer = grid->GetRightBoundary(true);
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteLeftBoundary(aBuffer.buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  aBuffer = grid->GetRightBoundary(true);

  if (!this->EvenOdd()) {
    if (!this->isRight()) {
      index_t rightNeighbor = this->GetRightNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, rightNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isLeft()) {
      index_t leftNeighbor = this->GetLeftNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, leftNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteLeftBoundary(aBuffer.buffer);
    }
  }

  return true;
}
bool Communicator::copyTopBoundary (Grid* grid) const{
  arrayBuffer aBuffer = grid->GetTopBoundary(true);
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteBottomBoundary(aBuffer.buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  aBuffer = grid->GetTopBoundary(true);

  if (!this->EvenOdd()) {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteBottomBoundary(aBuffer.buffer);
    }
  }

  return true;
}

bool Communicator::copyBottomBoundary (Grid* grid) const{
  arrayBuffer aBuffer = grid->GetBottomBoundary(true);
  MPI_Status stat;

  if (this->EvenOdd()) {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteTopBoundary(aBuffer.buffer);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  aBuffer = grid->GetBottomBoundary(true);

  if (!this->EvenOdd()) {
    if (!this->isBottom()) {
      index_t bottomNeighbor = this->GetBottomNeighbor();
      MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
    }
  } else {
    if (!this->isTop()) {
      index_t topNeighbor = this->GetTopNeighbor();
      MPI_Recv(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, topNeighbor, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, &stat);
      grid->WriteTopBoundary(aBuffer.buffer);
    }
  }

  return true;
}

index_t Communicator::GetLeftNeighbor() const {
  return _tdim[0] * _tidx[1] + _tidx[0] - 1;
}

index_t Communicator::GetRightNeighbor() const {
  return _tdim[0] * _tidx[1] + _tidx[0] + 1;
}

index_t Communicator::GetTopNeighbor() const {
  return _tdim[0] * _tidx[1] + _tidx[0] + _tdim[0];
}

index_t Communicator::GetBottomNeighbor() const {
  return _tdim[0] * _tidx[1] + _tidx[0] - _tdim[0];
}

void Communicator::SetDimensions() {
  switch (_size) {
    case 4:
      _tdim = {2, 2};
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
      _tdim = {2, 1};
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

    case 1:
      _tdim = {1, 1};
      switch(_rank) {
        case 0:
          _tidx = {0,0};
          break;

        default:
          throw std::runtime_error(std::string("Invalid process number: " + std::to_string(_rank)));
      }
      break;

    default:
      throw std::runtime_error(std::string("Invalid number of processes: " + std::to_string(_size)));
  }
}

index_t** Communicator::CollectExtent(index_t extent [4]) const{
  index_t** collected = NULL;
  MPI_Status stat;
  
  if (this->isMaster()) {
    // Create array to save values
    collected = new index_t*[_size];
    
    // Insert master values
    collected[0] = extent;
    
    // Insert other values
    for(int i_rank=1; i_rank < _size; ++i_rank){
      index_t buffer[4];
      MPI_Recv(buffer, 4, MPI_INDEX_TYPE, i_rank, MPI_TAG_EXTENT, MPI_COMM_WORLD, &stat);
      
      collected[i_rank] = new index_t[4];
      collected[i_rank][0] = buffer[0];
      collected[i_rank][1] = buffer[1];
      collected[i_rank][2] = buffer[2];
      collected[i_rank][3] = buffer[3];
    }
    
  } else {
    MPI_Send(extent, 4, MPI_INDEX_TYPE, 0, MPI_TAG_EXTENT, MPI_COMM_WORLD);
  }
  
  return collected;
}

void Communicator::CopyStreamX(Grid* grid) const{
  MPI_Status stat;
  
  // Get right Boundary
  real_t* bottomRightCorner = new real_t (grid->GetBottomRightCorner());
  real_t* incoming          = new real_t(0.0);
  
  if (!this->isLeft()) {
    // Get value from left neighbor
    index_t leftNeighbor = this->GetLeftNeighbor();
    MPI_Recv(incoming, 1, MPI_REAL_TYPE, leftNeighbor, MPI_TAG_STREAM, MPI_COMM_WORLD, &stat);
    
    // Add to own value
    bottomRightCorner[0] += incoming[0];
  }
  
  if (!this->isRight()) {
    // Send to right neighbor
    index_t rightNeighbor = this->GetRightNeighbor();
    MPI_Send(bottomRightCorner, 1, MPI_REAL_TYPE, rightNeighbor, MPI_TAG_STREAM, MPI_COMM_WORLD);
  }
  
  // Add to value to whole field after communucation
  if (!this->isLeft()){
    grid->AddValInXDir(*incoming);
  }
  
  delete incoming;
  delete bottomRightCorner;
}

void Communicator::CopyStreamY(Grid* grid) const{
  MPI_Status stat;
  
  // Get right Boundary
  arrayBuffer aBuffer = grid->GetTopBoundary(false);
  real_t* incoming    = new real_t[aBuffer.size];
  
  if (!this->isBottom()) {
    // Get value from left neighbor
    index_t bottomNeighbor = this->GetBottomNeighbor();
    MPI_Recv(incoming, aBuffer.size, MPI_REAL_TYPE, bottomNeighbor, MPI_TAG_STREAM, MPI_COMM_WORLD, &stat);
    
    // Add own right boundary
    for(int i=0; i<aBuffer.size; ++i){
      aBuffer.buffer[i] += incoming[i];
    }
  }
  
  if (!this->isTop()) {
    // Send to right neighbor
    index_t topNeighbor = this->GetTopNeighbor();
    MPI_Send(aBuffer.buffer, aBuffer.size, MPI_REAL_TYPE, topNeighbor, MPI_TAG_STREAM, MPI_COMM_WORLD);
  }
  
  // Add to value to whole field after communucation
  if (!this->isBottom()){
    grid->AddXArrayInYDir(incoming);
  }
  
  delete[] incoming;
  
}