import re
import matplotlib.pyplot as plt
import pandas as pd

# === 1. Считываем файл ===
with open("../logs/trades.log", "r") as f:
    lines = f.readlines()

# === 2. Разбираем строки через регулярное выражение ===
pattern = re.compile(
    r"\[(.*?)\]\s+TRADE\s+(\d+)->(\d+)\s+qty=(\d+)\s+price=([\d.]+)"
)

records = []
for line in lines:
    m = pattern.search(line)
    if m:
        time, buy_id, sell_id, qty, price = m.groups()
        records.append({
            "time": time,
            "buy_id": int(buy_id),
            "sell_id": int(sell_id),
            "quantity": int(qty),
            "price": float(price)
        })

# === 3. Создаём DataFrame ===
df = pd.DataFrame(records)

if df.empty:
    print("⚠️ В файле trades.log нет распознанных сделок.")
    exit()

# === 4. Строим графики ===
plt.figure(figsize=(10, 6))

plt.subplot(2, 1, 1)
plt.plot(df.index, df["price"], marker="o", linestyle="-", color="blue")
plt.title("Динамика цен сделок")
plt.xlabel("Номер сделки")
plt.ylabel("Цена")
plt.grid(True)

plt.subplot(2, 1, 2)
plt.bar(df.index, df["quantity"], color="green")
plt.title("Объём сделок (quantity)")
plt.xlabel("Номер сделки")
plt.ylabel("Количество")
plt.tight_layout()

plt.show()

# === 5. Сохраняем как CSV (для Excel/анализа) ===
df.to_csv("trades_parsed.csv", index=False)
print(f"✅ Обработано {len(df)} сделок. Данные сохранены в trades_parsed.csv")
