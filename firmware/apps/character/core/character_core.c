#include "character_core.h"
#include <string.h>

static void remember(character_core_t *core)
{
    core->undo = core->state;
    core->can_undo = true;
}

void character_core_init(character_core_t *core, const character_state_t *initial)
{
    memset(core, 0, sizeof(*core));
    if (!initial) return;
    core->state = *initial;
    if (core->state.hp_max < 1) core->state.hp_max = 1;
    if (core->state.hp < 0) core->state.hp = 0;
    if (core->state.hp > core->state.hp_max) core->state.hp = core->state.hp_max;
    if (core->state.spell_slots > core->state.spell_slots_max)
        core->state.spell_slots = core->state.spell_slots_max;
    if (core->state.resource > core->state.resource_max)
        core->state.resource = core->state.resource_max;
}

character_result_t character_damage(character_core_t *core, uint16_t amount)
{
    if (!core || amount == 0 || (core->state.hp == 0 && core->state.temp_hp == 0))
        return CHARACTER_NO_CHANGE;
    remember(core);
    int remaining = amount;
    if (remaining <= core->state.temp_hp) {
        core->state.temp_hp -= remaining;
        return CHARACTER_OK;
    }
    remaining -= core->state.temp_hp;
    core->state.temp_hp = 0;
    core->state.hp = remaining >= core->state.hp ? 0 : core->state.hp - remaining;
    return CHARACTER_OK;
}

character_result_t character_heal(character_core_t *core, uint16_t amount)
{
    if (!core || amount == 0 || core->state.hp >= core->state.hp_max)
        return CHARACTER_NO_CHANGE;
    remember(core);
    int healed = core->state.hp + amount;
    core->state.hp = healed > core->state.hp_max ? core->state.hp_max : healed;
    return CHARACTER_OK;
}

character_result_t character_set_temp_hp(character_core_t *core, uint16_t amount)
{
    if (!core || amount > 9999) return CHARACTER_INVALID;
    if (core->state.temp_hp == (int16_t)amount) return CHARACTER_NO_CHANGE;
    remember(core);
    core->state.temp_hp = (int16_t)amount;
    return CHARACTER_OK;
}

character_result_t character_use_slot(character_core_t *core)
{
    if (!core || core->state.spell_slots == 0) return CHARACTER_EMPTY;
    remember(core);
    core->state.spell_slots--;
    return CHARACTER_OK;
}

character_result_t character_use_resource(character_core_t *core)
{
    if (!core || core->state.resource == 0) return CHARACTER_EMPTY;
    remember(core);
    core->state.resource--;
    return CHARACTER_OK;
}

character_result_t character_long_rest(character_core_t *core)
{
    if (!core) return CHARACTER_INVALID;
    character_state_t next = core->state;
    next.hp = next.hp_max;
    next.temp_hp = 0;
    next.spell_slots = next.spell_slots_max;
    next.resource = next.resource_max;
    if (memcmp(&next, &core->state, sizeof(next)) == 0) return CHARACTER_NO_CHANGE;
    remember(core);
    core->state = next;
    return CHARACTER_OK;
}

character_result_t character_undo(character_core_t *core)
{
    if (!core || !core->can_undo) return CHARACTER_EMPTY;
    core->state = core->undo;
    core->can_undo = false;
    return CHARACTER_OK;
}
