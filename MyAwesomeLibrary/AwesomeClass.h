#pragma once
#if defined _WIN64
#include <string>
#define AWESOMECLASS_IMPORT __declspec(dllimport)
#else
#define HELLOWORLD_IMPORT
#endif
AWESOMECLASS_IMPORT std::string PrettyPrint();
