// Copyright 2022 The TCMalloc Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TCMALLOC_SAMPLED_ALLOCATION_ALLOCATOR_H_
#define TCMALLOC_SAMPLED_ALLOCATION_ALLOCATOR_H_

#include <utility>

#include "absl/base/thread_annotations.h"
#include "tcmalloc/arena.h"
#include "tcmalloc/common.h"
#include "tcmalloc/internal/allocation_guard.h"
#include "tcmalloc/internal/config.h"
#include "tcmalloc/internal/logging.h"
#include "tcmalloc/internal/sampled_allocation.h"
#include "tcmalloc/metadata_object_allocator.h"

GOOGLE_MALLOC_SECTION_BEGIN
namespace tcmalloc {
namespace tcmalloc_internal {

// Wrapper around MetadataObjectAllocator<SampledAllocation> to provide a
// customized New() and Delete() for SampledAllocation. 1) SampledAllocation is
// used internally by TCMalloc and can not use normal heap allocation. We rely
// on MetadataObjectAllocator that allocates from TCMalloc's arena and requires
// the pageheap_lock here. 2) MetadataObjectAllocator only allocates/deallocates
// memory, so we need to manually invoke the constructor/destructor to
// initialize/clear some fields.
class SampledAllocationAllocator {
 public:
  // TODO(b/175334169):  Delete this class by pushing construction logic into
  // MetadataObjectAllocator.
  constexpr SampledAllocationAllocator() = default;

  void Init(Arena* arena) { allocator_.Init(arena); }

  SampledAllocation* New(StackTrace stack_trace) {
    SampledAllocation* s = allocator_.New();
    return new (s) SampledAllocation(std::move(stack_trace));
  }

  void Delete(SampledAllocation* s) { allocator_.Delete(s); }

  AllocatorStats stats() const { return allocator_.stats(); }

 private:
  MetadataObjectAllocator<SampledAllocation> allocator_;
};

}  // namespace tcmalloc_internal
}  // namespace tcmalloc
GOOGLE_MALLOC_SECTION_END

#endif  // TCMALLOC_SAMPLED_ALLOCATION_ALLOCATOR_H_
