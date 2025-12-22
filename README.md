# Поисковая система (Лабораторные работы)

Проект обернут в `uv` для управления зависимостями и виртуальным окружением.

## Установка

Если у вас не установлен `uv`, установите его:
```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

Затем установите зависимости проекта:
```bash
uv sync
```

## Запуск компонентов

### 1. Сбор корпуса (Crawler)
```bash
uv run python lab01_02_corpus_crawler/crawler.py
```

### 2. Токенизация (C++)
```bash
cd lab03_tokenization && make && ./tokenizer ../data/corpus/doc_00001.txt ../data/tokens/doc_00001.txt
```

### 3. Стемминг (C++)
```bash
cd lab04_stemming && make && ./stemmer ../data/tokens/doc_00001.txt ../data/stems/doc_00001.txt
```

### 4. Анализ Ципфа (Python)
```bash
uv run python lab05_zipf/zipf_analysis.py
```

### 5. Индексация (C++)
```bash
cd lab07_bool_index && make && ./indexer ../data/stems ../data/index.bin
```

### 6. Поиск (C++)
```bash
cd lab08_bool_search && make && ./searcher ../data/index.bin
```

## Структура проекта
- `data/` — корпус, токены, стеммы и индекс.
- `lab01_02_corpus_crawler/` — скрипт для сбора данных.
- `lab03_tokenization/` — токенизатор на C++.
- `lab04_stemming/` — стеммер на C++.
- `lab05_zipf/` — анализ распределения слов.
- `lab07_bool_index/` — построение инвертированного индекса.
- `lab08_bool_search/` — поисковый движок.
