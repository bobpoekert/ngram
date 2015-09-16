import subprocess
import gensim, logging
import os, re
from nltk.stem import WordNetLemmatizer

lemmatizer = WordNetLemmatizer()

logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s', level=logging.INFO)

def xz_stream_url(url):
    logging.info('fetching %s' % url)
    curl_proc = subprocess.Popen(['curl', url], stdout=subprocess.PIPE)
    proc = subprocess.Popen(['xzcat'], stdin=curl_proc.stdout, stdout=subprocess.PIPE)
    with proc.stdout:
        for row in proc.stdout:
            yield row


def sentences_to_words(sentences):
    for sentence in sentences:
        yield sentence.split()

commoncrawl_urls = ["en.2012.00.xz","en.2012.01.xz","en.2012.02.xz","en.2012.03.xz","en.2012.04.xz","en.2012.05.xz","en.2012.06.xz","en.2012.07.xz","en.2012.08.xz","en.2012.09.xz","en.2012.10.xz","en.2012.11.xz","en.2012.12.xz","en.2012.13.xz","en.2012.14.xz","en.2012.15.xz","en.2012.16.xz","en.2012.17.xz","en.2012.19.xz","en.2012.20.xz","en.2012.21.xz","en.2012.23.xz","en.2012.24.xz","en.2012.25.xz","en.2012.27.xz","en.2012.28.xz","en.2012.29.xz","en.2012.31.xz","en.2012.32.xz","en.2012.33.xz","en.2012.34.xz","en.2012.35.xz","en.2012.36.xz","en.2012.37.xz","en.2012.38.xz","en.2012.39.xz","en.2012.40.xz","en.2012.41.xz","en.2012.42.xz","en.2012.43.xz","en.2012.44.xz","en.2012.45.xz","en.2012.46.xz","en.2012.47.xz","en.2012.48.xz","en.2012.49.xz","en.2012.50.xz","en.2012.51.xz","en.2012.52.xz","en.2012.53.xz","en.2012.54.xz","en.2012.55.xz","en.2012.56.xz","en.2012.57.xz","en.2012.58.xz","en.2012.59.xz","en.2012.60.xz","en.2012.61.xz","en.2012.62.xz","en.2012.63.xz","en.2012.64.xz","en.2012.65.xz","en.2012.68.xz","en.2012.69.xz","en.2012.70.xz","en.2012.71.xz","en.2012.72.xz","en.2012.73.xz","en.2012.74.xz","en.2012.75.xz","en.2012.77.xz","en.2012.78.xz","en.2012.79.xz","en.2012.80.xz","en.2012.81.xz","en.2012.82.xz","en.2012.83.xz","en.2012.84.xz","en.2012.85.xz","en.2012.86.xz","en.2012.87.xz","en.2012.88.xz","en.2012.89.xz","en.2012.90.xz","en.2012.91.xz","en.2012.92.xz","en.2012.93.xz","en.2012.94.xz","en.2012.95.xz","en.2012.96.xz","en.2012.97.xz","en.2012.98.xz","en.2012.99.xz","en.2013.00.xz","en.2013.01.xz","en.2013.02.xz","en.2013.03.xz","en.2013.04.xz","en.2013.05.xz","en.2013.06.xz","en.2013.07.xz","en.2013.08.xz","en.2013.09.xz","en.2013.10.xz","en.2013.11.xz","en.2013.12.xz","en.2013.13.xz","en.2013.14.xz","en.2013.15.xz","en.2013.16.xz","en.2013.17.xz","en.2013.18.xz","en.2013.19.xz","en.2013.20.xz","en.2013.21.xz","en.2013.22.xz","en.2013.23.xz","en.2013.24.xz","en.2013.25.xz","en.2013.26.xz","en.2013.27.xz","en.2013.28.xz","en.2013.29.xz","en.2013.31.xz","en.2013.32.xz","en.2013.33.xz","en.2013.34.xz","en.2013.35.xz","en.2013.36.xz","en.2013.37.xz","en.2013.38.xz","en.2013.39.xz","en.2013.40.xz","en.2013.41.xz","en.2013.42.xz","en.2013.43.xz","en.2013.44.xz","en.2013.45.xz","en.2013.46.xz","en.2013.47.xz","en.2013.48.xz","en.2013.49.xz","en.2013.50.xz","en.2013.51.xz","en.2013.52.xz","en.2013.53.xz","en.2013.54.xz","en.2013.55.xz","en.2013.57.xz","en.2013.58.xz","en.2013.59.xz","en.2013.60.xz","en.2013.61.xz","en.2013.62.xz","en.2013.63.xz","en.2013.64.xz","en.2013.65.xz","en.2013.66.xz","en.2013.67.xz","en.2013.68.xz","en.2013.69.xz","en.2013.70.xz","en.2013.71.xz","en.2013.72.xz","en.2013.73.xz","en.2013.74.xz","en.2013.75.xz","en.2013.76.xz","en.2013.77.xz","en.2013.78.xz","en.2013.79.xz","en.2013.80.xz","en.2013.81.xz","en.2013.82.xz","en.2013.83.xz","en.2013.84.xz","en.2013.85.xz","en.2013.86.xz","en.2013.87.xz","en.2013.88.xz","en.2013.89.xz","en.2013.90.xz","en.2013.91.xz","en.2013.92.xz","en.2013.93.xz","en.2013.94.xz","en.2013.95.xz","en.2013.96.xz","en.2013.97.xz","en.2013.98.xz","en.2013.99.xz","en.2014.00.xz","en.2014.01.xz","en.2014.02.xz","en.2014.03.xz","en.2014.04.xz","en.2014.05.xz","en.2014.06.xz","en.2014.07.xz","en.2014.09.xz","en.2014.10.01.xz","en.2014.10.02.xz","en.2014.10.03.xz","en.2014.10.04.xz","en.2014.10.05.xz","en.2014.10.06.xz","en.2014.10.07.xz","en.2014.10.08.xz","en.2014.10.10.xz","en.2014.10.11.xz","en.2014.10.12.xz","en.2014.10.13.xz","en.2014.10.14.xz","en.2014.10.15.xz","en.2014.10.16.xz","en.2014.10.17.xz","en.2014.10.18.xz","en.2014.10.19.xz","en.2014.10.20.xz","en.2014.10.21.xz","en.2014.10.22.xz","en.2014.10.23.xz","en.2014.10.24.xz","en.2014.10.25.xz","en.2014.10.26.xz","en.2014.10.27.xz","en.2014.10.28.xz","en.2014.10.29.xz","en.2014.10.30.xz","en.2014.10.31.xz","en.2014.10.32.xz","en.2014.10.33.xz","en.2014.10.34.xz","en.2014.10.36.xz","en.2014.10.37.xz","en.2014.10.38.xz","en.2014.10.39.xz","en.2014.10.40.xz","en.2014.10.41.xz","en.2014.10.42.xz","en.2014.10.44.xz","en.2014.10.45.xz","en.2014.10.46.xz","en.2014.10.47.xz","en.2014.10.48.xz","en.2014.10.49.xz","en.2014.10.50.xz","en.2014.10.51.xz","en.2014.10.53.xz","en.2014.10.54.xz","en.2014.10.55.xz","en.2014.10.56.xz","en.2014.10.57.xz","en.2014.10.58.xz","en.2014.10.59.xz","en.2014.10.60.xz","en.2014.10.61.xz","en.2014.10.62.xz","en.2014.10.63.xz","en.2014.10.64.xz","en.2014.10.65.xz","en.2014.10.66.xz","en.2014.10.67.xz","en.2014.10.68.xz","en.2014.10.69.xz","en.2014.10.70.xz","en.2014.10.71.xz","en.2014.10.73.xz","en.2014.10.75.xz","en.2014.10.76.xz","en.2014.10.77.xz","en.2014.10.78.xz","en.2014.10.79.xz","en.2014.10.80.xz","en.2014.10.81.xz","en.2014.10.82.xz","en.2014.10.83.xz","en.2014.10.84.xz","en.2014.10.85.xz","en.2014.10.86.xz","en.2014.10.87.xz","en.2014.10.88.xz","en.2014.10.89.xz","en.2014.10.90.xz","en.2014.10.92.xz","en.2014.10.93.xz","en.2014.10.94.xz","en.2014.10.95.xz","en.2014.10.96.xz","en.2014.10.97.xz","en.2014.10.98.xz","en.2014.10.99.xz","en.2014.10.xz","en.2014.11.xz","en.2014.12.xz","en.2014.13.xz","en.2014.14.xz","en.2014.15.xz","en.2014.16.xz","en.2014.17.xz","en.2014.18.xz","en.2014.19.xz","en.2014.20.xz","en.2014.21.xz","en.2014.22.xz","en.2014.23.xz","en.2014.24.xz","en.2014.25.xz","en.2014.26.xz","en.2014.27.xz","en.2014.28.xz","en.2014.29.xz","en.2014.30.xz","en.2014.31.xz","en.2014.32.xz","en.2014.33.xz","en.2014.34.xz","en.2014.35.xz","en.2014.36.xz","en.2014.37.xz","en.2014.38.xz","en.2014.39.xz","en.2014.40.xz","en.2014.41.xz","en.2014.42.xz","en.2014.43.xz","en.2014.44.xz","en.2014.45.xz","en.2014.46.xz","en.2014.47.xz","en.2014.48.xz","en.2014.49.xz","en.2014.50.xz","en.2014.51.xz","en.2014.52.xz","en.2014.53.xz","en.2014.54.xz","en.2014.55.xz","en.2014.56.xz","en.2014.57.xz","en.2014.58.xz","en.2014.59.xz","en.2014.61.xz","en.2014.62.xz","en.2014.63.xz","en.2014.64.xz","en.2014.65.xz","en.2014.66.xz","en.2014.67.xz","en.2014.68.xz","en.2014.69.xz","en.2014.70.xz","en.2014.71.xz","en.2014.72.xz","en.2014.73.xz","en.2014.74.xz","en.2014.75.xz","en.2014.76.xz","en.2014.77.xz","en.2014.78.xz","en.2014.79.xz","en.2014.80.xz","en.2014.81.xz","en.2014.82.xz","en.2014.83.xz","en.2014.84.xz","en.2014.85.xz","en.2014.86.xz","en.2014.87.xz","en.2014.88.xz","en.2014.89.xz","en.2014.90.xz","en.2014.91.xz","en.2014.92.xz","en.2014.93.xz","en.2014.94.xz","en.2014.95.xz","en.2014.96.xz","en.2014.97.xz","en.2014.98.xz","en.2014.99.xz"]
commoncrawl_urls = ['http://data.statmt.org/ngrams/raw_en/%s' % v for v in commoncrawl_urls]

