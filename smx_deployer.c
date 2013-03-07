// Simple program to execute a remote karaf clean
#include <errno.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

const char * PID_FILE = "/var/run/karaf.pid";
const char * KARAF = "/usr/share/apache-servicemix/bin/karaf";

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

pid_t
read_pid_from_file(const char *filename, int *err)
{
  pid_t pid;
  FILE * pidfile;

  if (access(filename, R_OK) != 0) {
    *err = 1;
    return 0;
  }

  pidfile = fopen(filename, "r");
  if (pidfile == NULL) {
    *err = 2;
    return 0;
  }

  if (fscanf(pidfile, "%d\n", &pid) != 1) {
    *err = 3;
  }
  fclose(pidfile);
  return pid;
}

void
write_pid_to_file(const char *filename, int *err)
{
  FILE * pidfile;

  pidfile = fopen(filename, "w");
  if (pidfile == NULL) {
    *err = 1;
    return;
  }
  fprintf(pidfile, "%d\n", getpid());
  fclose(pidfile);
}

int
main(int argc, char **argv)
{
  int err, retval;
  char * username;
  uid_t uid;
  pid_t running_process;

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

  running_process = read_pid_from_file(PID_FILE, &err);
  if (err > 1) {
    fprintf(stderr, "could not read from file %s\n", PID_FILE);
    exit(1);
  } else if (err == 0) {
    kill(running_process, 9);
    if (remove(PID_FILE) != 0) {
      fprintf(stderr, "could not remove pid file %s\n", PID_FILE);
      exit(1);
    }
  }

  write_pid_to_file(PID_FILE, &err);
  if (err == 1) {
    fprintf(stderr, "could not write pid to file %s\n", PID_FILE);
  }

  retval = setuid(uid);
  if (retval != 0) {
    fprintf(stderr, "couldn't change uid to user %s\n", username);
    exit(1);
  }

  if (access(KARAF, X_OK) == -1) {
	fprintf(stderr, "can't find executable %s\n", KARAF);
	exit(1);
  }

  char *args[] = {"clean", NULL};
  char *envp[] = {NULL};
  execve(KARAF, args, envp);
}
