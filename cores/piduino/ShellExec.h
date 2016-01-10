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
  if(!result){
    //command failed for some reason
    Console.print("ERROR: ");
  }
  //print the buffer if any
  while(result.available())
    Console.write(result.read());
}

void setup(){
  ...
  Console.printf("System Time: %s", execCommand("date"));
}
*/

class ShellExec: public Stream {
  private:
    cbuf * buffer;
    bool   success;
    void setBuffer(const char *data, size_t len);
  public:
    operator bool(){ return success; }
    ShellExec(const char *cmd, size_t expected_len=4096);
    ShellExec(const char *cmd[], size_t expected_len=4096);
    ~ShellExec(){
      if(buffer != NULL) delete buffer;
    }
    int available(){
      return buffer->getSize();
    }
    int read(){
      return buffer->read();
    }
    int read(char * buf, size_t len){
      return buffer->read(buf, len);
    }
    int peek(){
      return buffer->peek();
    }
    void flush(){
      buffer->flush();
    }
};

#endif
