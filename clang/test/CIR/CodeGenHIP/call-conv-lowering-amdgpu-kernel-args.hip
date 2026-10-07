// REQUIRES: amdgpu-registered-target
// RUN: %clang_cc1 -triple amdgcn-amd-amdhsa -x hip -fcuda-is-device -fclangir -fclangir-call-conv-lowering -emit-cir %s -o %t.cir
// RUN: FileCheck --check-prefix=CIR --input-file=%t.cir %s
// RUN: %clang_cc1 -triple amdgcn-amd-amdhsa -x hip -fcuda-is-device -fclangir -fclangir-call-conv-lowering -emit-llvm %s -o %t-cir.ll
// RUN: FileCheck --check-prefix=LLVM --input-file=%t-cir.ll %s
// RUN: %clang_cc1 -triple amdgcn-amd-amdhsa -x hip -fcuda-is-device -emit-llvm %s -o %t.ll
// RUN: FileCheck --check-prefix=LLVM --input-file=%t.ll %s

#define __global__ __attribute__((global))

typedef struct { int i; } i32_wrapper_t;

__global__ void kern_scalar(int i, float f) {}

// CIR: cir.func {{.*}}@_Z11kern_scalarif(%arg0: !s32i {{.*}}, %arg1: !cir.float {{.*}}){{.*}}cc(amdgpu_kernel)
// LLVM: define {{.*}}amdgpu_kernel void @_Z11kern_scalarif(i32 noundef %{{.*}}, float noundef %{{.*}})

// A single-element struct kernel argument is passed as its element.
__global__ void kern_wrapper(i32_wrapper_t w) {}

// CIR: cir.func {{.*}}@_Z12kern_wrapper13i32_wrapper_t(%arg0: !s32i{{.*}}){{.*}}cc(amdgpu_kernel)
// LLVM: define {{.*}}amdgpu_kernel void @_Z12kern_wrapper13i32_wrapper_t(i32 %{{.*}})
