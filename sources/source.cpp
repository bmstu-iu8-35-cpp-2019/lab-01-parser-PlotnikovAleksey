// Copyright 2019 Plotnikov Aleksey <alex280201@gmail.com>

#include <header.hpp>

std::string readFile(const std::string& fileName) {
  std::ifstream f(fileName);
  f.seekg(0, std::ios::end);
  size_t size = static_cast<size_t>(f.tellg());
  std::string s;
  s.resize(size);
  f.seekg(0);
  f.read(&s[0], size);
  return s;
}

Json::Json(const std::string& s) {
  size_t ind = 0;
  if (s[ind] == '{') {
    ind += 1;
    is_o = true;
    is_a = false;
    State state = find_key_or_end;
    std::string key;
    std::any value;
    std::unordered_map<std::string, std::any> _data;
    for (; ind < s.size(); ind++) {
      if (s[ind] == ' ' || s[ind] == '\t' || s[ind] == '\n') continue;
      if (s[ind] == '"') {
        ind += 1;
        if (state == find_key_or_end) {
          key = parse_string(s, ind);
          state = find_colon;
        } else if (state == find_value) {
          value = parse_string(s, ind);
          state = find_comma_or_end;
          _data[key] = value;
        } else {
          throw std::invalid_argument("wrong data format");
        }
        continue;
      }
      if (std::isdigit(s[ind])) {
        if (state == find_value) {
          value = parse_number(s, ind);
          state = find_comma_or_end;
          _data[key] = value;
          continue;
        } else {
          throw std::invalid_argument("wrong data format");
        }
      }
      if (s[ind] == '-') {
        if (state == find_value && std::isdigit(s[ind + 1])) {
          value = parse_number(s, ind);
          state = find_comma_or_end;
          _data[key] = value;
          continue;
        } else {
          throw std::invalid_argument("wrong data format");
        }
      }
      if (s[ind] == 't' || s[ind] == 'f') {
        if (state == find_value) {
          value = parse_bool(s, ind);
          state = find_comma_or_end;
          _data[key] = value;
          continue;
        } else {
          throw std::invalid_argument("wrong data format");
        }
      }
      if (s[ind] == 'n') {
        if (state == find_value) {
          parse_null(s, ind);
          std::any temp;
          state = find_comma_or_end;
          _data[key] = temp;
          continue;
        } else {
          throw std::invalid_argument("wrong data format");
        }
      }
      if (s[ind] == ':') {
        if (state == find_colon)
          state = find_value;
        else
          throw std::invalid_argument("something wrong");
        continue;
      }
      if (s[ind] == ',') {
        if (state == find_comma_or_end) {
          state = find_key_or_end;
          continue;
        } else {
          throw std::invalid_argument("wrong data format");
        }
      }
      if (s[ind] == '}') {
        if (state == find_comma_or_end || state == find_key_or_end)
          break;
        else
          throw std::invalid_argument("wrong data format");
      }
      if (s[ind] == '{') {
        if (state == find_value) {
          std::string sub = s.substr(ind);
          value = Json(sub);
          state = find_comma_or_end;
          _data[key] = value;
          brace_check(s, ind, true);
          continue;
        } else {
          throw std::invalid_argument("wrong data format");
        }
      }
      if (s[ind] == '[') {
        if (state == find_value) {
          std::string sub = s.substr(ind);
          value = Json(sub);
          state = find_comma_or_end;
          _data[key] = value;
          brace_check(s, ind, false);
          continue;
        } else {
          throw std::invalid_argument("wrong data format");
        }
      }
    }
    data = _data;
  } else if (s[ind] == '[') {
    ind++;
    is_a = true;
    is_o = false;
    data = parse_array(s, ind);
  } else {
    throw std::invalid_argument("not a Json format");
  }
}

bool Json::is_array() const { return is_a; }

bool Json::is_object() const { return is_o; }

std::any& Json::operator[](const std::string& key) {
  if (is_object())
    return std::any_cast<std::unordered_map<std::string, std::any>&>(data)[key];
  else
    throw std::invalid_argument("is not an object");
}

std::any& Json::operator[](int index) {
  if (is_array())
    return std::any_cast<std::vector<std::any>&>(data)[index];
  else
    throw std::invalid_argument("is not an array");
}

Json Json::parse(const std::string& s) { return Json(s); }

Json Json::parseFile(const std::string& path_to_file) {
  std::string s = readFile(path_to_file);
  return Json(s);
}

