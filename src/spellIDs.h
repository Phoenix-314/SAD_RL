#pragma once

enum SpellID {
    MEND = 0,
    HEMLOCK = 1,
    RESTORE = 2,
    GAZE = 3,
    BANDAGE = 4,
    BALANCE = 5,
    VINE = 6,
    RENEW = 7,
    PRAY = 8,
    INFUSE = 9,
    FLIP = 10,
    GLOW = 11,
    CIRCLE = 12,
    LIQUOR = 13,
    BIND = 14,
    SOOTHE = 15,
    RITUAL = 16,
    SALVE = 17,
    LEECH = 18,
    OPERATE = 19,
    STRAND = 20,
    POKE = 21,
    SCORCH = 22,
    SLAY = 23,
    SLICE = 24,
    GATHER = 25,
    CUT = 26,
    SCALD = 27,
    DROP = 28,
    CHILL = 29,
    FLICK = 30,
    ZAP = 31,
    SPORE = 32,
    FORETELL = 33,
    BURN = 34,
    BLADES = 35,
    CRUSH = 36,
    MIASMA = 37,
    TICK = 38,
    BLAZE = 39,
    DRAW = 40,
    HARVEST = 41,
    INSPIRE = 42,
    // Weird spells/Tactics
    IMBUE = 43,
    FORMATION = 44,
    PARRY = 45,
    ELSE = 46,
    LIGHT = 47,
    UNITE = 48,
    CLINK = 49,
    BURST = 50 // default spell for state.spellData[0]
};
enum SpellData {
    // Positive values represent additional cost for DEPLETE spells
    STANDARD = 0,
    COOLDOWN_USED = -1,
    SINGLE_CASTED = -2
};
enum class SpellCastAffect {
    STANDARD = 0,
    COOLDOWN = 1,
    SINGLE_CAST = 2,
    DEPLETE = 3
};
enum class SpellTargetReq {
    UNTARGETED = 0,
    ENEMY = 1,
    ALLY = 2,
    EITHER = 3,
    RESURRECT = 4,
    HP1 = 5, // Harvest
    HP2 = 6, // Zap
    HP3 = 7, // Slay
    RECHARGE = 8,
    DAMAGE_N_SHIELD_3 = 9, // Formation
    DAMAGE_2 = 10, // Parry
    ONE_OF_ALL = 11, // Unite - 1x (Damage, Shield, Heal, Blank)
    AN_X = 12 // Else
};