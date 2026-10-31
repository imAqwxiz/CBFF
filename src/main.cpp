#include <Geode/Geode.hpp>
#include "Core/CBFFSettings.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    cbff::CBFFSettings::get().load();
}
