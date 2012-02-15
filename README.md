# Fuzzy

A compilation of functions that allow fuzzy string matches.


## API

```
  Fuzzy
    .stem(word, language = "english")         #=> snowball stem of word
    .stem_languages                           #=> list of languages supporter by stemmer
    .soundex(word)                            #=> soundex code of english word
    .jaro_winkler_distance(string1, string2)  #=> numeric Jaro-Winkler distance of two strings
```

## Example

```ruby

require 'fuzzy'
Fuzzy.jaro_winkler_distance(Fuzzy.stem("apples"), Fuzzy.stem("apple")) #=> 1
```

## License

[Creative Commons Attribution - CC BY](http://creativecommons.org/licenses/by/3.0)
