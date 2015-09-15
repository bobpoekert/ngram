import countmin
import train_commoncrawl_word2vec as tr
import gensim
import cPickle as pickle

phrase_model = gensim.models.Phrases.load('commoncrawl_phrases.model')

if __name__ == '__main__':
    import sys
    outf = sys.argv[1]
    sketch = countmin.Sketch()
    total = 0
    total_count = 0
    total_max = 0
    try:
        for sentence in tr.sentences_from_dir('commoncrawl_sentences'):
            phrases = phrase_model[sentence]
            for phrase in phrases:
                sketch.update(phrase, 1)
                cnt = sketch.get(phrase)
                total_count += cnt
                total_max = max(total_max, cnt)
            total += 1
            if total % 10000 == 0:
                print total
    except KeyboardInterrupt:
        pass
    with open(outf, 'w') as outfile:
        pickle.dump((total_count, total_max, sketch), outfile)
