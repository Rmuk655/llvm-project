// REQUIRES: amdgpu-registered-target
// RUN: %clang_cc1 -triple amdgcn-amd-amdhsa -fclangir -fclangir-call-conv-lowering -emit-cir %s -o %t.cir
// RUN: FileCheck --check-prefix=CIR --input-file=%t.cir %s
// RUN: %clang_cc1 -triple amdgcn-amd-amdhsa -fclangir -fclangir-call-conv-lowering -emit-llvm %s -o %t-cir.ll
// RUN: FileCheck --check-prefix=LLVM --input-file=%t-cir.ll %s
// RUN: %clang_cc1 -triple amdgcn-amd-amdhsa -emit-llvm %s -o %t.ll
// RUN: FileCheck --check-prefix=LLVM --input-file=%t.ll %s

typedef struct {} empty_t;
typedef struct { char c; } i8_wrapper_t;
typedef struct { int i; } i32_wrapper_t;
typedef struct { float f; } f32_wrapper_t;
typedef struct { char a, b; } char2_t;
typedef struct { char a, b, c; } small_t;
typedef short short4 __attribute__((ext_vector_type(4)));

void arg_void(void) {}

// CIR: cir.func {{.*}}@arg_void()
// LLVM: define {{.*}}void @arg_void()

void arg_bool(_Bool b) {}

// CIR: cir.func {{.*}}@arg_bool(%arg0: !cir.bool {{.*}}llvm.zeroext
// LLVM: define {{.*}}void @arg_bool(i1 noundef zeroext %{{.*}})

void arg_char(char c) {}

// CIR: cir.func {{.*}}@arg_char(%arg0: !s8i {{.*}}llvm.signext
// LLVM: define {{.*}}void @arg_char(i8 noundef signext %{{.*}})

void arg_uchar(unsigned char c) {}

// CIR: cir.func {{.*}}@arg_uchar(%arg0: !u8i {{.*}}llvm.zeroext
// LLVM: define {{.*}}void @arg_uchar(i8 noundef zeroext %{{.*}})

void arg_short(short s) {}

// CIR: cir.func {{.*}}@arg_short(%arg0: !s16i {{.*}}llvm.signext
// LLVM: define {{.*}}void @arg_short(i16 noundef signext %{{.*}})

void arg_int(int i) {}

// CIR: cir.func {{.*}}@arg_int(%arg0: !s32i {{.*}})
// LLVM: define {{.*}}void @arg_int(i32 noundef %{{.*}})

void arg_long(long l) {}

// CIR: cir.func {{.*}}@arg_long(%arg0: !s64i {{.*}})
// LLVM: define {{.*}}void @arg_long(i64 noundef %{{.*}})

void arg_float(float f) {}

// CIR: cir.func {{.*}}@arg_float(%arg0: !cir.float {{.*}})
// LLVM: define {{.*}}void @arg_float(float noundef %{{.*}})

void arg_double(double d) {}

// CIR: cir.func {{.*}}@arg_double(%arg0: !cir.double {{.*}})
// LLVM: define {{.*}}void @arg_double(double noundef %{{.*}})

void arg_ptr(int *p) {}

// CIR: cir.func {{.*}}@arg_ptr(%arg0: !cir.ptr<!s32i> {{.*}})
// LLVM: define {{.*}}void @arg_ptr(ptr noundef %{{.*}})

void arg_bitint65(_BitInt(65) i) {}

// CIR: cir.func {{.*}}@arg_bitint65(%arg0: !cir.int<s, 65, bitint> {{.*}})
// LLVM: define {{.*}}void @arg_bitint65(i65 noundef %{{.*}})

void arg_short4(short4 v) {}

// CIR: cir.func {{.*}}@arg_short4(%arg0: !cir.vector<4 x !s16i> {{.*}})
// LLVM: define {{.*}}void @arg_short4(<4 x i16> noundef %{{.*}})

// An empty struct is dropped from the signature.
void arg_empty(empty_t e) {}

// CIR: cir.func {{.*}}@arg_empty()
// LLVM: define {{.*}}void @arg_empty()

// Single-element structs pass as their element.
void arg_i8_wrapper(i8_wrapper_t w) {}

// CIR: cir.func {{.*}}@arg_i8_wrapper(%arg0: !s8i{{.*}})
// CIR:   cir.alloca "coerce" {{.*}} : !cir.ptr<{{.*}}, target_address_space(5)>
// LLVM: define {{.*}}void @arg_i8_wrapper(i8 %{{.*}})

void arg_i32_wrapper(i32_wrapper_t w) {}

// CIR: cir.func {{.*}}@arg_i32_wrapper(%arg0: !s32i{{.*}})
// LLVM: define {{.*}}void @arg_i32_wrapper(i32 %{{.*}})

void arg_f32_wrapper(f32_wrapper_t w) {}

// CIR: cir.func {{.*}}@arg_f32_wrapper(%arg0: !cir.float{{.*}})
// LLVM: define {{.*}}void @arg_f32_wrapper(float %{{.*}})

// Aggregates up to 32 bits are packed into one integer register.
void arg_char2(char2_t s) {}

// CIR: cir.func {{.*}}@arg_char2(%arg0: !u16i{{.*}})
// LLVM: define {{.*}}void @arg_char2(i16 %{{.*}})

void arg_small(small_t s) {}

// CIR: cir.func {{.*}}@arg_small(%arg0: !u32i{{.*}})
// LLVM: define {{.*}}void @arg_small(i32 %{{.*}})

// The call site is coerced the same way as the callee.
void call_small(small_t s) { arg_small(s); }

// CIR: cir.func {{.*}}@call_small(%arg0: !u32i{{.*}})
// CIR:   cir.call @arg_small(%{{.*}}){{.*}}: (!u32i{{.*}}) -> ()
// LLVM: define {{.*}}void @call_small(i32 %{{.*}})
// LLVM:   call void @arg_small(i32 %{{.*}})
