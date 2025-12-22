#!/bin/bash

# Пути
CORPUS_DIR="/root/infpoisk/my_lab/data/corpus"
OUTPUT_DIR="/root/infpoisk/my_lab/data/tokens"
TOKENIZER_BIN="./tokenizer"

# Создаем папку для токенов
mkdir -p "$OUTPUT_DIR"

# Собираем программу
make

echo "Starting tokenization..."
start_time=$(date +%s)

# Обрабатываем каждый файл
count=0
for file in "$CORPUS_DIR"/*.txt; do
    filename=$(basename "$file")
    "$TOKENIZER_BIN" "$file" "$OUTPUT_DIR/$filename"
    ((count++))
    
    if (( count % 1000 == 0 )); then
        echo "Processed $count files..."
    fi
done

end_time=$(date +%s)
duration=$((end_time - start_time))

echo "Finished! Processed $count files in $duration seconds."
