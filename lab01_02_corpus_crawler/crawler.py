import os
import requests
from bs4 import BeautifulSoup
import time
from tqdm import tqdm
import re
from urllib.parse import urljoin

class HabrAICrawler:
    def __init__(self, output_dir, max_docs=30000):
        self.output_dir = output_dir
        self.state_file = os.path.join(os.path.dirname(output_dir), "visited_urls.txt")
        self.max_docs = max_docs
        
        if not os.path.exists(self.output_dir):
            os.makedirs(self.output_dir)

        self.visited_articles = self._load_state()
        self.doc_count = self._count_existing_docs()
        
        self.hubs = [
            "artificial_intelligence",
            "machine_learning",
            "data_mining",
            "python",
            "algorithms",
            "image_processing",
            "neural_networks",
            "data_science",
            "natural_language_processing"
        ]

        self.headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"
        }

    def _load_state(self):
        if os.path.exists(self.state_file):
            with open(self.state_file, 'r', encoding='utf-8') as f:
                return set(line.strip() for line in f if line.strip())
        return set()

    def _save_url(self, url):
        with open(self.state_file, 'a', encoding='utf-8') as f:
            f.write(url + '\n')

    def _count_existing_docs(self):
        if not os.path.exists(self.output_dir):
            return 0
        files = [f for f in os.listdir(self.output_dir) if f.startswith("doc_") and f.endswith(".txt")]
        return len(files)

    def get_clean_text(self, soup):
        if not soup:
            return ""
        for element in soup(["script", "style", "div.tm-article-presenter__meta", "div.tm-article-blocks"]):
            element.decompose()
        
        text = soup.get_text(separator=' ')
        text = re.sub(r'\s+', ' ', text).strip()
        return text

    def is_ai_related(self, text, title):
        keywords = [
            'ии', 'ai', 'llm', 'gpt', 'нейросеть', 'machine learning', 'обучение', 
            'агент', 'agent', 'модель', 'transformer', 'bert', 'pytorch', 'tensorflow',
            'nlp', 'cv', 'deep learning', 'reinforcement', 'langchain', 'llama', 
            'mistral', 'rag', 'embeddings', 'векторный поиск', 'fine-tuning'
        ]
        content = (text + " " + title).lower()
        return any(kw in content for kw in keywords)

    def crawl_arxiv(self, pbar, quota=8000):
        target = self.doc_count + quota
        if target > self.max_docs:
            target = self.max_docs

        print(f"\n[INFO] Starting Arxiv crawl (target: {quota} docs)...")
        base_url = "http://export.arxiv.org/api/query?"
        start = 0
        batch_size = 100
        
        while self.doc_count < target:
            query = f"search_query=cat:cs.AI&start={start}&max_results={batch_size}&sortBy=submittedDate&sortOrder=descending"
            try:
                resp = requests.get(base_url + query, timeout=20)
                if resp.status_code != 200:
                    break
                
                soup = BeautifulSoup(resp.text, 'xml')
                entries = soup.find_all('entry')
                
                if not entries:
                    break
                
                for entry in entries:
                    url = entry.id.text.strip()
                    if url in self.visited_articles:
                        continue
                    
                    title = entry.title.text.strip()
                    summary = entry.summary.text.strip()
                    
                    if len(summary) > 300:
                        file_path = os.path.join(self.output_dir, f"doc_{self.doc_count:05d}.txt")
                        with open(file_path, 'w', encoding='utf-8') as f:
                            f.write(f"URL: {url}\n")
                            f.write(f"Title: {title}\n\n")
                            f.write(summary)
                        
                        self.visited_articles.add(url)
                        self._save_url(url)
                        self.doc_count += 1
                        pbar.update(1)
                        
                        if self.doc_count >= self.max_docs:
                            break
                
                start += batch_size
                time.sleep(1) # Arxiv API rate limit is strict
            except Exception as e:
                print(f"Error crawling Arxiv: {e}")
                break

    def crawl_techcrunch(self, pbar, quota=8000):
        """Дополнительный источник: TechCrunch (AI category)"""
        target = self.doc_count + quota
        if target > self.max_docs:
            target = self.max_docs
            
        print(f"\n[INFO] Starting TechCrunch crawl (target: {quota} docs)...")
        page = 1
        while self.doc_count < target:
            url = f"https://techcrunch.com/category/artificial-intelligence/page/{page}/"
            try:
                resp = requests.get(url, headers=self.headers, timeout=10)
                if resp.status_code != 200:
                    break
                
                soup = BeautifulSoup(resp.text, 'lxml')
                # Ссылки на статьи
                links = soup.find_all('a', class_='loop-card__title-link')
                
                if not links:
                    break
                
                for link in links:
                    article_url = link['href']
                    if article_url in self.visited_articles:
                        continue
                    
                    self.visited_articles.add(article_url)
                    self._save_url(article_url)
                    
                    self.download_techcrunch_article(article_url, pbar)
                    
                    if self.doc_count >= target:
                        break
                
                page += 1
                time.sleep(0.5)
            except Exception as e:
                print(f"Error crawling TechCrunch: {e}")
                break

    def download_techcrunch_article(self, url, pbar):
        try:
            resp = requests.get(url, headers=self.headers, timeout=10)
            if resp.status_code != 200:
                return
            
            soup = BeautifulSoup(resp.text, 'lxml')
            title = soup.find('h1', class_='wp-block-post-title')
            body = soup.find('div', class_='entry-content')
            
            if title and body:
                text = self.get_clean_text(body)
                if len(text) > 500:
                    file_path = os.path.join(self.output_dir, f"doc_{self.doc_count:05d}.txt")
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(f"URL: {url}\n")
                        f.write(f"Title: {title.text.strip()}\n\n")
                        f.write(text)
                    
                    self.doc_count += 1
                    pbar.update(1)
        except:
            pass

    def crawl(self):
        pbar = tqdm(total=self.max_docs, desc="Downloading AI Corpus", initial=self.doc_count)
        
        # 1. Habr (до 14к)
        if self.doc_count < 14000:
            for hub in self.hubs:
                if self.doc_count >= 14000: break
                page = 1
                while self.doc_count < 14000:
                    hub_url = f"https://habr.com/ru/hubs/{hub}/articles/page{page}/"
                    try:
                        response = requests.get(hub_url, headers=self.headers, timeout=10)
                        if response.status_code != 200: break
                        soup = BeautifulSoup(response.text, 'lxml')
                        links = soup.find_all('a', class_='tm-title__link')
                        if not links: break
                        for link in links:
                            article_url = urljoin("https://habr.com", link['href'])
                            if article_url not in self.visited_articles:
                                self.visited_articles.add(article_url)
                                self._save_url(article_url)
                                self.download_article(article_url, pbar)
                                if self.doc_count >= 14000: break
                                time.sleep(0.1)
                        page += 1
                    except: break

        # 2. Arxiv (8000 документов)
        if self.doc_count < 22000:
            self.crawl_arxiv(pbar, quota=8000)
        
        # 3. TechCrunch (остальное до 30к)
        if self.doc_count < self.max_docs:
            self.crawl_techcrunch(pbar, quota=(self.max_docs - self.doc_count))
                    
        pbar.close()
        print(f"Finished. Total documents: {self.doc_count}")

    def download_article(self, url, pbar):
        try:
            resp = requests.get(url, headers=self.headers, timeout=10)
            if resp.status_code != 200:
                print(f"\n[DEBUG] Failed to fetch {url}: Status {resp.status_code}")
                return
            
            soup = BeautifulSoup(resp.text, 'lxml')
            # Пробуем разные селекторы, так как Хабр иногда меняет верстку
            title_tag = soup.find('h1', class_='tm-title') or soup.find('h1', class_='tm-article-snippet__title')
            body_tag = soup.find('div', class_='tm-article-body') or soup.find('div', id='post-content-body')
            
            if not title_tag:
                print(f"\n[DEBUG] Title not found for {url}")
                return
            if not body_tag:
                print(f"\n[DEBUG] Body not found for {url}")
                return
                
            title = title_tag.text.strip()
            text = self.get_clean_text(body_tag)
            
            is_ai = self.is_ai_related(text, title)
            if len(text) > 500 and is_ai:
                file_path = os.path.join(self.output_dir, f"doc_{self.doc_count:05d}.txt")
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(f"URL: {url}\n")
                    f.write(f"Title: {title}\n\n")
                    f.write(text)
                
                self.doc_count += 1
                pbar.update(1)
            else:
                if len(text) <= 500:
                    print(f"\n[DEBUG] Article too short ({len(text)} chars): {url}")
                if not is_ai:
                    print(f"\n[DEBUG] Not AI related: {title}")
        except Exception as e:
            print(f"\n[DEBUG] Exception downloading {url}: {e}")
            pass

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--max_docs", type=int, default=30000)
    args = parser.parse_args()

    OUTPUT = "/root/infpoisk/my_lab/data/corpus"
    crawler = HabrAICrawler(OUTPUT, max_docs=args.max_docs)
    crawler.crawl()
