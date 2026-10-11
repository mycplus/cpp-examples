"""shipping_procedural.py - the procedural version: one function decides
the cost by checking which kind of shipping was chosen."""


def shipping_cost_cents(kind: str, kg: int) -> int:
    if kind == "standard":
        return 400 + 50 * kg
    if kind == "express":
        return 1000 + 100 * kg
    if kind == "pickup":
        return 0
    raise ValueError(f"unknown shipping kind: {kind}")


def money(cents: int) -> str:
    return f"{cents // 100}.{cents % 100:02d}"


for kind in ("standard", "express", "pickup"):
    print(f"{kind:<8} 2 kg: {money(shipping_cost_cents(kind, 2))}")
