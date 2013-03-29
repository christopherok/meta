/**
 * @file inverted_index.cpp
 */

#include "index/inverted_index.h"
#include "index/structs.h"

namespace meta {
namespace index {

using std::vector;
using std::cerr;
using std::endl;
using std::multimap;
using std::unordered_map;
using std::string;

using tokenizers::Tokenizer;

InvertedIndex::InvertedIndex(const string & lexiconFile, const string & postingsFile,
        std::shared_ptr<Tokenizer> tokenizer):
    _lexicon(lexiconFile),
    _postings(postingsFile),
    _tokenizer(tokenizer)
{
    if(!_lexicon.isEmpty())
        _tokenizer->setTermIDMapping(_lexicon.getTermIDMapping());
}

multimap<double, string> InvertedIndex::search(Document & query) const
{
    cerr << "[InvertedIndex]: scoring documents for query " << query.getName()
         << " (" << query.getCategory() << ")" << endl;

    double k1 = 1.5;
    double b = 0.75;
    double k3 = 500;
    double numDocs = _lexicon.getNumDocs();
    double avgDL = _lexicon.getAvgDocLength();
    unordered_map<DocID, double> scores;

    _tokenizer->tokenize(query);
    const unordered_map<TermID, unsigned int> freqs = query.getFrequencies();

    for(auto & queryTerm: freqs)
    {
        TermID queryTermID = queryTerm.first;
        if(!_lexicon.containsTermID(queryTermID))
            continue;

        size_t queryTermFreq = queryTerm.second;
        TermData termData = _lexicon.getTermInfo(queryTermID);
        vector<PostingData> docList = _postings.getDocs(termData);

        for(auto & doc: docList)
        {
            double docLength = _lexicon.getDocLength(doc.docID);
            double IDF = log((numDocs - termData.idf + 0.5) / (termData.idf + 0.5));
            double TF = ((k1 + 1.0) * doc.freq) / ((k1 * ((1.0 - b) + b * docLength / avgDL)) + doc.freq);
            double QTF = ((k3 + 1.0) * queryTermFreq) / (k3 + queryTermFreq);
            double score = TF * IDF * QTF;

            scores[doc.docID] += score;
        }
    }

    // combine into sorted multimap
    multimap<double, string> results;
    for(auto & score: scores)
        results.insert(make_pair(score.second, Document::getCategory(_lexicon.getDoc(score.first))));
    return results;
}

void InvertedIndex::indexDocs(vector<Document> & documents, size_t chunkMBSize)
{
    if(!_lexicon.isEmpty())
        throw IndexException("[InvertedIndex]: attempted to create an index in an existing index location");

    size_t numChunks = _postings.createChunks(documents, chunkMBSize, _tokenizer);
    _tokenizer->saveTermIDMapping("termid.mapping");
    _postings.saveDocIDMapping("docid.mapping");
    _postings.createPostingsFile(numChunks, _lexicon);
    _postings.saveDocLengths(documents, "docs.lengths");
    _lexicon.save("docs.lengths", "termid.mapping", "docid.mapping");
}

}
}