#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

void *MyThread(void *foo) {
  char *ptr = (char *)foo;
  for (int i = 0; i < 50; i++) {
    if (ptr[i] > 96 && ptr[i] < 123)
      ptr[i] = ptr[i] - 32;
    else if (ptr[i] > 64 && ptr[i] < 91)
      ptr[i] = ptr[i] + 32;
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int fd = open(argv[1], O_RDWR);
  if (fd == -1) {
    cout << "File could not open!" << endl;
    return 0;
  }
  struct stat sbuff;
  fstat(fd, &sbuff);
  int fsize = sbuff.st_size;
  pthread_t thread[2];

  char *map =
      (char *)mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    cout << "File could not be memory mapped!" << endl;
    return 0;
  }

  pthread_create(&thread[0], NULL, &MyThread, map);
  pthread_create(&thread[1], NULL, &MyThread, map + 50);

  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);
  cout << map << endl;
  msync(map, fsize, MS_SYNC);
  munmap(map, fsize);
  close(fd);

  return 0;
}