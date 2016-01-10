/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <Process.h>

Process::~Process() {}

size_t Process::write(uint8_t c) {
  return 1;
}

void Process::flush() {}

int Process::available() {
  return 0;
}

int Process::read() {
  return -1;
}

int Process::peek() {
  return -1; // no chars available
}

void Process::begin(const String &command) {}

void Process::addParameter(const String &param) {}

void Process::runAsynchronously() {}

void Process::close() {}

boolean Process::running() {
  return false;
}

unsigned int Process::exitValue() {
  return 0;
}

unsigned int Process::run() {
  runAsynchronously();
  while (running())
    delay(100);
  return exitValue();
}

unsigned int Process::runShellCommand(const String &command) {
  runShellCommandAsynchronously(command);
  while (running())
    delay(100);
  return exitValue();
}

void Process::runShellCommandAsynchronously(const String &command) {
  begin("/bin/ash");
  addParameter("-c");
  addParameter(command);
  runAsynchronously();
}
