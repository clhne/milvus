// Copyright (C) 2019-2020 Zilliz. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing permissions and limitations under the License.

#include "server/init/CpuChecker.h"

#include <iostream>
#include <string>
#include <vector>

#include "faiss/FaissHook.h"
#include "faiss/utils/instruction_set.h"
#include "utils/Log.h"
#include "utils/StringHelpFunctions.h"

namespace milvus {
namespace server {

Status
CpuChecker::CheckCpuInstructionSet() {
    std::vector<std::string> instruction_sets;

    auto& instruction_set_inst = faiss::InstructionSet::GetInstance();
    if (faiss::support_avx512()) {
        instruction_sets.emplace_back("avx512");
    }
    if (instruction_set_inst.AVX2()) {
        instruction_sets.emplace_back("avx2");
    }
    if (instruction_set_inst.SSE42()) {
        instruction_sets.emplace_back("sse4_2");
    }

    if (instruction_sets.empty()) {
        std::string msg =
            "CPU instruction sets are not supported. Ensure the CPU supports at least one of the following instruction "
            "sets: sse4_2, avx2, avx512";
        LOG_SERVER_FATAL_ << msg;
        std::cerr << msg << std::endl;
        return Status(SERVER_UNEXPECTED_ERROR, msg);
    }

    std::string instruction_sets_msg;
    StringHelpFunctions::MergeStringWithDelimeter(instruction_sets, ", ", instruction_sets_msg);
    std::string msg = "Supported CPU instruction sets: " + instruction_sets_msg;
    LOG_SERVER_INFO_ << msg;
    LOG_ENGINE_DEBUG_ << msg;
    std::cout << msg << std::endl;

    return Status::OK();
}

}  // namespace server
}  // namespace milvus
