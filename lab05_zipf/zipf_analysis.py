import os
import collections
import matplotlib.pyplot as plt
from tqdm import tqdm

def analyze_zipf(stems_dir, output_plot):
    word_counts = collections.Counter()
    
    files = [f for f in os.listdir(stems_dir) if f.endswith('.txt')]
    print(f"Analyzing {len(files)} files for Zipf's Law...")
    
    for filename in tqdm(files):
        file_path = os.path.join(stems_dir, filename)
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                words = f.read().split()
                word_counts.update(words)
        except Exception as e:
            print(f"Error reading {filename}: {e}")

    # Сортируем по частоте
    sorted_counts = word_counts.most_common()
    
    # Сохраняем топ-50 в текстовый файл для отчета
    with open("top_words.txt", "w", encoding="utf-8") as f:
        for i, (word, count) in enumerate(sorted_counts[:50], 1):
            f.write(f"{i}. {word}: {count}\n")

    # Данные для графика
    ranks = range(1, len(sorted_counts) + 1)
    frequencies = [count for word, count in sorted_counts]

    plt.figure(figsize=(10, 6))
    plt.plot(ranks, frequencies)
    plt.title("Zipf's Law Analysis")
    plt.xlabel("Rank")
    plt.ylabel("Frequency")
    plt.grid(True)
    
    # Логарифмический масштаб (в нем закон Ципфа выглядит как прямая)
    plt.figure(figsize=(10, 6))
    plt.loglog(ranks, frequencies)
    plt.title("Zipf's Law Analysis (Log-Log scale)")
    plt.xlabel("Log(Rank)")
    plt.ylabel("Log(Frequency)")
    plt.grid(True)
    
    plt.savefig(output_plot)
    print(f"Plot saved to {output_plot}")
    print(f"Top 50 words saved to top_words.txt")

if __name__ == "__main__":
    STEMS_DIR = "/root/infpoisk/my_lab/data/stems"
    OUTPUT_PLOT = "zipf_plot.png"
    analyze_zipf(STEMS_DIR, OUTPUT_PLOT)
