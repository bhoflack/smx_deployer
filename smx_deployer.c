// Simple program to execute a remote karaf clean
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

uid_t
uid_for_username(const char *name, int *err)
{
  struct passwd * passwd;

  passwd = getpwnam(name);
  if (passwd == NULL) {
    *err = 1;
    return 0;
  }

  return passwd->pw_uid;
}

int
main(int argc, char **argv)
{
  int err, retval;
  char * username;
  uid_t uid;

  if (argc < 2) {
    fprintf(stderr, "need username\n");
    exit(1);
  }

  username = argv[1];
  uid = uid_for_username(username, &err);

  if (err == 1) {
    fprintf(stderr, "username not found\n");
    exit(1);
  }

  retval = setuid(uid);
  if (retval != 0) {
    fprintf(stderr, "couldn't change uid to user %s\n", username);
    exit(1);
  }

  char *args[] = {"clean"};
  execve("/usr/bin/apache-servicemix/bin/karaf", args);
}
