// open or create a file with pathname 'name' and return a File
// handle. The file is always opened with read/write access. If the
// open operation fails, the global 'fserror' is set to OPEN_FAILED,
// otherwise to NONE.
File open_file(char *name);

// close a 'file'. If the close operation fails, the global 'fserror'
// is set to CLOSE_FAILED, otherwise to NONE.
void close_file(File file);

// read at most 'num_bytes' bytes from 'file' into the buffer 'data',
// starting 'offset' bytes from the 'start' position. The starting
// position is BEGINNING_OF_FILE, CURRENT_POSITION, or END_OF_FILE. If
// the read operation fails, the global 'fserror' is set to
// READ_FAILED, otherwise to NONE.
unsigned long read_file_from(File file, void *data, unsigned long
num_bytes, SeekAnchor start, long offset);

// write 'num_bytes' to 'file' from the buffer 'data', starting
// 'offset' bytes from the 'start' position. The starting position
// is BEGINNING_OF_FILE, CURRENT_POSITION, or END_OF_FILE. If an
// attempt is made to modify a file such that "MZ" appears in the
// first two bytes of the file, the write operation fails and
// ILLEGAL_MZ is stored in the global 'fserror'. If the write fails
// for any other reason, the global 'fserror' is set to
// WRITE_FAILED, otherwise to NONE.
unsigned long write_file_at(File file, void *data, unsigned long
num_bytes, SeekAnchor start, long offset);
