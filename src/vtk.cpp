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

#include "vtk.hpp"
#include <cstring>
#include <cstdio>
//------------------------------------------------------------------------------
uint32_t VTK::_cnt = 0;
//------------------------------------------------------------------------------
VTK::VTK(const Geometry *geom, const Communicator *comm, const multi_real_t &offset)
    : _h(geom->Mesh()), _size(geom->Size()), _totsize(geom->TotalSize()), _comm(comm), _offset(offset){
  _handle = NULL;
  
  /// Parallel stuff
  _phandle = NULL;
  
  // Save extent / _extents
  _extents = geom->Extents();
  _extent  = geom->Extent();
  
  // Calculate cells to print from extent
  // Only for right border processes print right border cells, thus the size varies per process
  _iterXMax = _extent[1] - _extent[0];
  _iterYMax = _extent[3] - _extent[2];
}
//------------------------------------------------------------------------------
VTK::VTK(const Geometry *geom,  const Communicator *comm)
    : VTK(geom, comm, multi_real_t(0.0)){
}
//------------------------------------------------------------------------------
void VTK::Init(const char *path, const char *file) {
  if (_handle)
    return;
  int flength = strlen(path) + strlen(file) + 20;
  char *filename;
  filename = new char[flength];
  if (strlen(path))
    sprintf(filename, "%s%s_%i_%i.vts", path, file, _cnt, _comm->ThreadNum());
  else
    sprintf(filename, "%s_%i_%i.vts", "field", _cnt, _comm->ThreadNum());
  _handle = fopen(filename, "w");
  delete[] filename;

  fprintf(_handle, "<?xml version=\"1.0\"?>\n");
  fprintf(_handle, "<VTKFile type=\"StructuredGrid\">\n");
  fprintf(_handle, "<StructuredGrid WholeExtent=\"%i %i %i %i 0 0 \">\n",
          _extent[0], _extent[1], _extent[2], _extent[3]);
  fprintf(_handle, "<Piece Extent=\"%i %i %i %i 0 0 \">\n",
          _extent[0], _extent[1], _extent[2], _extent[3]);
  fprintf(_handle, "<Points>\n");
  fprintf(_handle, "<DataArray type=\"Float64\" format=\"ascii\" "
                   "NumberOfComponents=\"3\">\n");

  for (uint32_t z = 0; z <= 0; ++z)
    for (uint32_t y = _extent[2]; y <= _extent[3]; ++y)
      for (uint32_t x = _extent[0]; x <= _extent[1]; ++x)
        fprintf(_handle, "%le %le %le\n",
                  (double)x * _h[0] + _offset[0], (double)y * _h[1] + _offset[1], 0.0);

  fprintf(_handle, "</DataArray>\n");
  fprintf(_handle, "</Points>\n");
  fprintf(_handle, "<PointData>\n");
  
  // Write Parallel file
  if (_comm->isMaster()) {
    filename = new char[flength];
    if (strlen(path))
      sprintf(filename, "%s%s_%i.pvts", path, file, _cnt);
    else
      sprintf(filename, "%s_%i.pvts", "field", _cnt);
    _phandle = fopen(filename, "w");
    delete[] filename;
    
    fprintf(_phandle, "<?xml version=\"1.0\"?>\n");
    fprintf(_phandle, "<VTKFile type=\"PStructuredGrid\">\n");
    fprintf(_phandle, "<PStructuredGrid WholeExtent=\"0 %i 0 %i 0 0 \" GhostLevel=\"0\">\n",
            _totsize[0]-2, _totsize[1]-2);
    fprintf(_phandle, "<PPoints>\n");
    fprintf(_phandle, "<PDataArray type=\"Float64\" format=\"ascii\" "
                    "NumberOfComponents=\"3\" />\n");
    fprintf(_phandle, "</PPoints>\n");
    
    int numProc = 0;
    
    while (numProc < _comm->ThreadCnt()) {
      // Assemble current filename
      filename = new char[flength];
      if (strlen(path))
        sprintf(filename, "%s_%i_%i.vts", file, _cnt, numProc);
      else
        sprintf(filename, "%s_%i_%i.vts", "field", _cnt, numProc);
      
      // Print current piece
      fprintf(_phandle, "<Piece Extent=\"%i %i %i %i 0 0 \" Source=\"%s\" />\n",
              _extents[numProc][0], _extents[numProc][1],
              _extents[numProc][2], _extents[numProc][3],
              filename
             );
      
      // Continue
      delete[] filename;
      ++numProc;
    }
    
    fprintf(_phandle, "<PPointData>\n");
  }
}
//------------------------------------------------------------------------------
void VTK::Finish() {
  if (!_handle)
    return;

  fprintf(_handle, "</PointData>\n");
  fprintf(_handle, "</Piece>\n");
  fprintf(_handle, "</StructuredGrid>\n");
  fprintf(_handle, "</VTKFile>\n");

  fclose(_handle);

  _handle = NULL;
  
  // Write Parallel file
  if (_comm->isMaster()) {
    fprintf(_phandle, "</PPointData>\n");
    fprintf(_phandle, "</PStructuredGrid>\n");
    fprintf(_phandle, "</VTKFile>\n");
    
    fclose(_phandle);
    
    _phandle = NULL;
  }

  _cnt++;
}
//------------------------------------------------------------------------------
void VTK::AddScalar(const char *title, const Grid *grid) {
  if (!_handle)
    return;

  fprintf(_handle,
          "<DataArray Name=\"%s\" type=\"Float64\" format=\"ascii\">\n", title);

  multi_real_t pos;
  for (uint32_t z = 0; z <= 0; ++z) {
    for (uint32_t y = 0; y <= _iterYMax; ++y) {
      pos[1] = (double)y * _h[1] + _offset[1];
      for (uint32_t x = 0; x <= _iterXMax; ++x) {
        pos[0] = (double)x * _h[0] + _offset[0];
        fprintf(_handle, "%le ", (double)grid->Interpolate(pos));
      }
#if DIM == 2
      fprintf(_handle, "\n");
#endif // DIM
    }
  }

  fprintf(_handle, "</DataArray>\n");
  
  // Write Parallel file
  if (_comm->isMaster()) {
    fprintf(_phandle,
        "<PDataArray Name=\"%s\" type=\"Float64\" format=\"ascii\" />\n", title);
  }
}
//------------------------------------------------------------------------------
void VTK::AddField(const char *title, const Grid *v1, const Grid *v2) {
  if (!_handle)
    return;

  fprintf(_handle, "<DataArray Name=\"%s\" type=\"Float64\" format=\"ascii\" "
                   "NumberOfComponents=\"3\">\n",
          title);

  multi_real_t pos;
  
  for (uint32_t y = 0; y <= _iterYMax; ++y) {
    pos[1] = (double)y * _h[1] + _offset[1];
    for (uint32_t x = 0; x <= _iterXMax; ++x) {
      pos[0] = (double)x * _h[0] + _offset[0];
      fprintf(_handle, "%le %le %le\n", (double)v1->Interpolate(pos),
              (double)v2->Interpolate(pos), 0.0);
    }
  }

  fprintf(_handle, "</DataArray>\n");
  
  // Write Parallel file
  if (_comm->isMaster()) {
    fprintf(_phandle, "<PDataArray Name=\"%s\" type=\"Float64\" format=\"ascii\" "
                   "NumberOfComponents=\"3\" />\n",
            title);
  }
}
//------------------------------------------------------------------------------
void VTK::AddField(const char *title, const Grid *v1, const Grid *v2,
                   const Grid *v3) {
  if (!_handle)
    return;

  fprintf(_handle, "<DataArray Name=\"%s\" type=\"Float64\" format=\"ascii\" "
                   "NumberOfComponents=\"3\">\n",
          title);

  multi_real_t pos;
  for (uint32_t y = 0; y <= _iterYMax; ++y) {
    pos[1] = (double)y * _h[1] + _offset[1];
    for (uint32_t x = 0; x <= _iterXMax; ++x) {
      pos[0] = (double)x * _h[0] + _offset[0];
      fprintf(_handle, "%le %le %le\n", (double)v1->Interpolate(pos),
              (double)v2->Interpolate(pos), (double)v3->Interpolate(pos));
    }
  }

  fprintf(_handle, "</DataArray>\n");
  
  // Write Parallel file
  if (_comm->isMaster()) {
    fprintf(_phandle, "<PDataArray Name=\"%s\" type=\"Float64\" format=\"ascii\" "
                   "NumberOfComponents=\"3\" />\n",
            title);
  }
}
//------------------------------------------------------------------------------
