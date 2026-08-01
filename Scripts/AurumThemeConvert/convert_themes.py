#!/usr/bin/env python3
"""Convert opencode desktop-theme.json palette files to Aurum (sgcl runtime) theme JSON.

Faithful port of the opencode desktop-theme resolver
(ElectronTheme-WithTest/src/renderer/theme/{color,resolve}.ts): generates
OKLCH-based 12-step neutral/color scales with per-scheme (dark/light) curves,
then maps the resolved tokens onto the Aurum schema (surface/gold/semantic).
"""
import argparse
import copy
import json
import math
import sys
from pathlib import Path

DEFAULT_THEME = {
    "meta": {"name": "Modern Dark Gold", "version": "1.0.0"},
    "surface": {
        "normalTop": "#2D3035", "normalBottom": "#111318",
        "hoverTop": "#3A3D44", "hoverBottom": "#191C21",
        "pressedTop": "#101217", "pressedBottom": "#262A30",
        "disabledTop": "#3A3C40", "disabledBottom": "#292B2F",
        "insetTop": "#0C0E12", "insetBottom": "#181B20",
    },
    "gold": {
        "border": "#A98A4A", "hover": "#D0AF63", "highlight": "#D8C078",
        "bright": "#F0D995", "shadow": "#675126", "muted": "#74674C",
    },
    "gloss": {
        "tint": "#FFF8E2", "normalAlpha": 48, "hoverMultiplier": 1.32,
        "disabledMultiplier": 0.30, "heightRatio": 0.45, "blur": 1.15, "centerX": 0.42,
    },
    "semantic": {
        "text": "#F2F0EA", "disabledText": "#85837D", "danger": "#B76367",
        "success": "#6AA986", "warning": "#C69B50", "focus": "#D8C078",
    },
    "geometry": {"radius": 8, "borderWidth": 1, "innerRim": 0.65, "depth": 1.6, "shadow": 2},
    "state": {"hoverLift": 10, "checkedGoldMix": 30, "disabledContrast": 42},
}


# ---------- legacy 0-255 color helpers (studio mix()) ----------

def hex_to_rgb(h):
    h = h.lstrip("#")
    if len(h) == 3:
        h = "".join(c * 2 for c in h)
    return [int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16)]


def rgb_to_hex(c):
    return "#" + "".join(format(max(0, min(255, int(v))), "02x") for v in c)


def mix(a, b, p):
    A = hex_to_rgb(a)
    B = hex_to_rgb(b)
    t = p / 100
    return rgb_to_hex([math.floor(A[i] * (1 - t) + B[i] * t + 0.5) for i in range(3)])


# ---------- OKLCH color math (color.ts port) ----------

def _clamp(v, lo, hi):
    return max(lo, min(hi, v))


def _hue(v):
    return ((v % 360) + 360) % 360


def _hex_to_rgb_f(h):
    h = h.lstrip("#")
    if len(h) == 3 or len(h) == 4:
        h = "".join(c * 2 for c in h)
    num = int(h[:6], 16)
    return [(num >> 16 & 255) / 255.0, (num >> 8 & 255) / 255.0, (num & 255) / 255.0]


def _rgb_f_to_hex(c):
    def to_hex(v):
        return format(int(_clamp(v, 0, 1) * 255 + 0.5), "02x")

    return "#" + to_hex(c[0]) + to_hex(c[1]) + to_hex(c[2])


def _linear_to_srgb(c):
    return c * 12.92 if c <= 0.0031308 else 1.055 * c ** (1 / 2.4) - 0.055


def _srgb_to_linear(c):
    return c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4


