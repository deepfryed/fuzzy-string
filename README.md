# Fuzzy

A compilation of functions that allow fuzzy string matching.

## API

```
  FuzzyString
    .stem(word, language = "english")         #=> snowball stem of word
    .stem_languages                           #=> list of languages supporter by stemmer
    .soundex(word)                            #=> soundex code of english word
    .jaro_winkler_distance(string1, string2)  #=> numeric Jaro-Winkler distance of two strings (0-1) 1: same 0: different
    .levenstein_distance(string1, string2)    #=> numeric levenstein distance of two strings (edit distance)

```

## Example

```ruby

require 'fuzzy-string'
FuzzyString.jaro_winkler_distance("apples", "apple")
```

## License

[Creative Commons Attribution - CC BY](http://creativecommons.org/licenses/by/3.0)
