#include <process/descriptor.hpp>
#include <stdexcept>
#include <cstring>
#include <iostream>

size_t Descriptor::write(const void *data, size_t len) {
    if (fd_ == FD_CLOSED) {
        throw std::runtime_error("write on closed descriptor");
    }
    ssize_t sent = ::write(fd_, data, len);
    if (sent == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
    return sent;
}

size_t Descriptor::read(void *data, size_t len) {
    if (fd_ == FD_CLOSED) {
        throw std::runtime_error("read on closed descriptor");
    }
    ssize_t got = ::read(fd_, data, len);
    if (got == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
    return got;
}

void Descriptor::writeExact(const void *data, size_t len) {
    size_t total = 0;
    while (total < len) {
        auto sent = this->write(static_cast<const char *>(data) + total, len - total);
        if (total != len && sent == 0) {
            throw std::runtime_error("EOF encountered in writeExact method");
        }
        total += sent;
    }
}

void Descriptor::readExact(void *data, size_t len) {
    size_t total = 0;
    while (total < len) {
        size_t got = this->read(static_cast<char *> (data) + total, len - total);
        if (total != len && got == 0) {
            throw std::runtime_error("EOF encountered in readExact method");
        }
        total += got;
    }
}

Descriptor::~Descriptor() {
    this->close();
}

void Descriptor::close() {
    if (fd_ != FD_CLOSED) {
        ::close(fd_);
        std::cout << "closing " << fd_ << '\n';
        fd_ = FD_CLOSED;

    }
}

Descriptor::Descriptor(int fd)
        : fd_{fd} {
    if (fd == FD_CLOSED) {
        throw std::runtime_error("invalid descriptor");
    }
}

Descriptor::Descriptor(Descriptor &&other) noexcept
        : fd_{other.fd_} {
    std::cout << "fd " << other.fd_ << '\n';
    other.fd_ = FD_CLOSED;
}

void Descriptor::bind(int new_fd) {
    if (fd_ == FD_CLOSED) {
        this->fd_ = new_fd;
        return;
    }
    if (dup2(fd_, new_fd) == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
    this->close();
    fd_ = new_fd;
}

Descriptor &Descriptor::operator=(Descriptor &&other) noexcept {
    this->close();
    this->fd_ = other.fd_;
    other.fd_ = FD_CLOSED;
    return *this;
}

Descriptor::Descriptor()
        : fd_{FD_CLOSED} {

}
