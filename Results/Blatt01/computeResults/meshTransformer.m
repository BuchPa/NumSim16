%% Transforms different mesh sizes
clear all;
close all;
clc;

%% Parameters
samples = {'2x2.vts';
           '4x4.vts';
           '8x8.vts';
           '16x16.vts';
           '32x32.vts';
           '64x64.vts';
           '128x128.vts';
           '256x256.vts'};
reference = '256x256.vts';

n_sam_plot = [2,4,8,16,32,64,128,256];

folder = 'differentMeshResults';

%% Calculation
n_sam = numel(samples);
err_u = zeros(n_sam,1);
err_v = zeros(n_sam,1);
err_p = zeros(n_sam,1);

% Construct full path for reference
path_ref = fullfile(folder, reference);

for ii=1:n_sam
    % Construct full path for sample
    path_sam = fullfile(folder, samples{ii});
    
    % Read VTK files
    [ref_u, ref_v, ref_p] = readCellVTK(path_ref);
    [sam_u, sam_v, sam_p] = readCellVTK(path_sam);
    
    % Calculate error
    errMat_u = transformMesh(ref_u, sam_u);
    errMat_v = transformMesh(ref_v, sam_v);
    errMat_p = transformMesh(ref_p, sam_p);
    
    % Mean error
    err_u(ii) = mean(mean(errMat_u));
    err_v(ii) = mean(mean(errMat_v));
    err_p(ii) = mean(mean(errMat_p));
end

%% Plotting stuff
O_N = @(x,N) x.^N;

%% Run-time plot
close all;
runtime = [107.1; 107.9; 108.0; 109.1; 113.7; 185.6; 709.8; 4686];

hf_t = figure;
ha_t = axes(hf_t);
loglog(ha_t, n_sam_plot, runtime, n_sam_plot, O_N(n_sam_plot, 3));
title(ha_t, 'Runtime in log-plot');
xlabel(ha_t, 'Number of cells in one dimension');
ylabel(ha_t, 'Runtime in seconds');
legend(ha_t, 'Experiment', 'Theory O(N^3)');
print(hf_t, 'plot_runtime', '-dpng');

%% Error plot u
hf_err_u = figure();
ha_err_u = axes(hf_err_u);
loglog(ha_err_u, n_sam_plot(1:end-1), err_u(1:end-1), n_sam_plot(1:end-1), O_N(n_sam_plot(1:end-1)+0.3, -2));
title(ha_err_u, 'Error in u in log-plot');
xlabel(ha_err_u, 'Number of cells in one dimension');
ylabel(ha_err_u, 'Error in u');
legend(ha_err_u, 'Experiment', 'Theory O(N^{-2})');
print(hf_err_u, 'plot_err_u', '-dpng');

%% Error plot v
hf_err_v = figure();
ha_err_v = axes(hf_err_v);
loglog(ha_err_v, n_sam_plot(1:end-1), err_v(1:end-1), n_sam_plot(1:end-1), O_N(n_sam_plot(1:end-1)+0.3, -2));
title(ha_err_v, 'Error in v in log-plot');
xlabel(ha_err_v, 'Number of cells in one dimension');
ylabel(ha_err_v, 'Error in v');
legend(ha_err_v, 'Experiment', 'Theory O(N^{-2})');
print(hf_err_v, 'plot_err_v', '-dpng');

%% Error plot p
hf_err_p = figure();
ha_err_p = axes(hf_err_p);
loglog(ha_err_p, n_sam_plot(1:end-1), err_p(1:end-1), n_sam_plot(1:end-1), O_N(n_sam_plot(1:end-1)+0.3, -2));
title(ha_err_p, 'Error in p in log-plot');
xlabel(ha_err_p, 'Number of cells in one dimension');
ylabel(ha_err_p, 'Error in p');
legend(ha_err_p, 'Experiment', 'Theory O(N^{-2})');
print(hf_err_p, 'plot_err_p', '-dpng');

