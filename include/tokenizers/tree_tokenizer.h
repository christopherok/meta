/**
 * @file tree_tokenizer.h
 */

#ifndef _TREE_TOKENIZER_H_
#define _TREE_TOKENIZER_H_

#include <string>
#include <memory>
#include <unordered_map>
#include "tokenizers/tokenizer.h"
#include "tokenizers/parse_tree.h"
#include "index/document.h"

namespace meta {
namespace tokenizers {

/**
 * Tokenizes parse trees with various methods.
 */
class TreeTokenizer: public Tokenizer
{
    public:
        /**
         * Enumeration representing different ways to tokenize parse trees.
         */
        enum TreeTokenizerType { Subtree, Depth, Branch, Tag, Skeleton, SemiSkeleton, Multi };

        /**
         * Constructor.
         * @param type - The specific type of tree tokenizer employed
         */
        TreeTokenizer(TreeTokenizerType type);

        /**
         * Tokenizes a file into a Document.
         * @param document - the Document to store the tokenized information in
         * @param docFreq - optional parameter to store IDF values in
         */
        void tokenize(index::Document & document,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq = nullptr);

    private:

        /** the file extension for ParseTrees stored on disk */
        const static char* _extension;

        /** the type of tree tokenizer this tokenizer is using */
        TreeTokenizerType _type;

        /** Tree tokenizer function type */
        typedef std::function<void(index::Document &, const ParseTree &, std::shared_ptr<std::unordered_map<index::TermID, unsigned int>>)> TokenizerFunction;

        /** Hashes specific tree tokenizer types to tokenizer functions */
        std::unordered_map< TreeTokenizerType, TokenizerFunction, std::hash<int> > _tokenizerTypes;

        /**
         * Extracts the depth feature from parse trees: what are the heights of
         * a document's trees?
         * @param document - the document to parse
         * @param tree - the current ParseTree in the document
         * @param docFreq - used to aggregate counts for this TreeTokenizer
         *  instance
         */
        void depthTokenize(index::Document & document, const ParseTree & tree,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq);

        /**
         * Counts occurrences of subtrees in this document's ParseTrees.
         * @param document - the document to parse
         * @param tree - the current ParseTree in the document
         * @param docFreq - used to aggregate counts for this TreeTokenizer
         *  instance
         */
        void subtreeTokenize(index::Document & document, const ParseTree & tree,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq);

        /**
         * Counts occurrences of leaf and interior node labels.
         * @param document - the document to parse
         * @param tree - the current ParseTree in the document
         * @param docFreq - used to aggregate counts for this TreeTokenizer
         *  instance
         */
        void tagTokenize(index::Document & document, const ParseTree & tree,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq);

        /**
         * Keeps track of the branching factor for this document's ParseTrees.
         * @param document - the document to parse
         * @param tree - the current ParseTree in the document
         * @param docFreq - used to aggregate counts for this TreeTokenizer
         *  instance
         */
        void branchTokenize(index::Document & document, const ParseTree & tree,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq);

        /**
         * Ignores node labels and only tokenizes the tree structure.
         * @param document - the document to parse
         * @param tree - the current ParseTree in the document
         * @param docFreq - used to aggregate counts for this TreeTokenizer
         *  instance
         */
        void skeletonTokenize(index::Document & document, const ParseTree & tree,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq);

        /**
         * Keeps track of one node's tag and the skeleton structure beneath it.
         * @param document - the document to parse
         * @param tree - the current ParseTree in the document
         * @param docFreq - used to aggregate counts for this TreeTokenizer
         *  instance
         */
        void semiSkeletonTokenize(index::Document & document, const ParseTree & tree,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq);

        /**
         * Runs multiple tokenizers at once.
         * @param document - the document to parse
         * @param tree - the current ParseTree in the document
         * @param docFreq - used to aggregate counts for this TreeTokenizer
         *  instance
         */
        void multiTokenize(index::Document & document, const ParseTree & tree,
                const std::shared_ptr<std::unordered_map<index::TermID, unsigned int>> & docFreq);
};

}
}

#endif