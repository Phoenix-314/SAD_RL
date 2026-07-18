"""
KeywordIDs. Order is important.
"""
class KeywordIDs:
    # TARGETING
    RANGED=0
    HEAVY=1
    STICKY=2
    MANDATORY=3
    CANTRIP=4
    STASIS=5
    ELIMINATE=6
    # COPYCAT
    COPYCAT=7
    # INCREASING PIPS
    PRISTINE=8
    DEATHWISH=9
    CHAIN=10
    BLOODLUST=11
    STEEL=12
    CHARGED=13
    ERA=14
    INSPIRED=15
    DEFY=16
    PAIR=17
    TRIO=18
    PLAGUE=19
    # Selfshield must be before the action for spikymons, and must happen only once (not once per target). Selfheal must be after
    SELFSHIELD=20 # Selfshield is the only action in this category
    # Dynamic keywords
    FOCUS=21
    CRUEL=22
    ENGAGE=23
    # Effects applying once per target (selfshield, selfheal, etc. dont apply once per target, but also dont have cleave. Cleanse, poison, weaken, inflicts do though)
    POISON=24
    CLEANSE=25
    VITALITY=26
    REGEN=27
    INFLICT_PAIN=28
    INFLICT_EXERT=29
    INFLICT_DEATH=30
    INFLICT_SINGLEUSE=31
    PETRIFY=32
    GUILT=33
    WEAKEN=34
    VULNERABLE=35
    BOOST=36
    SMITH=37
    PERMABOOST=38
    # Action Time. These keywords are handled by useDice, instead of in the keyword function in keywords.py
    RAMPAGE=39
    RESCUE=40
    DOUBLEUSE=41
    QUADUSE=42
    CLEAVE=43
    DESCEND=44
    REPEL=45
    # BONED
    BONED=46
    # Post Attack Effects
    DEATH=47
    PAIN=48
    MANAGAIN=49
    SELFHEAL=50
    DUPLICATE=51
    EXERT=52
    GROOOOWTH=53
    GROWTH=54
    SINGLEUSE=55
    DECAY=56
    # CONSTANTS relating to keywords
    NUM_KEYWORDS = 57 # NUM KEYWORDS (+1 since 0 based)
    START_STATICS = 8 # first 7 are only for checking valid actions/targeting things, 8th is for copycat
    START_PRE_MON_EFFECTS = 20
    START_DYNAMICS = 21 # after keyword 21, save the amount for comparing with pair and trio. Engage, cruel, etc. dont count
    START_APPLIED_EFFECTS = 24 # after keyword 24, the keywords are applied effects (poison, cleanse, etc.)
    ACT_OCCUR = 39 # At 39, these things effect whether the dice is "used" and who is hit by the use of the dice
    ACT_POST_OCCUR = 47