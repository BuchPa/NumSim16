/*
 * Copyright (C) 2015   Malte Brunn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
// For debugging with printf
#include <stdio.h>
// For throwing puzzling errors at the simple-minded user i.e. make him fear c/c++
#include <stdexcept>
// For MPI_DOUBLE/INT
#include <mpich/mpi.h>
// For variable input args
#include <stdarg.h>

//------------------------------------------------------------------------------

#ifndef __TYPEDEF_HPP
#define __TYPEDEF_HPP

//------------------------------------------------------------------------------

#define DIM 2
#define REAL_TYPE double
#define INDEX_TYPE uint32_t
#define MPI_REAL_TYPE MPI_DOUBLE
#define MPI_INDEX_TYPE MPI_INT
//------------------------------------------------------------------------------

/// Typedef for reals
typedef REAL_TYPE real_t;

/// Typedef for integers
typedef INDEX_TYPE index_t;

/// Used in MPI communication to differentiate between various tasks
/// when calling the MPI functions. E.g. exchanging boundaries uses
/// a different tag gathering the extent of subdomains.
enum MPI_TAG : int {
  MPI_TAG_BOUNDARY, MPI_TAG_EXTENT, MPI_TAG_STREAM
};

/// Used for working with array with variable size. This avoids having to
/// recalculate the length of the array each time.
struct arrayBuffer {
  /// buffer real_t* The data values
  real_t* buffer;

  /// size int The size of the array
  int size;
};

//------------------------------------------------------------------------------

/// Template for array/vector types
template <typename _type, uint32_t _dim> struct array_t {
  // Constructors
  // Empty; initialize array_t to 0
  array_t() {
    for (uint32_t i = 0; i < _dim; ++i)
      x[i] = 0;
  }

  // Value; initialize array_to to val
  array_t(const _type &v1) {
    for (uint32_t i = 0; i < _dim; ++i)
      x[i] = v1;
  }

  // Value 2: initialize first value to v1, all others to v2
  array_t(const _type &v1, const _type &v2) {
    x[0] = v1;
    for (uint32_t i = 1; i < _dim; ++i)
      x[i] = v2;
  }

  // Copy-constructor: field of values
  array_t(const _type(&cp)[_dim]) {
    for (uint32_t i = 0; i < _dim; ++i)
      x[i] = cp[i];
  }
  // Copy-constructor: array_t
  array_t(const array_t<_type, _dim> &cp) {
    for (uint32_t i = 0; i < _dim; ++i)
      x[i] = cp.x[i];
  }

  // access operator
  _type &operator[](uint32_t i) { return x[i]; }
  const _type &operator[](uint32_t i) const { return x[i]; }

  // store values and size
  _type x[_dim];
  static const uint32_t dim = _dim;
};
/// Typedef for d-dimensional array of reals
typedef array_t<real_t, DIM> multi_real_t;
/// Typedef for d-dimensional array of integer
typedef array_t<index_t, DIM> multi_index_t;

//------------------------------------------------------------------------------

/// Forward declaration of classes used
class Iterator;
class Geometry;
class Grid;
class Parameter;
class Solver;
class RedOrBlackSOR;
class Compute;
class Communicator;

//------------------------------------------------------------------------------

/// Works like printf, except only the master process/thread will print to the
/// standard output.
/// 
/// @param format char* The format for printing (e.g. "Value: %d")
/// @return int The status number after calling printf. If the calling process
///   isn't the master, returns 1
inline int mprintf(const char *format, ...){
  
  // Check, if current process is master
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  // In case of master plot current info
  if(rank == 0){
    
    va_list arg; // Get variable input args
    int done;
    
    va_start(arg, format);
    done = vprintf(format, arg); // Call original vprintf
    va_end(arg);
   
    return done;
  }else{
    return 1;
  }
}

#endif // __TYPEDEF_HPP
