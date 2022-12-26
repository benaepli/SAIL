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
        std::shared_ptr<Native::Functions::Print> print =
            std::make_shared<Native::Functions::Print>();
        environment.define(print->name(), print);

        std::shared_ptr<Native::Functions::Millis> millis =
            std::make_shared<Native::Functions::Millis>();
        environment.define(millis->name(), millis);

        std::shared_ptr<Native::Functions::Seconds> seconds =
            std::make_shared<Native::Functions::Seconds>();
        environment.define(seconds->name(), seconds);
    }
}  // namespace sail