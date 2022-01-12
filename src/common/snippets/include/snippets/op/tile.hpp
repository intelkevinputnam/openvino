// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "ngraph/op/op.hpp"
#include "snippets/emitter.hpp"

namespace ngraph {
namespace snippets {
namespace op {

/**
 * @interface Tile
 * @brief Generated by Canonicalization and represents Loop in affine notation
 * @ingroup snippets
 */
class Tile : public ngraph::op::Op {
public:
    OPENVINO_OP("Tile", "SnippetsOpset");

    Tile(const std::vector<std::pair<std::shared_ptr<ngraph::snippets::Emitter>, ngraph::snippets::RegInfo>>& region);
    Tile() = default;
    std::vector<std::pair<std::shared_ptr<ngraph::snippets::Emitter>, ngraph::snippets::RegInfo>> region;

    std::shared_ptr<Node> clone_with_new_inputs(const OutputVector& inputs) const override {
        return std::make_shared<Tile>(region);
    }
    const void *compile_params;
};

} // namespace op
} // namespace snippets
} // namespace ngraph