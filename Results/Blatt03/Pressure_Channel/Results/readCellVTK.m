% Read file from VTS
function [u,v,p] = readCellVTK(fname, nCells, format)
    [u,v,p] = read_vts(fname, nCells);
    
    % Transform to cell data, if required
    if strcmp(format,'cell')
        u = buildMean(u);
        v = buildMean(v);
        p = buildMean(p);
    end
end

% Transform vertex data to cell data (by using mean)
function cellMat = buildMean(vertMat)
    n_cell  = size(vertMat,1)-1;
    cellMat = zeros(n_cell, n_cell);
    
    for ii=1:n_cell
        for jj=1:n_cell
            cellMat(ii,jj) = mean(mean(vertMat(ii:ii+1, jj:jj+1)));
        end
    end
            
end