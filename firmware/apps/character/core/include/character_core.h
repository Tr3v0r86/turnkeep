#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CHARACTER_OK = 0,
    CHARACTER_NO_CHANGE,
    CHARACTER_EMPTY,
    CHARACTER_INVALID,
} character_result_t;

typedef struct {
    int16_t hp;
    int16_t hp_max;
    int16_t temp_hp;
    uint8_t spell_slots;
    uint8_t spell_slots_max;
    uint8_t resource;
    uint8_t resource_max;
} character_state_t;

typedef struct {
    character_state_t state;
    character_state_t undo;
    bool can_undo;
} character_core_t;

void character_core_init(character_core_t *core, const character_state_t *initial);
character_result_t character_damage(character_core_t *core, uint16_t amount);
character_result_t character_heal(character_core_t *core, uint16_t amount);
character_result_t character_set_temp_hp(character_core_t *core, uint16_t amount);
character_result_t character_use_slot(character_core_t *core);
character_result_t character_use_resource(character_core_t *core);
character_result_t character_long_rest(character_core_t *core);
character_result_t character_undo(character_core_t *core);
