// Ensure that xen_gntalloc is loaded (modprobe xen_gntalloc)
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xen/grant_table.h>
#include <xen/gntdev.h>

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

  int fd = open("/dev/xen/gntdev", O_RDWR);
  if (fd < 0) {
    perror("main");
    exit(1);
  }

  struct ioctl_gntdev_map_grant_ref gnt_ref;
  gnt_ref.count = 1;
  gnt_ref.refs[0].domid = domid;
  gnt_ref.refs[0].ref = 0;
  // struct ioctl_gntdev_map_grant_ref {
  //	/* IN parameters */
  //	/* The number of grants to be mapped. */
  //	__u32 count;
  //	__u32 pad;
  //	/* OUT parameters */
  //	/* The offset to be used on a subsequent call to mmap(). */
  //	__u64 index;
  //	/* Variable IN parameter. */
  //	/* Array of grant references, of size @count. */
  //	struct ioctl_gntdev_grant_ref refs[1];
  // };

  int err = ioctl(fd, IOCTL_GNTDEV_MAP_GRANT_REF, &gnt_ref);
  if (err < 0) {
    printf("Error: IOCTL returned %d\n", err);
    goto failure;
  }

  char *shbuf =
      mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shbuf == MAP_FAILED) {
    perror("mmap");
    goto failure;
  }

  printf("Read %s\n", shbuf);

  munmap(shbuf, page_size);
  close(fd);
  return 0;

failure:
  close(fd);
  exit(1);
}
