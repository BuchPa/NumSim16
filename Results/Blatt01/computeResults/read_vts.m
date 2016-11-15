function [ vel_u, vel_v, pres ] = read_vts( filename, verbose )
%READ_VTS Read VTS file of numsim project
%   Detailed explanation goes here

if nargin<2
    verbose = false;
end
    
[~,name,ext] = fileparts(filename);
if(verbose),fprintf('Read VTS file: %s.%s \n', name, ext); end

% Check if file is wavefront object
if ~strcmp(ext, '.vts'); error('read_vts.m is designed to read .vts files only!'); end

% Preallocate velocity data
vel=[]; start_read_velocity = false;

% Preallocate pressure data
pres=[]; start_read_pressure = false;

% Open file
fid = fopen(filename, 'r');

% Check if file exists
if fid < 0; error('VTS file does not exist'); end 
    
% Read obj-file line by line
cur_line = fgetl(fid);
while ischar(cur_line)
    if strcmp(cur_line, '<DataArray Name="Velocity" type="Float64" format="ascii" NumberOfComponents="3">')
        start_read_velocity = true;
        
    elseif strcmp(cur_line, '<DataArray Name="Pressure" type="Float64" format="ascii">')
        start_read_pressure = true;
        
    elseif start_read_velocity
        scan_line = textscan(cur_line, '%f %f %f', 'Delimiter', '\n');
        scan_rest = textscan(fid, '%f %f %f', 'Delimiter', '\n');
        vel       = [ scan_line{1}(:), scan_line{2}(:);
                     [scan_rest{1}(:), scan_rest{2}(:)]];
        start_read_velocity = false;
        
    elseif start_read_pressure
        scan_line = textscan(cur_line, '%f', 'Delimiter', '\n');
        scan_rest = textscan(fid, '%f', 'Delimiter', '\n');
        pres      = [scan_line{1}(:); scan_rest{1}(:)];
        start_read_pressure = false;
    end
    
    cur_line = fgetl(fid);
end

% Reshape arrays
n = sqrt(numel(pres));

vel_u = reshape(vel(:,1),n,n)';
vel_v = reshape(vel(:,2),n,n)';

pres  = reshape(pres,n,n)';
end