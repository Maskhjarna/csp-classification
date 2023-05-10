#pragma once

#include <cspc/encodings/common.hpp>

extern auto minizinc_siggers_checker() -> cspc::polymorphism_checker;
extern auto encoding_siggers_checker() -> cspc::polymorphism_checker;
extern auto check_single(cspc::relation const& relation, cspc::polymorphism_checker checker)
	-> void;
extern auto
check_all_nary_on_domain(size_t n, size_t domain_size, cspc::polymorphism_checker checker) -> void;