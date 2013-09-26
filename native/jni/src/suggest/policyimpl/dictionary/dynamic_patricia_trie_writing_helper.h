/*
 * Copyright (C) 2013, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LATINIME_DYNAMIC_PATRICIA_TRIE_WRITING_HELPER_H
#define LATINIME_DYNAMIC_PATRICIA_TRIE_WRITING_HELPER_H

#include <stdint.h>

#include "defines.h"
#include "utils/hash_map_compat.h"

namespace latinime {

class BufferWithExtendableBuffer;
class DynamicBigramListPolicy;
class DynamicPatriciaTrieNodeReader;
class DynamicPatriciaTrieReadingHelper;
class DynamicShortcutListPolicy;
class HeaderPolicy;

class DynamicPatriciaTrieWritingHelper {
 public:
    typedef hash_map_compat<int, int> PtNodeArrayPositionRelocationMap;
    typedef hash_map_compat<int, int> PtNodePositionRelocationMap;
    struct DictPositionRelocationMap {
     public:
        DictPositionRelocationMap()
                : mPtNodeArrayPositionRelocationMap(), mPtNodePositionRelocationMap() {}

        PtNodeArrayPositionRelocationMap mPtNodeArrayPositionRelocationMap;
        PtNodePositionRelocationMap mPtNodePositionRelocationMap;

     private:
        DISALLOW_COPY_AND_ASSIGN(DictPositionRelocationMap);
    };

    DynamicPatriciaTrieWritingHelper(BufferWithExtendableBuffer *const buffer,
            DynamicBigramListPolicy *const bigramPolicy,
            DynamicShortcutListPolicy *const shortcutPolicy)
            : mBuffer(buffer), mBigramPolicy(bigramPolicy), mShortcutPolicy(shortcutPolicy) {}

    ~DynamicPatriciaTrieWritingHelper() {}

    // Add a word to the dictionary. If the word already exists, update the probability.
    bool addUnigramWord(DynamicPatriciaTrieReadingHelper *const readingHelper,
            const int *const wordCodePoints, const int codePointCount, const int probability);

    // Add a bigram relation from word0Pos to word1Pos.
    bool addBigramWords(const int word0Pos, const int word1Pos, const int probability);

    // Remove a bigram relation from word0Pos to word1Pos.
    bool removeBigramWords(const int word0Pos, const int word1Pos);

    void writeToDictFile(const char *const fileName, const HeaderPolicy *const headerPolicy);

    void writeToDictFileWithGC(const int rootPtNodeArrayPos, const char *const fileName,
            const HeaderPolicy *const headerPolicy);

    // CAVEAT: This method must be called only from inner classes of
    // DynamicPatriciaTrieGcEventListeners.
    bool markNodeAsDeleted(const DynamicPatriciaTrieNodeReader *const nodeToUpdate);

    // CAVEAT: This method must be called only from this class or inner classes of
    // DynamicPatriciaTrieGcEventListeners.
    bool writePtNodeToBufferByCopyingPtNodeInfo(BufferWithExtendableBuffer *const bufferToWrite,
            const DynamicPatriciaTrieNodeReader *const originalNode, const int parentPos,
            const int *const codePoints, const int codePointCount, const int probability,
            int *const writingPos);

 private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(DynamicPatriciaTrieWritingHelper);

    static const int CHILDREN_POSITION_FIELD_SIZE;
    static const size_t MAX_DICTIONARY_SIZE;

    BufferWithExtendableBuffer *const mBuffer;
    DynamicBigramListPolicy *const mBigramPolicy;
    DynamicShortcutListPolicy *const mShortcutPolicy;

    bool markNodeAsMovedAndSetPosition(const DynamicPatriciaTrieNodeReader *const nodeToUpdate,
            const int movedPos, const int bigramLinkedNodePos);

    bool writePtNodeWithFullInfoToBuffer(BufferWithExtendableBuffer *const bufferToWrite,
            const bool isBlacklisted, const bool isNotAWord,
            const int parentPos,  const int *const codePoints, const int codePointCount,
            const int probability, const int childrenPos, const int originalBigramListPos,
            const int originalShortcutListPos, int *const writingPos);

    bool writePtNodeToBuffer(BufferWithExtendableBuffer *const bufferToWrite,
            const int parentPos, const int *const codePoints, const int codePointCount,
            const int probability, int *const writingPos);

    bool createAndInsertNodeIntoPtNodeArray(const int parentPos, const int *const nodeCodePoints,
            const int nodeCodePointCount, const int probability, int *const forwardLinkFieldPos);

    bool setPtNodeProbability(const DynamicPatriciaTrieNodeReader *const originalNode,
            const int probability, const int *const codePoints);

    bool createChildrenPtNodeArrayAndAChildPtNode(
            const DynamicPatriciaTrieNodeReader *const parentNode, const int probability,
            const int *const codePoints, const int codePointCount);

    bool createNewPtNodeArrayWithAChildPtNode(const int parentPos, const int *const nodeCodePoints,
            const int nodeCodePointCount, const int probability);

    bool reallocatePtNodeAndAddNewPtNodes(
            const DynamicPatriciaTrieNodeReader *const reallocatingPtNode,
            const int *const reallocatingPtNodeCodePoints, const int overlappingCodePointCount,
            const int probabilityOfNewPtNode, const int *const newNodeCodePoints,
            const int newNodeCodePointCount);

    bool runGC(const int rootPtNodeArrayPos, BufferWithExtendableBuffer *const bufferToWrite);
};
} // namespace latinime
#endif /* LATINIME_DYNAMIC_PATRICIA_TRIE_WRITING_HELPER_H */