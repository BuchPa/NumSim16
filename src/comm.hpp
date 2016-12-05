#include "grid.hpp"
#include "typedef.hpp"
//------------------------------------------------------------------------------
#ifndef __COMM_HPP
#define __COMM_HPP
//------------------------------------------------------------------------------
/// Used in parallel computation for exchanging data between subdomains and for
/// handling information about the division of the domain into subdomains by 
/// the rank of the process requesting the information.
class Communicator {
public:
  /// Constructs a Communicator instance, relaying the given console arguments
  /// to the MPI library.
  ///
  /// @param argc int* The number of console arguments
  /// @param argv char*** The argument values
  Communicator (int* argc, char*** argv);

  /// Deconstructs the instance and deletes the memory blocks created by it.
  ~Communicator ();
  
  /// Returns the x and y coordinates of the calling process within the
  /// overall grid of subdomains. E.g if the subdomains are numbered left-to-right
  /// and down-to-top, the first process would be {0, 0}, the second one {1, 0}
  /// and so on.
  ///
  /// @return multi_index_t& The coordinates of the calling process
  const multi_index_t& ThreadIdx () const;

  /// Returns the number of subdomains in x and y direction. The processes are
  /// numbered left-to-right and down-to-top in that order.
  ///
  /// @return multi_index_t& The number of subdomains in each direction
  const multi_index_t& ThreadDim () const;
  
  /// Returns the rank of the calling process/thread within a linearization of
  /// the subdomain coordinates, starting with 0, which corresponds to the
  /// left-most, lower-most subdomain.
  ///
  /// @return int& The rank of the calling process/thread
  const int& ThreadNum () const;

  /// Returns how many processes/threads there are used in the program.
  ///
  /// @return int& The number of processes/threads
  const int& ThreadCnt () const;
  
  /// Returns whether the calling process/thread is even or odd within the
  /// even-odd scheme used in the two-tier, synchronous boundary values
  /// exchange.
  ///
  /// @return bool& Whether the calling process/thread is even or not
  const bool& EvenOdd () const;
  
  /// Gathers the sum of values of the given parameter val across all calling
  /// processes.
  ///
  /// @param val real_t& The individual value to add to the sum
  /// @return real_t The overall sum of values
  real_t gatherSum (const real_t& val) const;

  /// Gathers the minimal value of the given parameter val across all calling
  /// processes.
  ///
  /// @param real_t& The individual value to compare against the others
  /// @return real_t The minimal value across all values
  real_t gatherMin (const real_t& val) const;

  /// Gathers the maximal value of the given parameter val across all calling
  /// processes.
  ///
  /// @param real_t& The individual value to compare against the others
  /// @return real_t The maximal value across all values
  real_t gatherMax (const real_t& val) const;
  
  /// Exchanges the boundary value of each subdomain with its neighbor subdomains.
  /// This method automatically decides which subdomain exchanges values with
  /// which others and employs an even-odd scheme, where the values are shared
  /// in two cycles to avoid processes waiting for each other in a deadlock.
  ///
  /// @param grid Grid* The grid containing the values to be exchanged
  void copyBoundary (Grid* grid) const;

  /// Returns if the calling process works on the subdomain which sits on the
  /// left boundary of the overall domain.
  ///
  /// @return bool If the subdomain sits on the left boundary of the overall
  ///   domain.
  bool isLeft () const;

  /// Returns if the calling process works on the subdomain which sits on the
  /// right boundary of the overall domain.
  ///
  /// @return bool If the subdomain sits on the right boundary of the overall
  ///   domain.
  bool isRight () const;

  /// Returns if the calling process works on the subdomain which sits on the
  /// top boundary of the overall domain.
  ///
  /// @return bool If the subdomain sits on the top boundary of the overall
  ///   domain.
  bool isTop () const;

  /// Returns if the calling process works on the subdomain which sits on the
  /// bottom boundary of the overall domain.
  ///
  /// @return bool If the subdomain sits on the bottom boundary of the overall
  ///   domain.
  bool isBottom () const;
  
