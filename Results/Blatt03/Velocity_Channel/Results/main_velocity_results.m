%% Transforms different mesh sizes
clear all;
close all;
clc;

%% Parameters
pres = 0.1;
xLen = 5.0;
yLen = 1.0;

samples = {'20x4';
           '40x8';
           '80x16';
           '160x32';
           '320x64'};

fun_ref_p = @(y) pres*ones(size(y(:)));

n_sam_plot = [4,8,16,32,64];

folder = '../Velocity_VTK/';

%% Calculation
n_sam = numel(samples);
err_p_Linf = zeros(n_sam,1);
sol_p = cell(n_sam,1);
sol_y = cell(n_sam,1);

for ii=1:n_sam
    % Construct full path for sample
    path_sam = fullfile(folder, [samples{ii}, '.vts']);
    
    % Read VTK files
    [~, ~, sam_p] = readCellVTK(path_sam, n_sam_plot(ii)*[xLen/yLen,1]-[1,1],'vertex');
    
    % Choose left boundary
    sam_p = sam_p(:,1);
    
    % Save solution
    sol_p{ii} = sam_p;
    sol_y{ii} = linspace(0,yLen,n_sam_plot(ii)-1);
    
    % Calculate error
    ref_p = fun_ref_p(sol_y{ii});
    err_p_Linf(ii) = max(abs(sol_p{ii} - ref_p));
end

%% Plotting stuff
O_N = @(x,N) x.^N;
samples = ['Exact solution'; samples(:)];

%% Solution plot
close all;
hf_sol = figure();
ha_sol = axes(hf_sol);
% Plot exact solution
yy = linspace(0,yLen, 100);
plot(ha_sol, fun_ref_p(yy), yy);
% Plot experimental data
hold(ha_sol, 'on');
for ii=1:n_sam
    plot(ha_sol, sol_p{ii}, sol_y{ii});
end
hold(ha_sol, 'off');
title(ha_sol, 'Pressure p on left border for different mesh sizes');
xlabel(ha_sol, 'Pressure p(y)');
ylabel(ha_sol, 'y');
legend(ha_sol, samples{:});
print(hf_sol, 'plot_sol_p', '-dpng');

%% Error plot
hf_err = figure();
ha_err = axes(hf_err);
plot(ha_err, n_sam_plot, err_p_Linf);
title(ha_err, 'Error in pressure p on left border');
xlabel(ha_err, 'Number of cells in y direction');
ylabel(ha_err, 'L_{inf} error in p');
print(hf_err, 'plot_err_p', '-dpng');

