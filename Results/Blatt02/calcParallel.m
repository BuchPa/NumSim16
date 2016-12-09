%% Calculate 

numProc = [1,2,3,4,6,8];
runtime.proc1 = [1818659]; %129x129
runtime.proc2 = [991009];
runtime.proc3 = [681731];  %129x129
runtime.proc4 = [544818];
runtime.proc6 = [483022];  %132x132
runtime.proc8 = [406615];
grid = '128x128';
proc = 'Threads/Core: 2, Cores/Socket: 4, Sockets: 2';
f1 = 0.9;
f2 = 0.95;

% numProc = [1,2,3,4];
% runtime.proc1 = [11176, 10551];
% runtime.proc2 = [7557, 7327];
% runtime.proc3 = [9279, 9228];
% runtime.proc4 = [9802, 9660];
% grid = '32x32';
% proc = 'Threads/Core: 2, Cores/Socket: 2, Socket: 1';
% f = 0.9;

n_proc = numel(numProc);

T   = zeros(n_proc,1);
S   = zeros(n_proc,1);
St1 = zeros(n_proc,1);
St2 = zeros(n_proc,1);
E   = zeros(n_proc,1);
bad = zeros(n_proc,1);
opt = zeros(n_proc,1);

for p=1:n_proc
    T(p)   = mean(getfield(runtime, ['proc',num2str(numProc(p))]))*10^-3;
    S(p)  = T(1)/T(p);
    St1(p)  = T(1)/(T(1) * ((1-f1)+f1/p));
    St2(p)  = T(1)/(T(1) * ((1-f2)+f2/p));
    E(p)  = S(p)/numProc(p);
    bad(p) = 1/numProc(p);
    opt(p) = 1;
end

%% Plot runtime
hf = figure;
ha_ep = subplot(2,1,1,'Parent',hf);
plot(ha_ep, numProc, E, 'x-', numProc, bad, '--', numProc, opt, '-.', 'LineWidth', 3);
title(ha_ep, ['Grid: ',grid,' (or near) on ', proc]);
xlabel(ha_ep, 'Number of threads');
ylabel(ha_ep, 'Parallel Efficiency');
legend(ha_ep, 'Experiment', '"Worst case"', 'Strong scaling');

ha_sp = subplot(2,1,2,'Parent',hf);
plot(ha_sp, numProc, S, 'x-', numProc, St1, 'x--', numProc, St2, 'x:', 'LineWidth', 3);
xlabel(ha_sp, 'Number of threads');
ylabel(ha_sp, 'Speedup');
legend(ha_sp, 'Experimental Speedup', ['Theoretical Speedup, f=',num2str(f1)], ['Theoretical Speedup, f=',num2str(f2)]);

print(hf, ['plot_runtime_',grid], '-dpng');