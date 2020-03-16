#ifndef PROCESS_WRAPPER_DESCRIPTOR_HPP
#define PROCESS_WRAPPER_DESCRIPTOR_HPP

#include <unistd.h>

class Descriptor {
    static constexpr int FD_CLOSED = -1;
public:
    explicit Descriptor(int fd);
    Descriptor();

    Descriptor(Descriptor &&other) noexcept;

    Descriptor(const Descriptor &other) = delete;

    Descriptor &operator=(const Descriptor &other) = delete;

    Descriptor &operator=(Descriptor &&other) noexcept;

    size_t write(const void *data, size_t len);

    size_t read(void *data, size_t len);

    void bind(int fd);

    void writeExact(const void *data, size_t len);

    void readExact(void *data, size_t len);

    void close();

    ~Descriptor();

private:
    int fd_;
};

#endif //PROCESS_WRAPPER_DESCRIPTOR_HPP