  /// Returns if the calling process is the "master" process. Usually this
  /// process handles input/output that should only occur once across all
  /// processes and usually the process with rank 0 is the master process.
  bool isMaster() const;

  /// Collects the values of the extent for each subdomain into a single array.
  ///
  /// @param extent index_t[4] The individual extent values
  /// @return index_t** The array with all collected extent values
  index_t** CollectExtent(index_t extent [4]) const;
  
  /// Copies the stream function values in horizontal direction from subdomain
  /// to subdomain.
  ///
  /// @param grid Grid* The grid containing the stream function values
  void CopyStreamX(Grid* grid) const;

  /// Copies the stream function values in vertica direction from subdomain
  /// to subdomain.
  ///
  /// @param grid Grid* The grid containing the stream function values
  void CopyStreamY(Grid* grid) const;
  
private:
  /// _tidx multi_index_t The subdomain indizes
  /// @see Communicator::ThreadIdx()
  multi_index_t _tidx;

  /// _tdim multi_index_t The overall dimensions of subdomains
  /// @see Communicator::ThreadDim()
  multi_index_t _tdim;

  /// _rank int The rank of the calling process/thread
  /// @see Communicator::ThreadNum()
  int _rank;

  /// _size int The overall number of processes/threads
  /// @see Communicator::ThreadCnt()
  int _size;

  /// _evenodd bool The even/odd status for the copying scheme
  /// @see Communicator::EvenOdd()
  bool _evenodd;

  /// Needs to be called after the MPI functions for initialization have been
  /// called. Sets the subdomain indizes and dimensions, calculcated from the
  /// rank and size.
  void SetDimensions();

  /// Copies the left boundary of all subdomains to their left neighbor, except
  /// for the subdomains on the left boundary of the overall domain.
  ///
  /// @param grid Grid* The grid containing the values to be copied
  bool copyLeftBoundary (Grid* grid) const;

  /// Copies the right boundary of all subdomains to their right neighbor, except
  /// for the subdomains on the right boundary of the overall domain.
  ///
  /// @param grid Grid* The grid containing the values to be copied
  bool copyRightBoundary (Grid* grid) const;

  /// Copies the top boundary of all subdomains to their top neighbor, except
  /// for the subdomains on the top boundary of the overall domain.
  ///
  /// @param grid Grid* The grid containing the values to be copied
  bool copyTopBoundary (Grid* grid) const;

  /// Copies the bottom boundary of all subdomains to their bottom neighbor, except
  /// for the subdomains on the bottom boundary of the overall domain.
  ///
  /// @param grid Grid* The grid containing the values to be copied
  bool copyBottomBoundary (Grid* grid) const;

  /// Returns the rank of the process that occupies the subdomain to the
  /// left of the calling process. This method assumes the calling process
  /// isn't sitting on the left boundary of the overall domain.
  ///
  /// @return index_t The rank of the process to the left
  index_t GetLeftNeighbor() const;

  /// Returns the rank of the process that occupies the subdomain to the
  /// right of the calling process. This method assumes the calling process
  /// isn't sitting on the right boundary of the overall domain.
  ///
  /// @return index_t The rank of the process to the right
  index_t GetRightNeighbor() const;

  /// Returns the rank of the process that occupies the subdomain to the
  /// top of the calling process. This method assumes the calling process
  /// isn't sitting on the top boundary of the overall domain.
  ///
  /// @return index_t The rank of the process to the top
  index_t GetTopNeighbor() const;

  /// Returns the rank of the process that occupies the subdomain to the
  /// bottom of the calling process. This method assumes the calling process
  /// isn't sitting on the bottom boundary of the overall domain.
  ///
  /// @return index_t The rank of the process to the bottom
  index_t GetBottomNeighbor() const;
};
//------------------------------------------------------------------------------
#endif // __COMM_HPP
//------------------------------------------------------------------------------
