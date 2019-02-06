/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/compiler/xla/service/cpu/runtime_single_threaded_matmul.h"

#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"
#include "tensorflow/compiler/xla/service/cpu/runtime_matvec.h"
#include "tensorflow/core/platform/types.h"

using tensorflow::int32;
using tensorflow::int64;

namespace {

void MatMul(float* out, float* lhs, float* rhs, int8_t m)
{
  const Eigen::TensorMap<Eigen::Tensor<const float, 2>, Eigen::Aligned> A(lhs, m, m);
  const Eigen::TensorMap<Eigen::Tensor<const float, 2>, Eigen::Aligned> B(rhs, 1, m);
  Eigen::TensorMap<Eigen::Tensor<float, 2>, Eigen::Aligned> C(out, m, 1);

  typedef typename Eigen::Tensor<float, 2>::DimensionPair DimPair;
//  int lhs_contract_dim = 0;
//  int rhs_contract_dim = 1;
  const Eigen::array<DimPair, 1> dims({DimPair(0, 1)});

  // Matrix multiply is a special case of the "contract" operation where
  // the contraction is performed along dimension 1 of the lhs and dimension
  // 0 of the rhs.
  C = A.contract(B, dims);
}

}  // namespace