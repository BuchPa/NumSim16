
#include "csv.hpp"

//------------------------------------------------------------------------------
uint32_t CSV::_cnt = 0;
//------------------------------------------------------------------------------
CSV::CSV(const real_t &re, list<multi_real_t> &pos)
    : _re(re), _pos(pos) {
  _handle = NULL;
}
//------------------------------------------------------------------------------
void CSV::Init(const char *path) {
  if (_handle)
    return;
  
  //Count positions
  index_t n_pos = _pos.size();
  
  //Set proper filename
  int flength = strlen(path) + 20;
  char *filename;
  filename = new char[flength];
  if (strlen(path))
    sprintf(filename, "%s_%02d.csv", path, n_pos);
  else
    sprintf(filename, "%s_%02d.csv", "multirun", n_pos);
  
  // Check, if file exists
  _handle = fopen(filename, "r");
  bool file_exists = false;
  if (_handle) {
    file_exists = true;
    fclose(_handle);
  }
  
  // Open in append mode
  _handle = fopen(filename, "a");
  delete[] filename;
  
  // Print headline, if file did not exist before
  if (!file_exists) {
    fprintf(_handle, "ID, RE, T");
    
    for (index_t i_pos=0; i_pos<n_pos;++i_pos) {
      fprintf(_handle, ", X%02d, Y%02d, U%02d, V%02d, P%02d", i_pos, i_pos, i_pos, i_pos, i_pos);
    }
    
    fprintf(_handle, "\n");
  }
}
//------------------------------------------------------------------------------
void CSV::Finish() {
  if (!_handle)
    return;

  fclose(_handle);

  _handle = NULL;
}
//------------------------------------------------------------------------------
void CSV::AddEntry(const real_t &t, const Grid *u, const Grid *v, const Grid *p) {
  if (!_handle)
    return;

  fprintf(_handle, "%d, %le, %le", _cnt, _re, t);
  
  for (list<multi_real_t>::iterator it_pos=_pos.begin(); it_pos!=_pos.end(); ++it_pos) {
    multi_real_t pos = (*it_pos);
    
    fprintf(_handle, ", %le, %le, %le, %le, %le", pos[0], pos[1], 
      (double)u->Interpolate(pos),
      (double)v->Interpolate(pos),
      (double)p->Interpolate(pos)
    );
  }

  fprintf(_handle, "\n");
  
  _cnt++;
}
//------------------------------------------------------------------------------