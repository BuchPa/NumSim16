function [ err ] = transformMesh( ref, sam )
%TRANSFORMMESH Summary of this function goes here
%   Detailed explanation goes here
    
%     % Test case
%     nn  = 2;
%     ref = [ones(nn,1); 2*ones(nn,1); 3*ones(nn,1); 4*ones(nn,1)];
%     ref = repmat(ref, 1, 4*nn);
%     
%     sam = [1;2;3;4];
%     sam = repmat(sam, 1, 4);
    
    s_ref = size(ref);
    s_sam = size(sam);
    
    mean_size = s_ref(:) ./ s_sam(:);
    
    if min(floor(mean_size)~=mean_size)
        error('Use only for grids with matching size');
    end
    
    % Build mean value based on mean_size
    err = zeros(s_sam(1), s_sam(2));
    
    for ii=1:s_sam(1)
        for jj=1:s_sam(2)
%             fprintf('[%d,%d] [%d,%d]\n', (ii-1)*mean_size(1)+1, ii*mean_size(1), ...
%                                        (jj-1)*mean_size(2)+1, jj*mean_size(2));
            matching_part = ref( ((ii-1)*mean_size(1)+1):(ii*mean_size(1)), ...
                                 ((jj-1)*mean_size(2)+1):(jj*mean_size(2)));
            err(ii,jj) = errL1(matching_part, sam(ii,jj));
        end
    end
end

function err = errL1(ref,sam)
    err = abs(sum(sum(ref))/numel(ref) - sam);
end
