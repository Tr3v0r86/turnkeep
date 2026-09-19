#include "character_core.h"
#include <assert.h>
#include <stdio.h>

static character_state_t fresh(void)
{
    return (character_state_t){
        .hp = 32, .hp_max = 40, .temp_hp = 5,
        .spell_slots = 2, .spell_slots_max = 3,
        .resource = 1, .resource_max = 2,
    };
}

static void damage_consumes_temp_before_hp(void)
{
    character_core_t core;
    character_state_t initial = fresh();
    character_core_init(&core, &initial);
    assert(character_damage(&core, 17) == CHARACTER_OK);
    assert(core.state.temp_hp == 0);
    assert(core.state.hp == 20);
}

static void healing_caps_at_max_and_zero_is_not_a_mutation(void)
{
    character_core_t core;
    character_state_t initial = fresh();
    character_core_init(&core, &initial);
    assert(character_heal(&core, 99) == CHARACTER_OK);
    assert(core.state.hp == 40);
    assert(character_heal(&core, 1) == CHARACTER_NO_CHANGE);
    assert(character_damage(&core, 0) == CHARACTER_NO_CHANGE);
}

static void spend_and_rest_obey_pool_bounds(void)
{
    character_core_t core;
    character_state_t initial = fresh();
    character_core_init(&core, &initial);
    assert(character_use_slot(&core) == CHARACTER_OK);
    assert(character_use_slot(&core) == CHARACTER_OK);
    assert(character_use_slot(&core) == CHARACTER_EMPTY);
    assert(character_use_resource(&core) == CHARACTER_OK);
    assert(character_use_resource(&core) == CHARACTER_EMPTY);
    assert(character_long_rest(&core) == CHARACTER_OK);
    assert(core.state.hp == 40);
    assert(core.state.temp_hp == 0);
    assert(core.state.spell_slots == 3);
    assert(core.state.resource == 2);
}

static void undo_restores_exact_previous_state_once(void)
{
    character_core_t core;
    character_state_t initial = fresh();
    character_core_init(&core, &initial);
    assert(character_damage(&core, 7) == CHARACTER_OK);
    assert(character_undo(&core) == CHARACTER_OK);
    assert(core.state.hp == 32);
    assert(core.state.temp_hp == 5);
    assert(character_undo(&core) == CHARACTER_EMPTY);
}

int main(void)
{
    damage_consumes_temp_before_hp();
    healing_caps_at_max_and_zero_is_not_a_mutation();
    spend_and_rest_obey_pool_bounds();
    undo_restores_exact_previous_state_once();
    puts("character core: ok");
    return 0;
}
