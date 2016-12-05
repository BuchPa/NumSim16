%% Calculate 

numProc = [1,2,4];
runtime.proc1 = [11176, 10551];
runtime.proc2 = [7557, 7327];
runtime.proc4 = [9802, 9660];

n_proc = numel(numProc);

run = zeros(n_proc,1);

for ii=1:n_proc
    run(ii) = mean(getfield(runtime, ['proc',num2str(numProc(ii))]))*10^-3;
end

%% Plot runtime
hf_t = figure;
ha_t = axes(hf_t);
plot(ha_t, numProc, run);
title(ha_t, 'Runtime for 32x32 grid');
xlabel(ha_t, 'Number of threads');
ylabel(ha_t, 'Runtime in seconds');
legend(ha_t, 'Experiment');
print(hf_t, 'plot_runtime', '-dpng');