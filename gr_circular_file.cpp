/*
* GNURadio Circular File adapted for testing purposes
*
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <utility>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <algorithm>
#include <string.h>

#define HAVE_MMAP

/*
   * writes input data into a circular buffer on disk.
   *
   * the file contains a fixed header:
   *   0x0000: int32 magic (0xEB021026)
   *   0x0004: int32 size in bytes of header (constant 4096)
   *   0x0008: int32 size in bytes of circular buffer (not including header)
   *   0x000C: int32 file offset to beginning of circular buffer
   *   0x0010: int32 byte offset from beginning of circular buffer to
   *                 current start of data
   */
class circular_file
{
private:
	int  d_fd;
	int	*d_header;
	unsigned char *d_buffer;
	int d_mapped_size;
	int d_bytes_read;

public:
	static const int HEADER_SIZE = 4096;
	static const int HEADER_MAGIC = 0xEB021026;

	static const int HD_MAGIC = 0;
  static const int HD_HEADER_SIZE = 1;	// integer offsets into header
  static const int HD_BUFFER_SIZE = 2;
  static const int HD_BUFFER_BASE = 3;
  static const int HD_BUFFER_CURRENT = 4;
  circular_file(const char *filename,
  	bool writable, int size)
  : d_fd(-1), d_header(0), d_buffer(0), d_mapped_size(0), d_bytes_read(0)
  {

  	int mm_prot;
  	if(writable) {
#ifdef HAVE_MMAP
  		mm_prot = PROT_READ | PROT_WRITE;
#endif
  		//std::cout << "filename " << filename << std::endl;
  		d_fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0664);
  		if(d_fd < 0) {
  			perror(filename);
  			exit(1);
  		}
#ifdef HAVE_MMAP	/* FIXME */
  		//std::cout << "file size truncated " << (size+HEADER_SIZE) << std::endl;
  		if(ftruncate(d_fd, size + HEADER_SIZE) != 0) {
  			perror(filename);
  			exit(1);
  		}
#endif
  	}
  	else {
#ifdef HAVE_MMAP
  		mm_prot = PROT_READ;
#endif
  		d_fd = open (filename, O_RDONLY);
  		if(d_fd < 0) {
  			perror(filename);
  			exit(1);
  		}
  	}

  	struct stat statbuf;
  	if(fstat (d_fd, &statbuf) < 0) {
  		perror(filename);
  		exit(1);
  	}

  	if(statbuf.st_size < HEADER_SIZE) {
  		fprintf(stderr, "%s: file too small to be circular buffer\n", filename);
  		exit(1);
  	}

  	d_mapped_size = statbuf.st_size;
#ifdef HAVE_MMAP
  	void *p = mmap (0, d_mapped_size, mm_prot, MAP_SHARED, d_fd, 0);
  	if(p == MAP_FAILED) {
  		perror("gr::circular_file: mmap failed");
  		exit(1);
  	}

  	d_header = (int*)p;
#else
  	perror("gr::circular_file: mmap unsupported by this system");
  	exit(1);
#endif

    if(writable) {    // init header

    	if(size < 0) {
    		fprintf(stderr, "gr::circular_buffer: size must be > 0 when writable\n");
    		exit(1);
    	}

    	d_header[HD_MAGIC] = HEADER_MAGIC;
    	d_header[HD_HEADER_SIZE] = HEADER_SIZE;
    	d_header[HD_BUFFER_SIZE] = size;
      d_header[HD_BUFFER_BASE] = HEADER_SIZE;    // right after header
      d_header[HD_BUFFER_CURRENT] = 0;
    }

    // sanity check (the asserts are a bit unforgiving...)

