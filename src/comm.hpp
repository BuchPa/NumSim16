#include "grid.hpp"
#include "typedef.hpp"
//------------------------------------------------------------------------------
#ifndef __COMM_HPP
#define __COMM_HPP
//------------------------------------------------------------------------------
class Communicator {
public:
  Communicator (int* argc, char*** argv);
  ~Communicator ();
  
  const multi_index_t& ThreadIdx () const;
  const multi_index_t& ThreadDim () const;
  
  const int& ThreadNum () const;
  const int& ThreadCnt () const;
  
  const bool& EvenOdd () const;
  
  real_t gatherSum (const real_t& val) const;
  real_t gatherMin (const real_t& val) const;
  real_t gatherMax (const real_t& val) const;
  
  void copyBoundary (Grid* grid) const;

  bool isLeft () const;
  bool isRight () const;
  bool isTop () const;
  bool isBottom () const;
  
  bool isMaster() const;

  index_t** CollectExtent(index_t extent [4]) const;
  
  void CopyStreamX(Grid* grid) const;
  void CopyStreamY(Grid* grid) const;
  
private:
  multi_index_t _tidx;
  multi_index_t _tdim;
  int _rank;
  int _size;
  bool _evenodd;

  void SetDimensions();
  bool copyLeftBoundary (Grid* grid) const;
  bool copyRightBoundary (Grid* grid) const;
  bool copyTopBoundary (Grid* grid) const;
  bool copyBottomBoundary (Grid* grid) const;

  index_t GetLeftNeighbor() const;
  index_t GetRightNeighbor() const;
  index_t GetTopNeighbor() const;
  index_t GetBottomNeighbor() const;
};
//------------------------------------------------------------------------------
#endif // __COMM_HPP
//------------------------------------------------------------------------------