def _rgb_to_oklch(r, g, b):
    lr, lg, lb = _srgb_to_linear(r), _srgb_to_linear(g), _srgb_to_linear(b)
    l_ = 0.4122214708 * lr + 0.5363325363 * lg + 0.0514459929 * lb
    m_ = 0.2119034982 * lr + 0.6806995451 * lg + 0.1073969566 * lb
    s_ = 0.0883024619 * lr + 0.2817188376 * lg + 0.6299787005 * lb
    l, m, s = l_ ** (1 / 3), m_ ** (1 / 3), s_ ** (1 / 3)
    L = 0.2104542553 * l + 0.793617785 * m - 0.0040720468 * s
    a = 1.9779984951 * l - 2.428592205 * m + 0.4505937099 * s
    bO = 0.0259040371 * l + 0.7827717662 * m - 0.808675766 * s
    C = math.sqrt(a * a + bO * bO)
    H = math.atan2(bO, a) * 180 / math.pi
    if H < 0:
        H += 360
    return (L, C, H)


def _oklch_to_rgb(L, C, H):
    a = C * math.cos(H * math.pi / 180)
    b = C * math.sin(H * math.pi / 180)
    l = L + 0.3963377774 * a + 0.2158037573 * b
    m = L - 0.1055613458 * a - 0.0638541728 * b
    s = L - 0.0894841775 * a - 1.291485548 * b
    l3, m3, s3 = l ** 3, m ** 3, s ** 3
    r = 4.0767416621 * l3 - 3.3077115913 * m3 + 0.2309699292 * s3
    g = -1.2684380046 * l3 + 2.6097574011 * m3 - 0.3413193965 * s3
    bO = -0.0041960863 * l3 - 0.7034186147 * m3 + 1.707614701 * s3
    return (_linear_to_srgb(r), _linear_to_srgb(g), _linear_to_srgb(bO))


def _hex_to_oklch(h):
    return _rgb_to_oklch(*_hex_to_rgb_f(h))


def _oklch_to_hex(L, C, H):
    return _rgb_f_to_hex(_oklch_to_rgb(L, C, H))


def _blend(color, background, alpha):
    fg = _hex_to_rgb_f(color)
    bg = _hex_to_rgb_f(background)
    return _rgb_f_to_hex([fg[i] * alpha + bg[i] * (1 - alpha) for i in range(3)])


def _shift(color, l=0.0, c=1.0, h=0.0):
    base = _hex_to_oklch(color)
    return _oklch_to_hex(base[0] + l, base[1] * c, base[2] + h)


def _mix_colors(c1, c2, amount):
    a = _hex_to_oklch(c1)
    b = _hex_to_oklch(c2)
    delta = ((b[2] - a[2]) % 360 + 540) % 360 - 180
    return _oklch_to_hex(a[0] + (b[0] - a[0]) * amount, a[1] + (b[1] - a[1]) * amount, a[2] + delta * amount)


# ---------- scale generators (color.ts port) ----------

def generate_scale(seed, is_dark):
    base = _hex_to_oklch(seed)
    if is_dark:
        light_steps = [
            0.118, 0.138, 0.167, 0.202, 0.246, 0.304, 0.378, 0.468,
            _clamp(base[0] * 0.825, 0.53, 0.705),
            _clamp(base[0] * 0.89, 0.61, 0.79),
            _clamp(base[0] + 0.033, 0.868, 0.943),
            0.984,
        ]
        chroma_mult = [0.52, 0.68, 0.86, 1.02, 1.14, 1.24, 1.36, 1.48, 1.56, 1.64, 1.62, 1.15]
    else:
        light_steps = [0.993, 0.983, 0.962, 0.936, 0.906, 0.866, 0.811, 0.74, base[0], max(0.0, base[0] - 0.036), 0.49, 0.27]
        chroma_mult = [0.12, 0.24, 0.46, 0.68, 0.84, 0.98, 1.08, 1.16, 1.22, 1.26, 1.18, 0.98]
    return [_oklch_to_hex(light_steps[i], base[1] * chroma_mult[i], base[2]) for i in range(12)]


