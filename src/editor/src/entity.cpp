#include "entity.hpp"

unsigned component_counter_base::family_counter = 0;
std::unique_ptr<component_deleter_base> component_deleter_base::deleters[max_components];