    assert(d_header[HD_MAGIC] == HEADER_MAGIC);
    assert(d_header[HD_HEADER_SIZE] == HEADER_SIZE);
    assert(d_header[HD_BUFFER_SIZE] > 0);
    assert(d_header[HD_BUFFER_BASE] >= d_header[HD_HEADER_SIZE]);
    assert(d_header[HD_BUFFER_BASE] + d_header[HD_BUFFER_SIZE] <= d_mapped_size);
    assert(d_header[HD_BUFFER_CURRENT] >= 0 &&
     d_header[HD_BUFFER_CURRENT] < d_header[HD_BUFFER_SIZE]);

    d_bytes_read = 0;
    d_buffer = (unsigned char*)d_header + d_header[HD_BUFFER_BASE];
  }
  ~circular_file()
  {
#ifdef HAVE_MMAP
   if(munmap ((char *) d_header, d_mapped_size) < 0) {
    perror("gr::circular_file: munmap");
    exit(1);
  }
#endif
  close(d_fd);
}
bool write(void *vdata, int nbytes)
{
	unsigned char *data = (unsigned char*)vdata;
	int buffer_size = d_header[HD_BUFFER_SIZE];
	int buffer_current = d_header[HD_BUFFER_CURRENT];

	while(nbytes > 0) {
		int n = std::min(nbytes, buffer_size - buffer_current);
		memcpy(d_buffer + buffer_current, data, n);
		buffer_current += n;
		if(buffer_current >= buffer_size)
			buffer_current = 0;

		data += n;
		nbytes -= n;
	}

	d_header[HD_BUFFER_CURRENT] = buffer_current;
	return true;
}

    // returns # of bytes actually read or 0 if end of buffer, or -1 on error.
int read(void *vdata, int nbytes)
{
	unsigned char *data = (unsigned char *) vdata;
	int buffer_current = d_header[HD_BUFFER_CURRENT];
	int buffer_size = d_header[HD_BUFFER_SIZE];
	int total = 0;

	nbytes = std::min(nbytes, buffer_size - d_bytes_read);
  //std::cout << "size of data requested " << nbytes << std::endl;
	while(nbytes > 0) {
		int offset = (buffer_current + d_bytes_read) % buffer_size;
		int n = std::min (nbytes, buffer_size - offset);
		memcpy(data, d_buffer + offset, n);
		data += n;
		d_bytes_read += n;
		total += n;
		nbytes -= n;
	}
	return total;
}
    // reset read pointer to beginning of buffer.
void reset_read_pointer()
{
	d_bytes_read = 0;
}
};


void consumer(circular_file& buffer, int ncons_items, std::future<void>&& fut, int delay){
  fut.wait();
  for (int i = 0; i < ncons_items; i++){
    double data;
    std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
    int value = buffer.read(&data, sizeof(data));
  }
}

void producer(circular_file& buffer, int nprod_items, std::promise<void>&& prom, int delay){
	for (int i = 0; i < nprod_items; i++){
		double data = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/5));
    std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
		buffer.write(&data, sizeof(data));
    if (i == ((nprod_items*3)/4)){
      prom.set_value();
    }
  }
}

int main (int argc, const char** argv){
  static const int BUFFER_SIZE = std::stoi(argv[1]);
  static const int PROD_DELAY = std::stoi(argv[2]);
  static const int CONSUME_DELAY = std::stoi(argv[3]);
  int buffer_capacity = BUFFER_SIZE*sizeof(double);

  static const char *test_file = "circular_file.data";
  circular_file *buffer;
  buffer = new circular_file(test_file, true, buffer_capacity);

  std::promise<void> data_ready;
  auto fut = data_ready.get_future();
  auto start = std::chrono::system_clock::now();
  std::thread p1([&]{
    producer(*buffer, BUFFER_SIZE, std::move(data_ready), PROD_DELAY);
  });
  std::thread c1([&]{
    consumer(*buffer, BUFFER_SIZE, std::move(fut), CONSUME_DELAY);
  });
  p1.join();
  c1.join();
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  std::cout << elapsed.count() << '\n';
  return 0;
}