def all_sentences(max_rows=None):
    row_count = 0
    for url in commoncrawl_urls:
        for row in sentences_to_words(xz_stream_url(url)):
            yield row
            row_count += 1
            if max_rows is not None and row_count > max_rows:
                return

def dump_sentences(out_dir):
    for url in commoncrawl_urls:
        subprocess.call(['curl', url, '-o', '%s/%s' % (out_dir, url.split('/')[-1])])

#phrase_model = gensim.models.Phrases.load('commoncrawl_phrases.model')

def sentences_from_dir(d):
    ctr = 0
    for fname in os.listdir(d):
        if '.xz' in fname:
            proc = subprocess.Popen(['xzcat', os.path.join(d, fname)], stdout=subprocess.PIPE)
            for row in proc.stdout:
                try:
                    row = gensim.utils.any2utf8(row)
                except:
                    continue
                yield [v.lower() for v in re.split(r'[^a-zA-Z0-9\-]', row)]
                ctr += 1

def build_model(get_sentences, outf, workers=7):
    model = gensim.models.Word2Vec(workers=workers)
    logging.info('building vocab')
    model.build_vocab(get_sentences())
    model.train(get_sentences())
    model.save(outf)
    return model

if __name__ == '__main__':
    import sys
    from functools import partial
    outfname = sys.argv[1]
    try:
        max_rows = int(sys.argv[2])
    except IndexError:
        max_rows = None
    model = build_model(partial(sentences_from_dir, 'commoncrawl_sentences'), outfname)

