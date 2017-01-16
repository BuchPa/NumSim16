
#include "typedef.hpp"
#include "grid.hpp"
#include <cstring>
#include <cstdio>
//------------------------------------------------------------------------------
#ifndef __CSV_HPP
#define __CSV_HPP
//------------------------------------------------------------------------------
#if DIM != 2
#error CSV only usable with DIM = {2,3}
#endif // DIM

//------------------------------------------------------------------------------
/// A class to generate CSV files to write data from Grid at specific points.
class CSV {
public:
  /// Constructs a CSV instance.
  ///
  /// @param re real_t& The reynolds number
  /// @param pos list<multi_real_t>& A list of coordinates for the points measured
  CSV(const real_t &re, list<multi_real_t> &pos);

  /// Initializes the file.
  ///
  /// @param path char* The path to the file into which to write
  void Init(const char *path);

  /// Closes the file and finishes writing.
  void Finish();

  /// Add a field of scalar values to the current line.
  ///
  /// @param t real_t& The time
  /// @param u Grid* The grid containing the u velocity
  /// @param v Grid* The grid containing the v velocity
  /// @param p Grid* The grid containing the pressure
  void AddEntry(const real_t &t, const Grid *u, const Grid *v, const Grid *p);

private:
  /// @var re real_t& The Reynolds number
  const real_t &_re;

  /// @var pos list<multi_real_t>& The coordinates of points at which we measure
  list<multi_real_t> &_pos;

  /// @var _handle FILE* The file handle
  FILE *_handle;

  /// @var _cnt uint32_t The current line number
  static uint32_t _cnt;
};
#endif // __CSV_HPP
