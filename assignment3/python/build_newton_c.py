import cmath
import math

def get_root(deg: int, root: int) -> complex:
    return cmath.exp(2 * math.pi * root / deg * 1j)

def build_fast_check(up_to_deg: int) -> str:

    def _build_if(deg: int) -> str:
        s = ""
        for r in range(deg // 2 + 1):
            root = get_root(deg, r)
            s += f"    double complex zt{r} = z - CMPLX({root.real:.14f}, {root.imag:.14f});\n"
            s += f"    if (fast_abs(zt{r}, LOWER_BOUND)) {'{'}\n"
            s += f"        r->attr = ZERO_ATTR + 1 + {r}"
            if not (r == 0 or (r == (deg // 2) and deg % 2 == 0)):
                s+= f" + has_neg_im * {deg // 2 + 1}"
            s += "\n        return 1;\n"
        return s

    def _build_case(deg: int) -> str:
        case_str = f"case {deg}:\n"
        case_str += "    int has_neg_im = cimag(z) < 0;\n"
        case_str += "    if (has_neg_im) z = conj(z);\n"
        case_str += _build_if(deg)
        case_str += "    break;\n"

        return case_str

    switch = "switch (degree) {\n"
    for deg in range(1, up_to_deg + 1):
        switch += _build_case(deg)
    switch += "}\n"
    return switch


print(build_fast_check(3))

