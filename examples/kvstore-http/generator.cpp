#include "data.hpp"
#include "generator_cli.hpp"
#include "kvstore/descriptor.hpp"
#include "kvstore/exceptions.hpp"
#include "kvstore/file.hpp"
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

File create_and_map(std::filesystem::path dest, size_t size) {
  Descriptor fd{open(dest.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600)};
  if (fd.get() == -1) {
    throw ErrnoException("unable to create new file or truncate existing file",
                         errno);
  }
  lseek(fd.get(), size + 1, SEEK_SET);
  write(fd.get(), "", 1);
  return {dest, size, File::Mode::READWRITE};
}

void fill_data_page(std::vector<Data> &page, Key key_begin, std::size_t step) {
  for (std::size_t i = 0; i < page.size(); ++i) {
    Key new_key = key_begin + i * step;
    page[i].key = new_key;
    uint8_t *data = page[i].data;
    std::fill(data, data + sizeof(Data::data), 'x');
  }
}

void generate_data(std::filesystem::path dest, size_t keys, size_t step) {
  size_t chunk_size = (4096 < keys) ? 4096 : keys;
  File data = create_and_map(dest, keys * sizeof(Data));
  std::vector<Data> data_page(chunk_size);

  size_t rest = keys % chunk_size;
  for (size_t i = 0; i < keys - rest; i += chunk_size) {
    fill_data_page(data_page, i * step, step);
    for (size_t j = 0; j < data_page.size(); ++j) {
      data.write_at<Data>(data_page[j], (i + j) * sizeof(Data));
    }
  }

  if (rest == 0) {
    return;
  }
  data_page.resize(rest);
  size_t rest_begin = keys - rest;
  fill_data_page(data_page, rest_begin * step, step);
  for (size_t j = 0; j < data_page.size(); ++j) {
    data.write_at<Data>(data_page[j], (rest_begin + j) * sizeof(Data));
  }
}

int main(int argc, char **argv) {
  GeneratorConfig cfg = parse_cli_opts(argc, argv);
  std::cout << "generating: " << cfg.keys_number << " keys in " << cfg.filename;
  generate_data(cfg.filename, cfg.keys_number, cfg.keys_step);
}