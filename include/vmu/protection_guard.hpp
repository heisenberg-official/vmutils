/*
* Copyright 2017 Justas Masiulis
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef VMU_PROTECTION_GUARD_HPP
#define VMU_PROTECTION_GUARD_HPP

#include "query.hpp"
#include "protect.hpp"

namespace vmu {

    struct adopt_protection_t { explicit adopt_protection_t() = default; };

    class protection_guard {
        struct old_prot_storage {
            std::uintptr_t      begin;
            std::uintptr_t      end;
            native_protection_t prot;
        };

        std::vector<old_prot_storage> _old;
    public:
        template<class Address>
        protection_guard(Address begin, Address end, protection::storage prot)
        {
            auto regions = query_range(begin, end);
            _old.reserve(regions.size());
            for (auto& region : regions) {
                if (region)
                    _old.emplace_back(region.begin, region.end, region.prot);
            }

            protect(begin, end, prot);
        }

        template<class Address>
        protection_guard(const basic_region<Address>& r, protection::storage prot)
                : protection_guard(r.begin(), r.end(), prot) {}

        template<class Address>
        protection_guard(Address begin
                         , Address end
                         , protection::storage restore_to
                         , adopt_protection_t)
        {
            _old.reserve(1);
            _old.emplace_back(begin, end, restore_to);
        }

        template<class Address>
        protection_guard(const basic_region<Address>& r
                         , protection::storage prot
                         , adopt_protection_t)
                : protection_guard(r.begin, r.end, prot, adopt_protection_t{}) {}

        /// \brief Restores the memory protection to its previous state
        ~protection_guard()
        {
            std::error_code ec;
            for (const auto& ele : _old)
                protect(ele.begin, ele.end, ele.prot, ec);
        }

        /// \brief not copy constructible
        protection_guard(const protection_guard&) = delete;
        /// \brief not copy assignable
        protection_guard& operator=(const protection_guard&) = delete;

        void restore()
        {
            while (!_old.empty()) {
                const auto& back = _old.back();
                protect(back.begin, back.end, back.prot);
                _old.pop_back();
            }
        }
        void restore(std::error_code& ec)
        {
            while (!_old.empty()) {
                const auto& back = _old.back();
                protect(back.begin, back.end, back.prot, ec);
                if (ec)
                    return;

                _old.pop_back();
            }
        }
    };

}

#endif // include guard
