__kernel void render(__global uint *target, int offset) {
  // plot a pixel into the target array in GPU memory
  int thread_idx = get_global_id(0);
  int x = thread_idx % 640;
  int y = thread_idx / 640;
  int red = x / 3 + offset, green = y / 3;
  target[x + y * 640] = (red << 16) + (green << 8);
}