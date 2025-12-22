#!/bin/bash

# Пути
TOKENS_DIR="/root/infpoisk/my_lab/data/tokens"
STEMS_DIR="/root/infpoisk/my_lab/data/stems"
STEMMER_BIN="./stemmer"

# Создаем папку для стемов
mkdir -p "$STEMS_DIR"

# Собираем программу
make

echo "Starting stemming..."
start_time=$(date +%s)

# Обрабатываем каждый файл
count=0
for file in "$TOKENS_DIR"/*.txt; do
    filename=$(basename "$file")
    "$STEMMER_BIN" "$file" "$STEMS_DIR/$filename"
    ((count++))
    
    if (( count % 1000 == 0 )); then
        echo "Processed $count files..."
    fi
done

end_time=$(date +%s)
duration=$((end_time - start_time))

echo "Finished! Processed $count files in $duration seconds."
