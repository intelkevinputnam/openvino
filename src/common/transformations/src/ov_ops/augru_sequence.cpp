// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "ov_ops/augru_sequence.hpp"

#include "augru_sequence_shape_inference.hpp"
#include "itt.hpp"
#include "ngraph/op/util/recurrent_sequence.hpp"

using namespace std;

ov::op::internal::AUGRUSequence::AUGRUSequence()
    : m_direction(op::RecurrentSequenceDirection::FORWARD),
      m_linear_before_reset(false) {}

ov::op::internal::AUGRUSequence::AUGRUSequence(const Output<Node>& X,
                                               const Output<Node>& H_t,
                                               const Output<Node>& sequence_lengths,
                                               const Output<Node>& W,
                                               const Output<Node>& R,
                                               const Output<Node>& B,
                                               const Output<Node>& A,
                                               std::size_t hidden_size)
    : RNNCellBase({X, H_t, sequence_lengths, W, R, B, A},
                  hidden_size,
                  0.f,
                  std::vector<std::string>{"sigmoid", "tanh"},
                  {},
                  {}),
      m_direction(op::RecurrentSequenceDirection::FORWARD),
      m_linear_before_reset(false) {
    constructor_validate_and_infer_types();
}

void ov::op::internal::AUGRUSequence::validate_and_infer_types() {
    INTERNAL_OP_SCOPE(internal_AUGRUSequence_validate_and_infer_types);

    NODE_VALIDATION_CHECK(this, m_clip == 0.f, "AUGRUSequence doesn't support clip other than 0.");
    NODE_VALIDATION_CHECK(this,
                          m_activations.size() == 2 && m_activations[0] == "sigmoid" && m_activations[1] == "tanh",
                          "AUGRUSequence supports only sigmoid for f and tanh for g activation functions.");
    NODE_VALIDATION_CHECK(this,
                          m_activations_alpha.empty() && m_activations_beta.empty(),
                          "AUGRUSequence doesn't support activations_alpha and activations_beta.");
    NODE_VALIDATION_CHECK(this,
                          m_direction == op::RecurrentSequenceDirection::FORWARD,
                          "AUGRUSequence supports only forward direction.");
    NODE_VALIDATION_CHECK(this,
                          m_linear_before_reset == false,
                          "AUGRUSequence supports only linear_before_reset equals false.");

    // Validate input types and save result for output type
    auto result_et = element::dynamic;
    NODE_VALIDATION_CHECK(this,
                          element::Type::merge(result_et, result_et, get_input_element_type(0)) &&
                              element::Type::merge(result_et, result_et, get_input_element_type(1)) &&
                              element::Type::merge(result_et, result_et, get_input_element_type(3)) &&
                              element::Type::merge(result_et, result_et, get_input_element_type(4)) &&
                              element::Type::merge(result_et, result_et, get_input_element_type(5)) &&
                              element::Type::merge(result_et, result_et, get_input_element_type(6)),
                          "Element types for inputs do not match.");

    const auto input_shapes = get_node_input_partial_shapes(*this);
    std::vector<ov::PartialShape> output_shapes = {ov::PartialShape::dynamic(4), ov::PartialShape::dynamic(3)};
    shape_infer(this, input_shapes, output_shapes);

    // Set output size, type and shape
    set_output_size(2);
    set_output_type(0, result_et, output_shapes[0]);
    set_output_type(1, result_et, output_shapes[1]);
}

bool ov::op::internal::AUGRUSequence::visit_attributes(AttributeVisitor& visitor) {
    INTERNAL_OP_SCOPE(internal_AUGRUSequence_visit_attributes);
    visitor.on_attribute("direction", m_direction);
    visitor.on_attribute("linear_before_reset", m_linear_before_reset);
    return op::util::RNNCellBase::visit_attributes(visitor);
}

shared_ptr<ov::Node> ov::op::internal::AUGRUSequence::clone_with_new_inputs(const OutputVector& new_args) const {
    INTERNAL_OP_SCOPE(internal_AUGRUSequence_clone_with_new_inputs);
    check_new_args_count(this, new_args);
    return make_shared<ov::op::internal::AUGRUSequence>(new_args.at(0),
                                                        new_args.at(1),
                                                        new_args.at(2),
                                                        new_args.at(3),
                                                        new_args.at(4),
                                                        new_args.at(5),
                                                        new_args.at(6),
                                                        get_hidden_size());
}
