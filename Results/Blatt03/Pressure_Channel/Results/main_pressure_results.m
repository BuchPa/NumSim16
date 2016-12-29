%% Transforms different mesh sizes
clear all;
close all;
clc;

%% Parameters
pres = 0.1;
yLen = 1.0;
xLen = 5.0;
Re   = 1e4;

samples = {'20x4';
           '40x8';
           '80x16';
           '160x32';
           '320x64'};

scale = -0.5*Re*pres/xLen;
fun_ref_u = @(y) scale*y(:).*(y(:)-yLen);

n_sam_plot = [4,8,16,32, 64];

folder = '../Pressure_VTK/';

%% Calculation
n_sam = numel(samples);
err_u_Linf = zeros(n_sam,1);
sol_u = cell(n_sam,1);
sol_y = cell(n_sam,1);

for ii=1:n_sam
    % Construct full path for sample
    path_sam = fullfile(folder, [samples{ii}, '.vts']);
    
    % Read VTK files
    [sam_u, ~, ~] = readCellVTK(path_sam, n_sam_plot(ii)*[xLen/yLen,1]-[1,1],'vertex');
    
    % Choose left boundary
    sam_u = sam_u(:,1);
    
    % Save solution
    sol_u{ii} = sam_u;
    sol_y{ii} = linspace(0,yLen,n_sam_plot(ii)-1);
    
    % Calculate error
    ref_u = fun_ref_u(sol_y{ii});
    err_u_Linf(ii) = max(abs(sol_u{ii} - ref_u(:)));
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
plot(ha_sol, fun_ref_u(yy), yy);
% Plot experimental data
hold(ha_sol, 'on');
for ii=1:n_sam
    plot(ha_sol, sol_u{ii}, sol_y{ii});
end
hold(ha_sol, 'off');
title(ha_sol, 'Velocity u on left border for different mesh sizes');
xlabel(ha_sol, 'Velocity u(y)');
ylabel(ha_sol, 'y');
legend(ha_sol, samples{:});
print(hf_sol, 'plot_sol_u', '-dpng');

%% Solution plot
close(hf_sol);
hf_sol = figure();
ha_sol = axes(hf_sol);
% Plot experimental data
hold(ha_sol, 'on');
for ii=1:n_sam
    plot(ha_sol, sol_u{ii}, sol_y{ii});
end
hold(ha_sol, 'off');
title(ha_sol, 'Velocity u on left border for different mesh sizes');
xlabel(ha_sol, 'Velocity u(y)');
ylabel(ha_sol, 'y');
legend(ha_sol, samples{2:end});
print(hf_sol, 'plot_sol_u_wo_ref', '-dpng');

%% Error plot
hf_err = figure();
ha_err = axes(hf_err);
plot(ha_err, n_sam_plot, err_u_Linf);
title(ha_err, 'Error in velocity u on left border');
xlabel(ha_err, 'Number of cells in y direction');
ylabel(ha_err, 'L_{inf} error in u');
print(hf_err, 'plot_err_u', '-dpng');

