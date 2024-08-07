perf record --call-graph dwarf ./aigdd bug4.aag small.aag /home/froleyks/24_voiraig/fuzz/voiraig
perf script -F +pid > perf.fire
# https://profiler.firefox.com/
