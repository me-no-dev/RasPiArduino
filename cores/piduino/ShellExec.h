/*
  ShellExec.h - Blocking shell command execution class for Raspberry Pi
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

#ifndef SHELL_EXEC_H
#define SHELL_EXEC_H

#include "cbuf.h"

/*
// Example:
void execCommand(const char* cmd){
  ShellExec result(cmd);
  result.execute();
  if(!result){
    //command failed for some reason
    Console.print("ERROR: ");
    while(result.errAvailable())
      Console.write(result.errRead());
  }
  //print the buffer if any
  while(result.available())
    Console.write(result.read());
}

void setup(){
  ...
  Console.print("System Time: ");
  execCommand("date");
}
*/

typedef const char ** cmd_array_t;

typedef union {
  int pipes[2];
  struct {
    int read;
    int write;
  };
} pipe_t;

typedef struct {
  pipe_t in;
  pipe_t out;
  pipe_t err;
} process_pipes_t;

class ShellExec: public Stream {
  private:
    cbuf *          buffer;
    bool            success;
    bool            executed;
    cmd_array_t     command;
    int             exec_pid;
    int             exec_result;
    process_pipes_t pipes;
    pthread_t       thread;
    bool            thread_running;

    void   setBuffer(const char *data, size_t len);
  public:
    ShellExec(const char *cmd);
    ShellExec(const char *cmd[]);
    ~ShellExec(){
      if(buffer != NULL) delete buffer;
    }
    operator bool(){ return success; }
    void execute();
    int result(){ return exec_result; }

    void executeAsync();
    bool running();

    int available();
    int read();
    int read(char * buf, size_t len);

    size_t write(uint8_t data);
    size_t write(uint8_t *data, size_t len);

    int peek(){ return -1; }
    void flush(){}

    int errAvailable();
    int errRead();
    int errRead(char * buf, size_t len);
};

#endif
