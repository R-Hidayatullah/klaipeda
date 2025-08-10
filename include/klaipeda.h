#if !defined(KLAIPEDA_H)
#define KLAIPEDA_H

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <zlib.h>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <map>
#include <sstream>
#include <thread>
#include <algorithm>

// Template function to read binary data into a given type
template <typename T>
void read(std::ifstream &file, T &data)
{
    file.read(reinterpret_cast<char *>(&data), sizeof(T));
}

// Function to read a string with a given length
void read(std::ifstream &file, std::string &data, uint16_t length)
{
    data.resize(length);
    file.read(&data[0], length);
}

// Specialization for std::vector<T>
template <typename T>
void read(std::ifstream &file, std::vector<T> &data, size_t count)
{
    data.resize(count);
    file.read(reinterpret_cast<char *>(data.data()), sizeof(T) * count);
}

// Specialization for std::array<T, N>
template <typename T, size_t N>
void read(std::ifstream &file, std::array<T, N> &data)
{
    file.read(reinterpret_cast<char *>(data.data()), sizeof(T) * N);
}

#endif // KLAIPEDA_H
