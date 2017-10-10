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

#ifndef VMU_WINDOWS_PROTECT_MEMORY_INL
#define VMU_WINDOWS_PROTECT_MEMORY_INL

#include "error.hpp"
#include "../../protect.hpp"
#include "../checked_pointers.hpp"

namespace vmu {

    template<class Address>
    inline void protect(Address begin, Address end, protection::storage prot)
    {
        unsigned long old;
        const auto    size = detail::pointer_cast<detail::ULONG_PTR_>(end)
                             - detail::pointer_cast_unchecked<detail::ULONG_PTR_>(begin);

        // end is checked. If begin is bigger than end the problem's on you
        if (detail::VirtualProtect(detail::pointer_cast_unchecked<void*>(begin)
                                   , size
                                   , prot.native()
                                   , &old) == 0)
            detail::throw_last_error("VirtualProtect() failed");
    }


    template<class Address>
    inline void
    protect(Address begin, Address end, protection::storage prot, std::error_code& ec)
    {
        unsigned long old;
        const auto    size = detail::pointer_cast<detail::ULONG_PTR_>(end)
                             - detail::pointer_cast_unchecked<detail::ULONG_PTR_>(begin);

        if (detail::VirtualProtect(detail::pointer_cast_unchecked<void*>(begin)
                                   , size
                                   , prot.native()
                                   , &old) == 0)
            ec = detail::get_last_error();
    }

} // namespace vmu

#endif // include guard
