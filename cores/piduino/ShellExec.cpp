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
#include <stdarg.h>

int shell_read(int fd, void *data, size_t len){
  return read(fd, data, len);
}

int shell_write(int fd, void *data, size_t len){
  return write(fd, data, len);
}

void *_shell_exec_thread(void *arg){
  ShellExec *caller = (ShellExec*)arg;
  caller->execute();
  pthread_exit(NULL);
}

void ShellExec::setBuffer(const char *data, size_t len){
  cbuf *b = buffer;
  buffer =  new cbuf(len);
  if(b != NULL) delete b;
  buffer->write(data, len);
}

void ShellExec::execute(){
  executed = true;
  thread_running = true;
  success = false;
  pipe(pipes.in.pipes);
  pipe(pipes.out.pipes);
  pipe(pipes.err.pipes);
  exec_pid = fork();

  //fork failed
  if(exec_pid == -1){
    thread_running = false;
    return;
  }

  if(exec_pid == 0){
    close(0);
    close(1);
    close(2);
    dup2(pipes.in.read,   0);//stdin
    dup2(pipes.out.write, 1);//stdout
    dup2(pipes.err.write, 2);//stderr
    close(pipes.in.read);
    close(pipes.out.write);
    close(pipes.err.write);
    if(execvp((char *)(command[0]), (char * const *)command) < 0){
      const char * eerr = "Exec Failed\r\n";
      shell_write(pipes.err.write, (void *)eerr, strlen(eerr));
    }
  } else {
    waitpid(exec_pid, &exec_result, WUNTRACED | WCONTINUED);
    success = exec_result == 0;
    thread_running = false;
  }
}

void ShellExec::executeAsync(){
  if(thread_running)
    return;
  thread_running = pthread_create(&thread, NULL, _shell_exec_thread, (void*)this) == 0;
  if(!thread_running)
    return;
  pthread_setname_np(thread, "arduino-shell");
  pthread_detach(thread);
}

bool ShellExec::running(){
  if(thread_running)
    pthread_yield();
  return thread_running;
}

int ShellExec::available(){
  if(!executed) return -1;
  int count;
  ioctl(pipes.out.read, FIONREAD, &count);
  return count;
}

int ShellExec::read(){
  if(!executed || !available())
    return -1;
  char data;
  if(shell_read(pipes.out.read, &data, 1) == 1){
    return data;
  }
  return -1;
}

int ShellExec::read(char * buf, size_t len){
  if(!executed) return -1;
  int a = available();
  if(a<0 || !a) return -1;
  if((size_t)a < len) len = a;
  return shell_read(pipes.out.read, buf, len);
}

int ShellExec::errAvailable(){
  if(!executed) return -1;
  int count;
  ioctl(pipes.err.read, FIONREAD, &count);
  return count;
}

int ShellExec::errRead(){
  if(!executed || !errAvailable())
    return -1;
  char data;
  if(shell_read(pipes.err.read, &data, 1) == 1){
    return data;
  }
  return -1;
}

int ShellExec::errRead(char * buf, size_t len){
  if(!executed) return -1;
  int a = errAvailable();
  if(a<0 || !a) return -1;
  if((size_t)a < len) len = a;
  return shell_read(pipes.err.read, buf, len);
}

size_t ShellExec::write(uint8_t data){
  return shell_write(pipes.in.write, &data, 1);
}

size_t ShellExec::write(uint8_t *data, size_t len){
  return shell_write(pipes.in.write, data, len);
}

ShellExec::ShellExec(const char **cmd){
  buffer = NULL;
  success = false;
  executed = false;
  command = cmd;
  exec_pid = -1;
  exec_result = -1;
  thread = -1;
  thread_running = false;
}

ShellExec::ShellExec(const char *cmd){
  buffer = NULL;
  success = false;
  executed = false;
  command = {"sh", "-c", cmd};
  exec_pid = -1;
  exec_result = -1;
  thread = -1;
  thread_running = false;
}