void Json::brace_check(const std::string& s, size_t& pos, bool is_usual) {
  if (is_usual) {
    int left_brace_counter = 1;
    int right_brace_counter = 0;
    while (left_brace_counter != right_brace_counter) {
      pos++;
      if (pos == s.size()) throw std::invalid_argument("wrong data format");
      if (s[pos] == '{') left_brace_counter++;
      if (s[pos] == '}') right_brace_counter++;
    }
  } else {
    int left_SQbrace_counter = 1;
    int right_SQbrace_counter = 0;
    while (left_SQbrace_counter != right_SQbrace_counter) {
      pos++;
      if (pos == s.size()) throw std::invalid_argument("wrong data format");
      if (s[pos] == '[') left_SQbrace_counter++;
      if (s[pos] == ']') right_SQbrace_counter++;
    }
  }
}

std::vector<std::any> Json::parse_array(const std::string& s, size_t& pos) {
  std::any value;
  State state = find_value;
  std::vector<std::any> _data;
  for (; pos < s.size(); pos++) {
    if (s[pos] == ' ' || s[pos] == '\t' || s[pos] == '\n') continue;
    if (std::isdigit(s[pos])) {
      if (state == find_value) {
        value = parse_number(s, pos);
        _data.push_back(value);
        state = find_comma_or_end;
        continue;
      } else {
        throw std::invalid_argument("wrong data format");
      }
    }
    if (s[pos] == '-') {
      if (state == find_value && std::isdigit(s[pos + 1])) {
        value = parse_number(s, pos);
        state = find_comma_or_end;
        _data.push_back(value);
        continue;
      } else {
        throw std::invalid_argument("wrong data format");
      }
    }
    if (s[pos] == 't' || s[pos] == 'f') {
      if (state == find_value) {
        value = parse_bool(s, pos);
        state = find_comma_or_end;
        _data.push_back(value);
        continue;
      } else {
        throw std::invalid_argument("wrong data format");
      }
    }
    if (s[pos] == 'n') {
      if (state == find_value) {
        parse_null(s, pos);
        std::any temp;
        state = find_comma_or_end;
        _data.push_back(temp);
        continue;
      } else {
        throw std::invalid_argument("wrong data format");
      }
    }
    if (s[pos] == ',') {
      if (state == find_comma_or_end) {
        state = find_value;
        continue;
      } else {
        throw std::invalid_argument("wrong data format");
      }
    }
    if (s[pos] == '[') {
      if (state == find_value) {
        std::string sub = s.substr(pos);
        value = Json(sub);
        state = find_comma_or_end;
        _data.push_back(value);
        brace_check(s, pos, false);
        continue;
      } else {
        throw std::invalid_argument("wrong data format");
      }
    }
    if (s[pos] == '{') {
      if (state == find_value) {
        std::string sub = s.substr(pos);
        value = Json(sub);
        state = find_comma_or_end;
        _data.push_back(value);
        brace_check(s, pos, true);
        continue;
      } else {
        throw std::invalid_argument("wrong data format");
      }
    }
    if (s[pos] == ']') {
      if (state == find_comma_or_end)
        break;
      else
        throw std::invalid_argument("wrong data format");
    }
  }
  return _data;
}

std::string Json::parse_string(const std::string& s, size_t& pos) {
  std::string res;
  while (s[pos] != '"') {
    res += s[pos];
    pos++;
  }
  return res;
}

double Json::parse_number(const std::string& s, size_t& pos) {
  double res;
  size_t len = 0;
  size_t _pos = pos;
  while (std::isdigit(s[pos]) || s[pos] == '.' || s[pos] == '-') {
    len++;
    pos++;
  }
  pos--;
  std::string sub = s.substr(_pos, len);
  res = stod(sub);
  return res;
}

void Json::parse_null(const std::string& s, size_t& pos) {
  if (s.substr(pos, 4) == "null")
    pos += 3;
  else
    throw std::invalid_argument("wrong data format");
}

bool Json::parse_bool(const std::string& s, size_t& pos) {
  bool res;
  if (s.substr(pos, 4) == "true") {
    pos += 3;
    res = true;
  } else if (s.substr(pos, 5) == "false") {
    pos += 4;
    res = false;
  } else {
    throw std::invalid_argument("wrong data format");
  }
  return res;
}

json parse_tickers(json def) {
  if (!(def.is_array())) throw std::invalid_argument("wrong input data");
  if (def.size() != 3) throw std::invalid_argument("wrong input data");
  if ((def[0].size() != def[1].size()) || (def[0].size() != def[2].size()) ||
      (def[1].size() != def[2].size()))
    throw std::invalid_argument("wrong input data");
  json res;
  for (size_t i = 0; i < def[0].size(); i++) {
    json temp;
    temp["description"] = def[2][i];
    temp["id"] = def[1][i];
    temp["ticker"] = def[0][i];
    res.push_back(temp);
  }
  return res;
}
