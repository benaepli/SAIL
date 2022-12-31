#pragma once

#define SAIL_DELETE_COPY(Class) \
    Class(const Class&) = delete; \
    auto operator=(const Class&)->Class& = delete

#define SAIL_DELETE_MOVE(Class) \
    Class(Class&&) = delete; \
    auto operator=(Class&&)->Class& = delete

#define SAIL_DELETE_COPY_MOVE(Class) \
    SAIL_DELETE_COPY(Class); \
    SAIL_DELETE_MOVE(Class)

#define SAIL_DEFAULT_COPY(Class) \
    Class(const Class&) = default; \
    auto operator=(const Class&)->Class& = default

#define SAIL_DEFAULT_MOVE(Class) \
    Class(Class&&) = default; \
    auto operator=(Class&&)->Class& = default

#define SAIL_DEFAULT_COPY_MOVE(Class) \
    SAIL_DEFAULT_COPY(Class); \
    SAIL_DEFAULT_MOVE(Class)