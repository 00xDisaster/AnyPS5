#include "prx/libSceLibreSsl3/include/Api.hpp"
#include "prx/libc/include/Api.hpp"
#include <limits>
#include <stdexcept>

namespace {

constexpr int MinimumNodes = 4;

}

extern "C" {

LibreSslStack* APS5_VABI sk_new_nid_postfix(StackCompare compare) {
    auto* const stack = static_cast<LibreSslStack*>(malloc_nid_postfix(sizeof(LibreSslStack)));
    if (!stack) return nullptr;
    auto** const elements = static_cast<char**>(reallocarray_nid_postfix(nullptr, MinimumNodes, sizeof(char*)));
    if (!elements) {
        free_nid_postfix(stack);
        return nullptr;
    }
    for (int index = 0; index < MinimumNodes; ++index) elements[index] = nullptr;
    stack->data = elements;
    stack->comp = compare;
    stack->numAlloc = MinimumNodes;
    stack->num = 0;
    stack->sorted = 0;
    return stack;
}

int APS5_VABI sk_push_nid_postfix(LibreSslStack* stack, void* element) {
    if (!stack) throw std::invalid_argument("sk_push: null stack");
    if (stack->num < 0) throw std::invalid_argument("sk_push: negative element count");
    if (static_cast<long long>(stack->numAlloc) <= static_cast<long long>(stack->num) + 1) {
        if (stack->numAlloc > std::numeric_limits<int>::max() / 2) throw std::length_error("sk_push: element array cannot double");
        auto** const grown = static_cast<char**>(reallocarray_nid_postfix(stack->data, stack->numAlloc, 2 * sizeof(char*)));
        if (!grown) return 0;
        stack->data = grown;
        stack->numAlloc *= 2;
    }
    if (!stack->data) throw std::invalid_argument("sk_push: null element array");
    stack->data[stack->num] = static_cast<char*>(element);
    ++stack->num;
    stack->sorted = 0;
    return stack->num;
}

void APS5_VABI sk_free_nid_postfix(LibreSslStack* stack) {
    if (!stack) return;
    free_nid_postfix(stack->data);
    free_nid_postfix(stack);
}

}
