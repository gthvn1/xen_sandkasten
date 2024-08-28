// Ensure that xen_gntalloc is loaded (modprobe xen_gntalloc)
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xen/gntalloc.h>

int main(int argc, char *argv[]) {

  // we are expecting a domid as argument
  if (argc != 2) {
    printf("Error: <domid> is expected as argument\n");
    exit(1);
  }

  int domid = atoi(argv[1]);
  if (domid == 0) {
    printf("Error: wrong domain id");
    exit(1);
  }

  int page_size = sysconf(_SC_PAGESIZE);
  printf("Page size is %d\n", page_size);

  printf("press enter to continue...");
  fflush(stdout);
  getchar();
  printf("\n");

  int fd = open("/dev/xen/gntalloc", O_RDWR);
  if (fd < 0) {
    perror("main");
    exit(1);
  }

  struct ioctl_gntalloc_alloc_gref gnt_ref;
  gnt_ref.domid = domid;
  gnt_ref.count = 1; // We only want 1 page
  gnt_ref.flags = GNTALLOC_FLAG_WRITABLE;

  int err = ioctl(fd, IOCTL_GNTALLOC_ALLOC_GREF, &gnt_ref);
  if (err < 0) {
    printf("Error: IOCTL returned %d\n", err);
    goto failure;
  }

  char *shpages =
      mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shpages == MAP_FAILED) {
    perror("mmap");
    goto failure;
  }

  sprintf(shpages, "Hello, World!");

  printf("press enter to quit...");
  fflush(stdout);
  getchar();
  printf("\n");

  munmap(shpages, page_size);
  close(fd);
  return 0;

failure:
  close(fd);
  exit(1);
}
