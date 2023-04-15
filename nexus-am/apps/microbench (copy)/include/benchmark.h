#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <am.h>
#include <klib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MB * 1024 * 1024
#define KB * 1024

#define true 1
#define false 0

#define REF_CPU    "i7-6700 @ 3.40GHz"
#define REF_SCORE  100000

#ifdef SETTING_TEST
  #define SETTING 0
#else
  #ifdef SETTING_REF
    #define SETTING 1
  #else
    #error "Must define SETTING_TEST or SETTING_REF"
  #endif
#endif

#define REPEAT  1

//                 size |  heap | time |  checksum   
#define   MD5_SM {     100,   1 KB,     0, 0xf902f28f}
#define   MD5_LG {10000000,  16 MB, 19593, 0x27286a42}

#define BENCHMARK_LIST(def) \
  def(  md5,   "md5",   MD5_SM,   MD5_LG, "MD5 digest") \

// Each benchmark will run REPEAT times

#define DECL(_name, _sname, _s1, _s2, _desc) \
  void bench_##_name##_prepare(); \
  void bench_##_name##_run(); \
  int bench_##_name##_validate();

BENCHMARK_LIST(DECL)

typedef struct Setting {
  int size;
  unsigned long mlim, ref;
  uint32_t checksum;
} Setting;

typedef struct Benchmark {
  void (*prepare)();
  void (*run)();
  int (*validate)();
  const char *name, *desc;
  Setting settings[2];
} Benchmark;

extern Benchmark *current;
extern Setting *setting;

typedef struct Result {
  int pass;
  unsigned long tsc, msec;
} Result;

void prepare(Result *res);
void done(Result *res);

// memory allocation
void* bench_alloc(size_t size);
void bench_free(void *ptr);
void bench_reset();

// random number generator
void bench_srand(int32_t seed);
int32_t bench_rand(); // return a random number between 0..32767

// checksum
uint32_t checksum(void *start, void *end);

#ifdef __cplusplus
}
#endif

#endif
