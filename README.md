# Trie-based Mini Search Engine

A sophisticated C++ document search application that implements intelligent text indexing and retrieval using a Trie data structure. The system processes a collection of text documents and enables fast keyword-based searching with advanced query operators.

---

## 📝 Project Description

**Mini-search-engine** is a C++ application for fast, flexible document search using a Trie data structure. It automatically indexes text files and supports advanced search queries with real-time performance and context-aware results.

### Key Capabilities
- **Document Indexing**: Automatically processes and indexes text files from specified directories
- **Advanced Query Types**: Supports 8+ query operators (AND, OR, exact phrase, price, hashtag, filetype, intitle, minus, plus)
- **Position Tracking**: Maintains exact word positions within documents for context-aware results
- **Stopword Filtering**: Excludes common English words to improve search relevance
- **Real-time Performance**: Displays execution time for each query operation
- **Search History**: Maintains a history of the last 10 searches

### Use Cases
- **Academic Research**: Search through collections of research papers or documents
- **Content Management**: Index and search corporate documents, articles, or reports
- **Data Mining**: Extract specific information patterns from large text collections
- **Educational Tools**: Demonstrate search algorithm concepts and data structure implementation



## 📝 Data Storage Architecture:

```
Root Node
├── 'f' → Node[0]
│   └── 'o' → Node[1]
│       └── 'o' → Node[2]
│           └── 'd' → Node[3] (word: "food")
│               └── Order_file: [DATASET_1, DATASET_15, DATASET_42]
│               └── Order_word: [pos: 23, 156, 289]
│
├── 'c' → Node[4]
│   └── 'h' → Node[5]
│       └── 'e' → Node[6]
│           └── 'f' → Node[7] (word: "chef")
│               └── Order_file: [DATASET_8, DATASET_33, DATASET_67]
│               └── Order_word: [pos: 12, 89, 234]
│
├── 'w' → Node[8]
│   └── 'i' → Node[9]
│       └── 'n' → Node[10]
│           └── 'e' → Node[11] (word: "wine")
│               └── Order_file: [DATASET_12, DATASET_28, DATASET_55]
│               └── Order_word: [pos: 67, 145, 312]
│
└── 'h' → Node[12]
    └── 'o' → Node[13]
        └── 't' → Node[14]
            └── 'e' → Node[15]
                └── 'l' → Node[16] (word: "hotel")
                    └── Order_file: [DATASET_19, DATASET_41, DATASET_73]
                    └── Order_word: [pos: 34, 178, 256]
```

---

## 🚀 Why Trie Was Chosen

1. **Prefix Matching**: Efficient for partial word searches and autocomplete features
2. **Memory Efficiency**: Shared prefixes reduce storage requirements
3. **Fast Retrieval**: O(m) search time where m is word length
4. **Flexible Querying**: Supports complex search patterns and operators

---


### Scope and Limitations
- Designed for moderate-sized document collections (tested with ~100 documents)
- Text-only processing (no binary files, images, or multimedia)
- English language optimization (stopword filtering is English-specific)
- Case-insensitive searching with limited stemming capabilities

---

## ⚙ Tech Stack
- **Programming Language**: C++11/14 (with experimental filesystem support)
- **Compilers**: GCC 7+, MSVC 2017+, Clang 5+

---

# 🔍 Complete Functionality Guide

## 1. Basic AND/OR Query (Default)
**Syntax**: `word1 word2 word3`
- **OR Operation**: Returns documents containing ANY of the keywords
- **AND Operation**: Returns documents containing ALL keywords
- Results are ranked with AND matches first, then OR matches

**Example:**
```
INPUT: food chef wine
OUTPUT: 
- Documents with all three words (AND results)
- Documents with any of the words (OR results)
```

## 2. Exact Phrase Search
**Syntax**: `"exact phrase here"`
- Searches for exact word sequence in specified order

**Example:**
```
INPUT: "Hong Kong"
OUTPUT: Documents containing exactly "Hong Kong"
```

## 3. Price Search
**Syntax**: `$123 keyword` or `keyword $amount`
- Finds monetary values with dollar sign prefix

**Example:**
```
INPUT: wine $75
OUTPUT: Documents containing "wine" and "$75"
```

## 4. Hashtag Search
**Syntax**: `#hashtag #another`
- OR operation for social media style hashtags

**Example:**
```
INPUT: #WhyIStayed #BlackLivesMatter
OUTPUT: Documents containing #WhyIStayed OR #BlackLivesMatter
```

## 5. File Type Search
**Syntax**: `filetype:extension`
- Searches only within files of specified type

**Example:**
```
INPUT: filetype:txt
OUTPUT: All .txt files in the collection
```

## 6. Title Search
**Syntax**: `intitle:keyword`
- OR operation, searches only in document titles (first line)

**Example:**
```
INPUT: intitle:Food
OUTPUT: Documents with "Food" in title
```

## 7. Exclusion Search (Minus)
**Syntax**: `keyword1 -excluded_word keyword2`
- AND operation excluding specified terms

**Example:**
```
INPUT: food -restaurant
OUTPUT: Documents with "food" but NOT containing "restaurant"
```

## 8. Plus Search (Mandatory)
**Syntax**: `word1 +mandatory_word word2`
- Ensures specific words are included even if they're stopwords

**Example:**
```
INPUT: peanut butter +and jam
OUTPUT: Documents containing all words including "and"
```

## 9. Complex Combined Queries
**Syntax**: Mix multiple query types
- Combines different search operations

**Example:**
```
INPUT: "Hong Kong" AND #food
OUTPUT: Documents with exact phrase "Hong Kong" AND #food hashtag

INPUT: intitle:Chef AND $200
OUTPUT: Documents with "Chef" in title AND containing "$200"
```

## 10. Performance and Edge Cases
**Syntax**: Various test cases
- Tests system performance and edge cases

**Example:**
```
INPUT: very long query with many words to test performance
OUTPUT: Performance test results

INPUT: a
OUTPUT: Single character query results
```

---

## 📄 License
This project is for educational and demonstration purposes.

.
