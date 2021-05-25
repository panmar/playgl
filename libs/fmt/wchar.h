// Formatting library for C++ - optional wchar_t support
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_WCHAR_H_
#define FMT_WCHAR_H_

#include <cwchar>

#include "format.h"

FMT_BEGIN_NAMESPACE
FMT_MODULE_EXPORT_BEGIN

using wstring_view = basic_string_view<wchar_t>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;
using wformat_context = buffer_context<wchar_t>;
using wformat_args = basic_format_args<wformat_context>;

#if FMT_GCC_VERSION && FMT_GCC_VERSION < 409
// Workaround broken conversion on older gcc.
template <typename... Args> using wformat_string = wstring_view;
#else
template <typename... Args>
using wformat_string = basic_format_string<wchar_t, type_identity_t<Args>...>;
#endif

template <typename... Args>
constexpr format_arg_store<wformat_context, Args...> make_wformat_args(
    const Args&... args) {
  return {args...};
}

inline namespace literals {
constexpr auto operator"" _format(const wchar_t* s, size_t n)
    -> detail::udl_formatter<wchar_t> {
  return {{s, n}};
}

#if FMT_USE_USER_DEFINED_LITERALS && !FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
constexpr detail::udl_arg<wchar_t> operator"" _a(const wchar_t* s, size_t) {
  return {s};
}
#endif
}  // namespace literals

template <typename It, typename Sentinel>
arg_join<It, Sentinel, wchar_t> join(It begin, Sentinel end, wstring_view sep) {
  return {begin, end, sep};
}

template <typename Range>
arg_join<detail::iterator_t<Range>, detail::sentinel_t<Range>, wchar_t> join(
    Range&& range, wstring_view sep) {
  return join(std::begin(range), std::end(range), sep);
}

template <typename T>
arg_join<const T*, const T*, wchar_t> join(std::initializer_list<T> list,
                                           wstring_view sep) {
  return join(std::begin(list), std::end(list), sep);
}

inline void vprint(std::FILE* f, wstring_view fmt, wformat_args args) {
  wmemory_buffer buffer;
  detail::vformat_to(buffer, fmt, args);
  buffer.push_back(L'\0');
  if (std::fputws(buffer.data(), f) == -1)
    FMT_THROW(system_error(errno, "cannot write to file"));
}

inline void vprint(wstring_view fmt, wformat_args args) {
  vprint(stdout, fmt, args);
}

template <typename... T>
void print(std::FILE* f, wformat_string<T...> fmt, T&&... args) {
  return vprint(f, wstring_view(fmt), make_wformat_args(args...));
}

template <typename... T> void print(wformat_string<T...> fmt, T&&... args) {
  return vprint(wstring_view(fmt), make_wformat_args(args...));
}

/**
  Converts *value* to ``std::wstring`` using the default format for type *T*.
 */
template <typename T> inline std::wstring to_wstring(const T& value) {
  return format(FMT_STRING(L"{}"), value);
}

FMT_MODULE_EXPORT_END
FMT_END_NAMESPACE

#endif  // FMT_WCHAR_H_
