/*
 * Executable a program without ASLR
 *
 * We actually only need the base address of the text section to stay the same,
 * and this theoretically can be accomplished with linker options to disable
 * position-independent code, but here we are using a large hammer to disable
 * ASLR altogether.
 *
 * Portions of the code are derived from util-linux, which are copyrighted by
 * Red Hat, Inc.
 *
 * Copyright (C) 2003-2007 Red Hat, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static inline void check_enough_args(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "usage: %s PROG [ARGS]\n"
            "Run the progam PROG with arguments ARGS without ASLR\n",
            argc ? argv[0] : "<program>");
    exit(1);
  }
}

static inline void could_not_exec(void) {
  fprintf(stderr, "could not spawn: %s\n", strerror(errno));
  exit(1);
}

/* Real implementation begins here. */

#ifdef __APPLE__
# include <spawn.h>
# ifndef _POSIX_SPAWN_DISABLE_ASLR
#  define _POSIX_SPAWN_DISABLE_ASLR 0x100
# endif

int main(int argc, char *argv[]) {
  check_enough_args(argc, argv);

  posix_spawnattr_t attrs;
  posix_spawnattr_init(&attrs);

  short ps_flags = 0;
  ps_flags |= POSIX_SPAWN_SETEXEC;
  ps_flags |= _POSIX_SPAWN_DISABLE_ASLR;
  int ret = posix_spawnattr_setflags(&attrs, ps_flags);

  if (ret) {
    fprintf(stderr, "could not set spawn flags: %s\n", strerror(errno));
    exit(1);
  }

  posix_spawnp(NULL, argv[1], NULL, &attrs, argv + 1, NULL);

  could_not_exec();

  return 1;
}

#else /* not __APPLE__ */

# ifdef __linux__
#  define _GNU_SOURCE
#  include <sys/personality.h>

#  ifndef HAVE_PERSONALITY
#   include <syscall.h>
#   define personality(pers) ((long)syscall(SYS_personality, pers))
#  endif

#  ifndef ADDR_NO_RANDOMIZE
#   define ADDR_NO_RANDOMIZE 0x0040000
#  endif

static inline int disable_aslr(void) {
  unsigned long pers_value = PER_LINUX | ADDR_NO_RANDOMIZE;

  if (personality(pers_value) < 0) {
    /*
     * Depending on architecture and kernel version, personality
     * syscall is either capable or incapable of returning an error.
     * If the return value is not an error, then it's the previous
     * personality value, which can be an arbitrary value
     * undistinguishable from an error value.
     * To make things clear, a second call is needed.
     */
    if (personality(pers_value) < 0)
      return 1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  check_enough_args(argc, argv);

  if (disable_aslr()) {
    fprintf(stderr, "could not set personality\n");
    exit(1);
  }

  execvp(argv[1], argv + 1);
  could_not_exec();
  return 1;
}

# else /* not __linux__ */
#  error "Unsupported operating system."
# endif
#endif
