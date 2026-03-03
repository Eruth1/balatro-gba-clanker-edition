#include "joker.h"
#include "game.h"
#include <stddef.h>

#include "custom_joker_sheet_0.h"
#include "custom_joker_sheet_1.h"
// #include "custom_joker_sheet_x.h" // Add this when you make IDs 1xx & 1xx!

#define MODDED_JOKER_START_ID 100
#define NUM_JOKERS_PER_SPRITESHEET 2

// --- 0. LOCAL EFFECT OBJECT ---
// We use this local object so we don't conflict with the vanilla file's locked memory
static JokerEffect modded_shared_joker_effect = {0};


// --- 1. YOUR CUSTOM JOKER LOGIC ---

static u32 mobius_joker_effect(
    Joker* joker, 
    Card* scored_card, 
    enum JokerEvent joker_event, 
    JokerEffect** joker_effect
)
{
    // You can add your actual Mobius logic here whenever you are ready!
    return JOKER_EFFECT_FLAG_NONE; 
}

static u32 last_dance_joker_effect(
    Joker* joker, 
    Card* scored_card, 
    enum JokerEvent joker_event, 
    JokerEffect** joker_effect
)
{
    if (joker_event == JOKER_EVENT_INDEPENDENT) {
        // Point to our safe local variable
        *joker_effect = &modded_shared_joker_effect;
        
        // x3 Total Mult
        (*joker_effect)->xmult = 3; 
        
        // x2 Total Chips (By adding 100% of our current chips to the pool!)
        (*joker_effect)->chips = get_chips(); 
        
        return JOKER_EFFECT_FLAG_XMULT | JOKER_EFFECT_FLAG_CHIPS;
    }
    
    return JOKER_EFFECT_FLAG_NONE; 
}

static u32 jaker_joker_effect(
    Joker* joker, 
    Card* scored_card, 
    enum JokerEvent joker_event, 
    JokerEffect** joker_effect
)
{
    // Jaker modifies hands at the start of the round, so his scoring effect is empty!
    return JOKER_EFFECT_FLAG_NONE; 
}

static u32 voor_joker_effect(
    Joker* joker, 
    Card* scored_card, 
    enum JokerEvent joker_event, 
    JokerEffect** joker_effect
)
{
    // Start with 2 Mult when conjured or bought
    if (joker_event == JOKER_EVENT_ON_JOKER_CREATED) {
        joker->persistent_state = 2; 
    }
    
    // Add the stored Mult to the score!
    if (joker_event == JOKER_EVENT_INDEPENDENT && joker->persistent_state > 0) {
        *joker_effect = &modded_shared_joker_effect;
        (*joker_effect)->mult = joker->persistent_state; 
        return JOKER_EFFECT_FLAG_MULT;
    }
    return JOKER_EFFECT_FLAG_NONE; 
}


// --- 2. YOUR MODDED REGISTRY ---

// The engine knows to start reading this array at ID 100.
// So Index 0 is ID 100 (Mobius), Index 1 is ID 101 (Last Dance).
// Because we set NUM_JOKERS_PER_SPRITESHEET to 2, 
// Mobius reads the Left half, Last Dance reads the Right half!
const JokerInfo modded_joker_registry[] = {
    { UNCOMMON_JOKER,  7, mobius_joker_effect     }, // Local Index 0 -> In-game ID: 100 (Mobius = Recursion)
    { RARE_JOKER, 20, last_dance_joker_effect }, // Local Index 1 -> In-game ID: 101
    { COMMON_JOKER,    7, voor_joker_effect       }, // ID 102
    { UNCOMMON_JOKER, 10, jaker_joker_effect      }, // ID 103
};


// --- 3. HELPER FUNCTIONS FOR THE ENGINE ---
// (Do not change these! The vanilla game uses them to read your mods safely)

size_t get_modded_registry_size(void) 
{
    return sizeof(modded_joker_registry) / sizeof(modded_joker_registry[0]);
}

const JokerInfo* get_modded_registry_entry(int local_id) 
{
    return &modded_joker_registry[local_id];
}
const JokerInfo modded_joker_registry[] = {
    // ... các joker trước đó ...
    
    // Sheet 30 - Trái (ID 160): Lucky Coin
    { UNCOMMON_JOKER, 10, lucky_coin_joker_effect }, 
    
    // Sheet 30 - Phải (ID 161): Silence Joker
    { RARE_JOKER,     25, silence_joker_effect    }, 
};
const JokerInfo modded_joker_registry[] = {
    // ... các joker trước đó ...
    
    // Sheet 30 - Trái (ID 160): Lucky Coin
    { UNCOMMON_JOKER, 10, lucky_coin_joker_effect }, 
    
    // Sheet 30 - Phải (ID 161): Silence Joker
    { RARE_JOKER,     25, silence_joker_effect    }, 
};
// --- source/modded_joker_effects.c ---
int silence_mult = 0; 
int silence_chips = 0;
int silence_discards = 0;
int silence_hands = 0;

// Logic Lucky Coin (ID 160 - Bên trái sheet 30)
static u32 lucky_coin_joker_effect(Joker* joker, Card* scored_card, enum JokerEvent joker_event, JokerEffect** joker_effect) {
    if (joker_event == JOKER_EVENT_INDEPENDENT) {
        if (rand() % 2 == 0) {
            *joker_effect = &modded_shared_joker_effect;
            (*joker_effect)->x_mult = 2;
            return JOKER_EFFECT_FLAG_X_MULT;
        }
    }
    return JOKER_EFFECT_FLAG_NONE;
}

// Logic Silence Joker (ID 161 - Bên phải sheet 30)
static u32 silence_joker_effect(Joker* joker, Card* scored_card, enum JokerEvent joker_event, JokerEffect** joker_effect) {
    if (joker_event == JOKER_EVENT_ON_ROUND_START) {
        int r = rand() % 4;
        if (r == 0) silence_mult += 2;
        else if (r == 1) silence_chips += 15;
        else if (r == 2) silence_discards += 1;
        else if (r == 3) silence_hands += 1;
        
        // Cập nhật ngay lập tức số lượt đánh/hủy bài nếu trúng
        if (r == 2) set_num_discards_remaining(get_num_discards_remaining() + 1);
        if (r == 3) set_num_hands_remaining(get_num_hands_remaining() + 1);
    }

    if (joker_event == JOKER_EVENT_INDEPENDENT) {
        *joker_effect = &modded_shared_joker_effect;
        (*joker_effect)->mult = silence_mult;
        (*joker_effect)->chips = silence_chips;
        return JOKER_EFFECT_FLAG_MULT | JOKER_EFFECT_FLAG_CHIPS;
    }
    return JOKER_EFFECT_FLAG_NONE;
}

// Cập nhật Registry
const JokerInfo modded_joker_registry[] = {
    // ... các Joker cũ của bạn ...
    
    // ID 160 (Vị trí 60 tính từ ID 100)
    { UNCOMMON_JOKER, 10, lucky_coin_joker_effect }, 
    // ID 161 (Vị trí 61 tính từ ID 100)
    { RARE_JOKER,     25, silence_joker_effect    }, 
};