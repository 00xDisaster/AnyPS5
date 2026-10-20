import re
import sys


def load(path):
    units = set()
    for line in open(path, encoding="latin1"):
        m = re.match(r"0x[0-9A-Fa-f]+\s+0x([0-9A-Fa-f]{4,6})", line)
        if m and int(m.group(1), 16) < 0x10000:
            units.add(int(m.group(1), 16))
    return units


def unit_range(first, last):
    return set(range(first, last + 1))


def build_a(cp932):
    units = set(cp932)
    units -= unit_range(0xE000, 0xE757) | {0x80} | unit_range(0xF8F0, 0xF8F3)
    units |= unit_range(0xA0, 0xFF) | {0x2014, 0x2016, 0x203E, 0x2212, 0x301C}
    return units


def build_b(cp936):
    units = set(cp936)
    units |= unit_range(0x80, 0xFF) | {0x1F9, 0x2E84, 0x2E88, 0x2E8B, 0x2E8C, 0x2E97, 0x2EA7, 0x2EAA}
    units |= {0x2EB6, 0x2EB7, 0x2EBB, 0x303E} | unit_range(0x2FF0, 0x2FFB)
    ext_a = unit_range(0x3400, 0x4DB5) - {0x3918, 0x39CF, 0x39DF, 0x415F, 0x4337, 0x4729, 0x477C, 0x478D, 0x4C9F, 0x4CA0, 0x4CA3}
    units |= ext_a | unit_range(0xE766, 0xE864)
    return units


def ranges(units):
    out = []
    start = prev = None
    for u in sorted(units):
        if start is None:
            start = prev = u
        elif u == prev + 1:
            prev = u
        else:
            out.append((start, prev))
            start = prev = u
    out.append((start, prev))
    return out


def emit(a, b):
    words = []
    for w in range(1024):
        v = 0
        for bit in range(64):
            if (w * 64 + bit) in a:
                v |= 1 << bit
        words.append(v)
    rb = ranges(b)
    out = ["#pragma once", "#include <cstdint>", "", "namespace ime_filter {", ""]
    out.append("inline constexpr std::uint64_t kCodepageABitmap[1024] = {")
    for i in range(0, 1024, 4):
        out.append("    " + ", ".join("0x%016xull" % x for x in words[i:i + 4]) + ",")
    out.append("};")
    out.append("")
    out.append("struct CodepageRange {\n    std::uint16_t first;\n    std::uint16_t last;\n};")
    out.append("")
    out.append("inline constexpr CodepageRange kCodepageBRanges[%d] = {" % len(rb))
    for i in range(0, len(rb), 6):
        out.append("    " + ", ".join("{0x%04x, 0x%04x}" % r for r in rb[i:i + 6]) + ",")
    out.append("};")
    out.append("")
    out.append("}")
    return "\n".join(out) + "\n"


if __name__ == "__main__":
    mappings, target = sys.argv[1], sys.argv[2]
    open(target, "w", newline="").write(emit(build_a(load(mappings + "/CP932.TXT")), build_b(load(mappings + "/CP936.TXT"))))
