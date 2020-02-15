#pragma once

#include <cstdio>

#ifndef NDEBUG
#include <assert.h>
#define NIMBLE_ASSERT(x) assert(x)
#define NIMBLE_LOG_INFO(...) ( printf("Nimble Logger [INFO]: ") , printf(__VA_ARGS__) )
#define NIMBLE_LOG_WARN(...) ( printf("Nimble Logger [WARN]: ") , printf(__VA_ARGS__) )
#define NIMBLE_LOG_ERROR(...) ( printf("Nimble Logger [ERROR]: ") , printf(__VA_ARGS__) , NIMBLE_ASSERT(false) )
#else
#define NIMBLE_ASSERT(x) do { (void)sizeof(x);} while (0)
#define NIMBLE_LOG_INFO(...) do { (void)sizeof(__VA_ARGS__);} while (0)
#define NIMBLE_LOG_WARN(...) do { (void)sizeof(__VA_ARGS__);} while (0)
#define NIMBLE_LOG_ERROR(...) do { (void)sizeof(__VA_ARGS__);} while (0)
#endif