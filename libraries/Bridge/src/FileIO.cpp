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

#include <FileIO.h>

namespace BridgeLib {

File::File(BridgeClass &b) {}

File::File(const char *_filename, uint8_t _mode, BridgeClass &b) {}

File::operator bool() {
  return false;
}

File::~File() {
  close();
}

size_t File::write(uint8_t c) {
  return write(&c, 1);
}

size_t File::write(const uint8_t *buf, size_t size) {
  return size;
}

int File::read() {
  return -1; // no chars available
}

int File::peek() {
  return -1; // no chars available
}

boolean File::seek(uint32_t position) {
  return false;
}

uint32_t File::position() {
  return 0;
}

int File::available() {
  return 0;
}

void File::flush() {}

int File::read(void *buff, uint16_t nbyte) {
  return -1;
}

uint32_t File::size() {
  return 0;
}

void File::close() {}

const char *File::name() {
  return 0;
}

boolean File::isDirectory() {
  return 0;
}

File File::openNextFile(uint8_t mode) {
  return File();
}
void File::rewindDirectory(void) {}



boolean FileSystemClass::begin() {
  return true;
}

File FileSystemClass::open(const char *filename, uint8_t mode) {
  return File(filename, mode);
}

boolean FileSystemClass::exists(const char *filepath) {
  return false;
}

boolean FileSystemClass::mkdir(const char *filepath) {
  return false;
}

boolean FileSystemClass::remove(const char *filepath) {
  return false;
}

boolean FileSystemClass::rmdir(const char *filepath) {
  return false;
}

FileSystemClass FileSystem;

}
