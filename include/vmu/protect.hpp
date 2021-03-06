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

#ifndef VMU_PROTECT_MEMORY_HPP
#define VMU_PROTECT_MEMORY_HPP

#include <system_error>
#include "protection.hpp"

namespace vmu {

    /// \brief Returns the operating systems page size.
    /// \throws Does not throw
    inline std::size_t page_size() noexcept;

    /// \brief Changes protection of pages in the range [begin; end)
    /// \param begin The beginning of range to protect.
    /// \param end One past the end of range to protect.
    /// \param prot The protection to apply to the range.
    /// \throws May throw system_error or overflow_error if address overflows.
    template<class Address>
    inline void protect(Address begin, Address end, protection_t prot);

    /// \brief Changes protection of pages in the range [begin; end)
    /// \param begin The beginning of range to protect.
    /// \param end One past the end of range to protect.
    /// \param prot The protection to apply to the range.
    /// \throws May throw overflow_error if address overflows.
    template<class Address>
    inline void protect(Address begin
                        , Address end
                        , protection_t prot
                        , std::error_code& ec);

    /// \brief Changes the protection of a page.
    /// \param begin The address of the page to protect.
    /// \param prot The protection to apply to the page.
    /// \throws May throw system_error or overflow_error if address overflows.
    template<class Address>
    inline void protect(Address address, protection_t prot)
    {
        auto fixed_address = detail::uintptr_cast(address);
#ifndef _WIN32
        fixed_address &= -page_size();
#endif
        protect(fixed_address, fixed_address + 1, prot);
    }

    /// \brief Changes the protection of a page.
    /// \param begin The address of the page to protect.
    /// \param prot The protection to apply to the page.
    /// \throws May throw overflow_error if address overflows.
    template<class Address>
    inline void protect(Address address, protection_t prot, std::error_code& ec)
    {
        auto fixed_address = detail::uintptr_cast(address);
#ifndef _WIN32
        fixed_address &= -page_size();
#endif
        protect(fixed_address, fixed_address + 1, prot, ec);
    }

} // namespace vmu

#if defined(_WIN32)
    #include "detail/windows/protect.inl"
#else
    #include "detail/posix/protect.inl"
#endif

#endif // include guard
