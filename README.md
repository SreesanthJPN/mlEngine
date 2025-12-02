Cache-Optimized Parallel Matrix Multiplication Engine
Designed and implemented a tiled, cache-aware matrix-multiplication engine with a synchronized thread pool to maximize L1/L2/L3 locality and reduce memory bandwidth stalls. Achieved significant speedups by ensuring all worker threads cooperatively process the same matrix tile before advancing.
Currently developing a custom OS-level thread-scheduling mechanism to preserve tile locality across context switches for further performance gains.