def generate_neutral_scale(seed, is_dark, ink=None):
    if not is_dark and ink:
        base = _hex_to_oklch(seed)
        if base[0] < 0.5:
            bg = seed
        else:
            bg = _oklch_to_hex(max(base[0], 0.82), base[1], base[2])
        steps = [0, 0.022, 0.042, 0.068, 0.102, 0.146, 0.208, 0.296, 0.432, 0.61, 0.81, 0.965]
        return [_mix_colors(bg, ink, step) for step in steps]
    if ink:
        base = _hex_to_oklch(seed)

        def lift(tone):
            return _oklch_to_hex(base[0] + (1 - base[0]) * tone, base[1] * max(0.0, 1 - tone), base[2])

        def sink(tone):
            return _oklch_to_hex(base[0] * (1 - tone), base[1] * max(0.0, 1 - tone * (0.12 if is_dark else 0.3)), base[2])

        if is_dark:
            bg = sink(_clamp(0.19 + max(0.0, base[0] - 0.12) * 0.33 + base[1] * 1.95, 0.17, 0.27))
        else:
            bg = lift(0.86) if base[0] < 0.82 else lift(_clamp(0.1 + base[1] * 3.2 + max(0.0, 0.95 - base[0]) * 0.35, 0.1, 0.28))
        steps = (
            [0, 0.018, 0.039, 0.064, 0.097, 0.143, 0.212, 0.31, 0.46, 0.649, 0.845, 0.984]
            if is_dark else
            [0, 0.022, 0.042, 0.068, 0.102, 0.146, 0.208, 0.296, 0.432, 0.61, 0.81, 0.965]
        )
        return [_mix_colors(bg, ink, step) for step in steps]

    base = _hex_to_oklch(seed)
    chroma = min(base[1], 0.068 if is_dark else 0.04)
    light_steps = (
        [0.138, 0.156, 0.178, 0.202, 0.232, 0.272, 0.326, 0.404, _clamp(base[0] * 0.83, 0.43, 0.55), 0.596, 0.719, 0.956]
        if is_dark else
        [0.991, 0.979, 0.964, 0.946, 0.931, 0.913, 0.891, 0.83, base[0], 0.617, 0.542, 0.205]
    )
    return [_oklch_to_hex(l, chroma, base[2]) for l in light_steps]


def generate_neutral_alpha_scale(neutral_scale, is_dark):
    alphas = (
        [0.038, 0.066, 0.1, 0.142, 0.19, 0.252, 0.334, 0.446, 0.58, 0.718, 0.854, 0.985]
        if is_dark else
        [0.03, 0.06, 0.1, 0.145, 0.2, 0.265, 0.35, 0.47, 0.61, 0.74, 0.86, 0.97]
    )
    return [_blend(neutral_scale[11], neutral_scale[0], alpha) for alpha in alphas]


# ---------- theme conversion (resolve.ts port, mapped to Aurum schema) ----------

