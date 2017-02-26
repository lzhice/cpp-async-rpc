#ifndef ASH_IO_ADAPTERS_H_
#define ASH_IO_ADAPTERS_H_

#include <cstddef>
#include <stdexcept>

namespace ash {

// Adapter classes for I/O.

// Input adapter base. Implementations should override at least one of
// read or getc and delegate the other. Throw exceptions for I/O errors.
class InputAdapter {
public:
	static constexpr int eof = -1;

	// Read up to l chars into the buffer pointed at by p. Return the
	// actual amount of bytes read, which could be fewer than l if hitting
	// EOF.
	virtual std::size_t read(char* p, std::size_t l) = 0;

	// Ensure that l chars are read, or throw.
	void readFully(char* p, std::size_t l) {
		auto r = read(p, l);
		if (r < l) {
			throw std::runtime_error("EOF before full read.");
		}
	}

	// Read one char out, or -1 on EOF.
	virtual int getc() = 0;

	virtual ~InputAdapter() {
	}
};

std::size_t InputAdapter::read(char* p, std::size_t l) {
	int c;
	std::size_t r = 0;
	while (r < l && (c = getc() != eof)) {
		r++;
		*p++ = c;
	}
	return r;
}

int InputAdapter::getc() {
	char c;
	if (read(&c, 1) < 1) {
		return eof;
	}
	return c;
}

// Output adapter base. Implementations should override at least one of
// write or putc and delegate the other. Throw exceptions for I/O errors.
class OutputAdapter {
public:
	// Write l chars out.
	virtual void write(const char* p, std::size_t l) = 0;

	// Write c out.
	virtual void putc(char c) = 0;

	virtual ~OutputAdapter() {
	}
};

void OutputAdapter::write(const char* p, std::size_t l) {
	while (l-- > 0) {
		putc(*p++);
	}
}

void OutputAdapter::putc(char c) {
	write(&c, 1);
}

// Output sizer.
class OutputSizer: public OutputAdapter {
public:
	// Get the total number of bytes written so far.
	std::size_t size() {
		return size_;
	}

	// Reset the byte count so that we can reuse the object.
	void reset() {
		size_ = 0;
	}

	// Write l chars out.
	void write(const char* p, std::size_t l) override {
		size_ += l;
	}

	// Write c out.
	void putc(char c) override {
		size_++;
	}

private:
	std::size_t size_ = 0;
};

}  // namespace ash

#endif /* ASH_IO_ADAPTERS_H_ */
