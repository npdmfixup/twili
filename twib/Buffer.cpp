#include "Buffer.hpp"

#include<algorithm>

namespace twili {
namespace util {

Buffer::Buffer() :
	data(2 * 1024 * 1024, 0) {
}

Buffer::Buffer(std::vector<uint8_t> data) :
	data(data), write_head(data.size()) {
}

Buffer::~Buffer() {
}

void Buffer::Write(const uint8_t *io, size_t size) {
	EnsureSpace(size);
	std::copy_n(io, size, data.begin() + write_head);
	write_head+= size;
}

void Buffer::Write(std::string &string) {
	Write((uint8_t*) string.data(), string.size());
}

std::tuple<uint8_t*, size_t> Buffer::Reserve(size_t size) {
	EnsureSpace(size);
	return std::make_tuple(data.data() + write_head, data.size() - write_head);
}

void Buffer::MarkWritten(size_t size) {
	write_head+= size;
}

bool Buffer::Read(uint8_t *io, size_t size) {
	if(read_head + size > write_head) {
		return false;
	}
	std::copy_n(data.begin() + read_head, size, io);
	read_head+= size;
	return true;
}

bool Buffer::Read(std::string &str, size_t len) {
	if(ReadAvailable() < len) {
		return false;
	}
	str = std::string(Read(), Read() + len);
	MarkRead(len);
	return true;
}

uint8_t *Buffer::Read() {
	return data.data() + read_head;
}

void Buffer::MarkRead(size_t size) {
	read_head+= size;
}

void Buffer::Clear() {
	read_head = 0;
	write_head = 0;
}

size_t Buffer::ReadAvailable() {
	return write_head - read_head;
}

void Buffer::EnsureSpace(size_t size) {
	if(write_head + size > data.size()) {
		Compact();
	}
	if(write_head + size > data.size()) {
		data.resize(write_head + size);
	}
}

void Buffer::Compact() {
	std::copy(data.begin() + read_head, data.end(), data.begin());
	write_head-= read_head;
	read_head = 0;
}

std::vector<uint8_t> Buffer::GetData() {
	return std::vector<uint8_t>(data.begin() + read_head, data.begin() + write_head);
}

} // namespace util
} // namespace twili
