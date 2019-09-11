// Copyright 2019 Plotnikov Aleksey <alex280201@gmail.com>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <any>
#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

enum State { find_key_or_end, find_colon, find_value, find_comma_or_end };

std::string readFile(const std::string& fileName);

class Json {
 private:
  std::any data;
  bool is_a;
  bool is_o;

 public:
  explicit Json(const std::string& s);

  bool is_array() const;

  bool is_object() const;

  std::any& operator[](const std::string& key);

  std::any& operator[](int index);

  static Json parse(const std::string& s);

  static Json parseFile(const std::string& path_to_file);

 private:
  std::string parse_string(const std::string& s, size_t& pos);

  double parse_number(const std::string& s, size_t& pos);

  void parse_null(const std::string& s, size_t& pos);

  bool parse_bool(const std::string& s, size_t& pos);
};

#endif // INCLUDE_HEADER_HPP_