def theme_from_electron(desktop, mode="dark"):
    variant = desktop.get(mode) or desktop.get("dark") or desktop.get("light")
    palette = variant.get("palette")
    seeds = variant.get("seeds")
    overrides = variant.get("overrides") or {}
    is_dark = mode == "dark"

    compact = palette is not None
    if compact:
        neutral = palette["neutral"]
        ink = palette.get("ink")
        primary = palette["primary"]
        success = palette["success"]
        warning = palette["warning"]
        error = palette["error"]
        info = palette["info"]
        interactive = palette.get("interactive") or primary
    else:
        neutral = seeds["neutral"]
        ink = None
        primary = seeds["primary"]
        success = seeds["success"]
        warning = seeds["warning"]
        error = seeds["error"]
        info = seeds["info"]
        interactive = seeds["interactive"]

    nscale = generate_neutral_scale(neutral, is_dark, ink)
    sscale = generate_scale(success, is_dark)
    escale = generate_scale(error, is_dark)
    xscale = generate_scale(interactive, is_dark)
    amber_seed = _shift(
        warning,
        l=-0.058 if is_dark else -0.082,
        c=1.14 if is_dark else 0.94,
        h=-16 if is_dark else -22,
    )
    amber = generate_scale(amber_seed, is_dark)

    if compact:
        tint = _hex_to_oklch(ink)
        body = _shift(
            ink,
            l=max(0.0, 0.88 - tint[0]) * 0.4 if is_dark else -max(0.0, tint[0] - 0.18) * 0.24,
            c=1.04 if is_dark else 1.02,
        )
        text = body
        text_weak = _shift(body, l=-0.11 if is_dark else 0.11, c=0.9)
    else:
        text = nscale[10]
        text_weak = nscale[8]

    bg = nscale[0]
    bg_override = overrides.get("background-base")
    if isinstance(bg_override, str) and bg_override.startswith("#"):
        bg = bg_override

    if is_dark:
        surface = {
            "normalTop": nscale[3], "normalBottom": nscale[1],
            "hoverTop": nscale[4], "hoverBottom": nscale[2],
            "pressedTop": nscale[1], "pressedBottom": nscale[3],
            "disabledTop": nscale[2], "disabledBottom": nscale[1],
            "insetTop": nscale[1], "insetBottom": nscale[2],
        }
    else:
        surface = {
            "normalTop": nscale[0],
            "normalBottom": _blend(primary, nscale[3], 0.06),
            "hoverTop": nscale[0],
            "hoverBottom": _blend(primary, nscale[1], 0.03),
            "pressedTop": _blend(primary, nscale[5], 0.10),
            "pressedBottom": _blend(primary, nscale[2], 0.05),
            "disabledTop": nscale[2],
            "disabledBottom": nscale[5],
            "insetTop": nscale[6],
            "insetBottom": nscale[4],
        }

    t = copy.deepcopy(DEFAULT_THEME)
    t["meta"] = {
        "name": desktop["name"], "id": desktop["id"],
        "source": "ElectronTheme", "scheme": mode, "version": "2.0.0",
    }
    t["surface"].update(surface)
    t["gold"].update({
        "border": primary,
        "hover": mix(primary, "#ffffff", 18 if is_dark else 8),
        "highlight": mix(primary, "#ffffff", 38 if is_dark else 20),
        "bright": mix(primary, "#ffffff", 58 if is_dark else 34),
        "shadow": mix(primary, "#000000", 58 if is_dark else 38),
        "muted": mix(primary, bg, 58),
    })

    text_override = overrides.get("text-base")
    if isinstance(text_override, str) and text_override.startswith("#"):
        text = text_override
    weak_override = overrides.get("text-weak")
    if isinstance(weak_override, str) and weak_override.startswith("#"):
        text_weak = weak_override

    t["semantic"].update({
        "text": text,
        "disabledText": text_weak,
        "danger": escale[8 if is_dark else 9],
        "success": sscale[8 if is_dark else 9],
        "warning": amber[8 if is_dark else 9],
        "focus": xscale[8],
    })

    t["gloss"]["tint"] = mix(ink, "#ffffff", 42) if is_dark else "#FFFFFF"
    t["gloss"]["normalAlpha"] = round((38 if is_dark else 24) / 255.0, 4)

    return t


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--src", required=True, help="Directory containing opencode theme JSON files")
    ap.add_argument("--out", required=True, help="Output directory for converted {id}.{mode}.json files")
    args = ap.parse_args()

    src_dir = Path(args.src)
    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    files = sorted(src_dir.glob("*.json"))
    written = 0
    for path in files:
        try:
            desktop = json.loads(path.read_text(encoding="utf-8"))
        except Exception as e:
            print(f"[SKIP] {path.name}: invalid JSON ({e})")
            continue
        if not isinstance(desktop, dict) or not desktop.get("palette") and not desktop.get("dark") and not desktop.get("light"):
            print(f"[SKIP] {path.name}: not a desktop-theme file")
            continue
        tid = desktop.get("id") or path.stem
        for mode in ("dark", "light"):
            result = theme_from_electron(desktop, mode)
            out_path = out_dir / f"{tid}.{mode}.json"
            out_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            written += 1
            print(f"[OK]   {out_path.name}")
    print(f"Done: {written} files -> {out_dir}")


if __name__ == "__main__":
    sys.exit(main())
