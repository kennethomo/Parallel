/*
Fractal code for CS 4380 / CS 5351

Copyright (c) 2020 Texas State University. All rights reserved.

Redistribution in source or binary form, with or without modification,
is *not* permitted. Use in source and binary forms, with or without
modification, is only permitted for academic use in CS 4380 or CS 5351
at Texas State University.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: Martin Burtscher
*/

#include <cstdio>
#include <cmath>
#include <algorithm>
#include <sys/time.h>
#include "BMP43805351.h"

static void fractal(const int width, const int frames, unsigned char* const pic, const int threads)
{
  const double Delta = 0.01;
  const double xMid = 0.28017435;
  const double yMid = 0.60538436;

  #pragma omp parallel for default(none) shared(width,frames, pic) \
      private(frame) num_threads(threads) SCHEDULE
  // compute pixels of each frame
  for (int frame = 0; frame < frames; frame++) {  // frames
    double delta = Delta * pow(0.981,frame);//take care of loop dependency
    const double xMin = xMid - delta;
    const double yMin = yMid - delta;
    const double dw = 2.0 * delta / width;
    for (int row = 0; row < width; row++) {  // rows
      const double cy = yMin + row * dw;
      for (int col = 0; col < width; col++) {  // columns
        const double cx = xMin + col * dw;
        double x = cx;
        double y = cy;
        double x2, y2;
        int depth = 256;
        do {
          x2 = x * x;
          y2 = y * y;
          y = 2.0 * x * y + cy;
          x = x2 - y2 + cx;
          depth--;
        } while ((depth > 0) && ((x2 + y2) < 5.0));
        pic[frame * width * width + row * width + col] = (unsigned char)depth;
      }
    }
  }
}

int main(int argc, char *argv[])
{
  printf("Fractal v2.0\n");

  // check command line
  if (argc != 3) {fprintf(stderr, "USAGE: %s frame_width number_of_frames\n", argv[0]); exit(-1);}
  const int width = atoi(argv[1]);
  if (width < 10) {fprintf(stderr, "ERROR: frame_width must be at least 10\n"); exit(-1);}
  const int frames = atoi(argv[2]);
  if (frames < 1) {fprintf(stderr, "ERROR: number_of_frames must be at least 1\n"); exit(-1);}
  printf("width: %d\n", width);
  printf("frames: %d\n", frames);

  //thread count from cmd line
  const int threads = atoi(argv[2]);
  if (threads < 1) {fprintf(stderr, "ERROR: thread_count must be at least 1\n"); exit(-1);}
  printf("thread count: %d\n", threads);

  // allocate picture array
  unsigned char* pic = new unsigned char [frames * width * width];

  // start time
  timeval start, end;
  gettimeofday(&start, NULL);

  // execute timed code
  fractal(width, frames, pic, threads); //needs threads to be passed

  // end time
  gettimeofday(&end, NULL);
  const double runtime = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0;
  printf("compute time: %.5f s\n", runtime);

  // write result to BMP files
  if ((width <= 256) && (frames <= 60)) {
    for (int frame = 0; frame < frames; frame++) {
      BMP24 bmp(0, 0, width, width);
      for (int y = 0; y < width; y++) {
        for (int x = 0; x < width; x++) {
          const int p = pic[frame * width * width + y * width + x];
          const int blue = 128 + sin((p + 85) * 0.0245436926) * 127;
          const int green = 128 + sin((p + 0) * 0.0245436926) * 127;
          const int red = 128 + sin((p + 171) * 0.0245436926) * 127;
          bmp.dot(x, y, red * 0x010000 + green * 0x000100 + blue * 0x000001);
        }
      }
      char name[32];
      sprintf(name, "fractal%d.bmp", frame + 10000);
      bmp.save(name);
    }
  }

  // clean up
  delete [] pic;
  return 0;
}
