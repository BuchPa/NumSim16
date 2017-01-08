
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
/// A class to generate CSV files to write data from Grid at specific points
class CSV {
public:
  /// Constructs a CSV instance
  CSV(const real_t &re, list<multi_real_t> &pos);

  /// Initializes the file
  void Init(const char *path);
  /// Closes the file
  void Finish();

  /// Add a field of scalar values
  void AddEntry(const real_t &t, const Grid *u, const Grid *v, const Grid *p);
private:
  const real_t &_re;
  list<multi_real_t> &_pos;
  FILE *_handle;

  static uint32_t _cnt;
};
#endif // __CSV_HPP
