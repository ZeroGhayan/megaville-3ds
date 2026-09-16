# Roster Flash → pasta JPEXS, origem PNG, ranges de label.
# Ordem = roster.h (CH_BLOSSOM … CH_SHIRA).

# (nome, pasta dump, família de ranges)
CHARS = [
    ("blossom",   "DefineSprite_471_Blossom",     "ppg"),
    ("bubbles",   "DefineSprite_563_Bubbles",     "ppg"),
    ("buttercup", "DefineSprite_690_Buttercup",   "ppg"),
    ("bell",      "DefineSprite_370_Bell",        "ppg"),
    ("dexter",    "DefineSprite_1236_Dexter",     "dexter"),
    ("rowdy",     "DefineSprite_1399_Rowdyruff",  "ppg"),
    ("zim",       "DefineSprite_1334_zim",        "zim"),
    ("shira",     "DefineSprite_1469_Shira Bell", "ppg"),
]

# Mesmos labels que frame_2266 gotoAndPlay("idle"/…)
# Dexter conferido em data/dexter-timeline.md.
# PPG: mesmos primeiros keyframes (49 58 61 70 85 94 108…).
RANGES = {
    "dexter": [
        ("idle", 1, 48), ("land", 50, 57), ("jump", 62, 69),
        ("forward", 71, 84), ("dash", 86, 97), ("shield", 98, 98),
        ("combo1", 99, 117), ("combo2", 118, 130), ("combo3", 131, 151),
        ("ranged", 152, 198), ("air", 199, 222), ("downatk", 223, 250),
        ("fall", 251, 257), ("fallen", 258, 267), ("recover", 268, 278),
        ("win", 283, 295), ("teleport", 296, 318), ("damage", 330, 394),
    ],
    "ppg": [
        ("idle", 1, 48), ("land", 50, 57), ("jump", 62, 69),
        ("forward", 71, 84), ("dash", 86, 97), ("shield", 98, 98),
        ("combo1", 99, 117), ("combo2", 118, 130), ("combo3", 131, 151),
        ("ranged", 152, 198), ("air", 199, 222), ("downatk", 223, 250),
        ("fall", 251, 257), ("fallen", 258, 267), ("recover", 268, 278),
        ("win", 283, 295), ("teleport", 296, 318), ("damage", 330, 394),
    ],
    "zim": [
        ("idle", 1, 48), ("land", 50, 57), ("jump", 62, 69),
        ("forward", 71, 84), ("dash", 86, 97), ("shield", 98, 98),
        ("combo1", 99, 117), ("combo2", 118, 130), ("combo3", 131, 151),
        ("ranged", 152, 198), ("air", 199, 222), ("downatk", 223, 250),
        ("fall", 251, 257), ("fallen", 258, 267), ("recover", 268, 278),
        ("win", 283, 295), ("teleport", 296, 318), ("damage", 330, 394),
    ],
}

# 10 poses do atlas sliced (sprites.c SPR_*)
POSES = {
    "ppg": [
        ("idle", 1), ("land", 50), ("jump", 62), ("walk", 71),
        ("dash", 86), ("heavy", 95), ("light", 113), ("light2", 126),
        ("hit", 277), ("ko", 301),
    ],
    "dexter": [
        ("idle", 1), ("land", 50), ("jump", 62), ("walk", 71),
        ("dash", 86), ("heavy", 95), ("light", 113), ("light2", 126),
        ("hit", 330), ("ko", 258),
    ],
    "zim": [
        ("idle", 1), ("land", 50), ("jump", 62), ("walk", 71),
        ("dash", 86), ("heavy", 95), ("light", 113), ("light2", 126),
        ("hit", 330), ("ko", 258),
    ],
}

ONESHOT = set((
    "combo1", "combo2", "combo3", "ranged", "air",
    "downatk", "fall", "recover", "damage", "dash", "teleport",
))

POSE_NAMES = [
    "idle", "land", "jump", "walk", "dash",
    "heavy", "light", "light2", "hit", "ko",
]

ANIM_NAMES = [
    "idle", "land", "jump", "forward", "dash", "shield",
    "combo1", "combo2", "combo3", "ranged", "air", "downatk",
    "fall", "fallen", "recover", "win", "teleport", "damage",
]
