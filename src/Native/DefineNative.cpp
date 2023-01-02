#include <memory>
#include <string_view>

#include "Native/DefineNative.h"

#include "Native/Functions/PrintFunction.h"
#include "Native/Functions/TimeFunction.h"
#include "Types/NullType.h"

namespace sail
{
    void defineNativeFunctions(Environment& environment)
    {
        auto print = std::make_shared<Native::Functions::Print>();
        environment.define(print->name(), print);

        auto millis = std::make_shared<Native::Functions::Millis>();
        environment.define(millis->name(), millis);

        auto seconds = std::make_shared<Native::Functions::Seconds>();
        environment.define(seconds->name(), seconds);
    }
}  // namespace sail