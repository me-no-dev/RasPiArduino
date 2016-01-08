/*
  ShellExec.cpp - Blocking shell command execution class for Raspberry Pi
  Copyright (c) 2016 Hristo Gochkov  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "Arduino.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

int shell_read(int fd, void *data, size_t len){
  return read(fd, data, len);
}

ShellExec::ShellExec(const char *cmd, size_t len){
  success = false;
  buffer =  new cbuf(0);
  int fd[2], childpid;
  int st;
  pipe(fd);
  if ((childpid = fork()) == -1){
    const char * ferr = "Fork Failed\r\n";
    delete buffer;
    buffer =  new cbuf(strlen(ferr));
    buffer->write(ferr, strlen(ferr));
    return;
  } else if( childpid == 0) {
     close(1);
     close(2);
     dup2(fd[1], 1);
     dup2(fd[1], 2);
     close(fd[0]);
     if(execl("/bin/sh","/bin/sh","-c",cmd,NULL) < 0){
       const char * eerr = "Exec Failed\r\n";
       delete buffer;
       buffer =  new cbuf(strlen(eerr));
       buffer->write(eerr, strlen(eerr));
     }
  } else {
    waitpid(childpid, &st, WUNTRACED | WCONTINUED);
    char result[len];
    int read_len;
    read_len = shell_read(fd[0], result, len);
    if (read_len > 0){
      delete buffer;
      buffer =  new cbuf(read_len);
      buffer->write(result, read_len);
    }
    success = st == 0;
  }
}
