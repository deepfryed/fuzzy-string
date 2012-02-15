/*
    (c) Bharanee Rathna 2011

    CC BY-SA 3.0
    http://creativecommons.org/licenses/by-sa/3.0/

    Free for every type of use. The author cannot be legally held responsible for
    any damages resulting from the use of this work. All modifications or derivatives
    need to be attributed.
*/

#include <ctype.h>
#include <libstemmer.h>
#include <ruby/ruby.h>
#include <ruby/encoding.h>
#include "version.h"

#define max(a, b)     (a > b ? a : b)
#define min(a, b)     (a < b ? a : b)
#define min3(a, b, c) (a < b ? (a < c ? a : c) : (b < c ? b : c))

#define TO_S(v)    rb_funcall(v, rb_intern("to_s"), 0)
#define CSTRING(v) RSTRING_PTR(TO_S(v))

VALUE fuzzy_default_language, mFuzzy;

// shamelessly stolen from https://github.com/kiyoka/fuzzy-string-match.git
double c_jaro_winkler_distance(char *s1, char *s2) {
    char *_max;
    char *_min;
    int _max_length = 0;
    int _min_length = 0;
    if (strlen(s1) > strlen(s2)) {
        _max = s1;
        _max_length = strlen(s1);
        _min = s2;
        _min_length = strlen(s2);
    }
    else {
        _max = s2;
        _max_length = strlen(s2);
        _min = s1;
        _min_length = strlen(s1);
    }
    int range = max(_max_length / 2 - 1, 0);

    int indexes[_min_length];
    for (int i = 0; i < _min_length; i++) {
        indexes[i] = -1;
    }

    int flags[_max_length];
    for (int i = 0; i < _max_length; i++) {
        flags[i] = 0;
    }
    int matches = 0;
    for (int mi = 0; mi < _min_length; mi++) {
        char c1 = _min[mi];
        for (int xi = max(mi - range, 0), xn = min(mi + range + 1, _max_length); xi < xn; xi++) {
            if (!flags[xi] && (c1 == _max[xi])) {
                indexes[mi] = xi;
                flags[xi] = 1;
                matches++;
                break;
            }
        }
    }

    char ms1[matches];
    char ms2[matches];
    int ms1_length = matches;

    for (int i = 0, si = 0; i < _min_length; i++) {
        if (indexes[i] != -1) {
            ms1[si] = _min[i];
            si++;
        }
    }
    for (int i = 0, si = 0; i < _max_length; i++) {
        if (flags[i]) {
            ms2[si] = _max[i];
            si++;
        }
    }
    int transpositions = 0;
    for (int mi = 0; mi < ms1_length; mi++) {
        if (ms1[mi] != ms2[mi]) {
            transpositions++;
        }
    }
    int prefix = 0;
    for (int mi = 0; mi < _min_length; mi++) {
        if (s1[mi] == s2[mi]) {
            prefix++;
        }
        else {
            break;
        }
    }

    double m = (double) matches;
    if (matches == 0) {
        return 0.0;
    }
    int t = transpositions / 2;
    double j = ((m / strlen(s1) + m / strlen(s2) + (m - t) / m)) / 3;
    double jw = j < 0.7 ? j : j + min(0.1, 1.0 / _max_length) * prefix * (1 - j);
    return jw;
}

int c_levenstein_distance(char *s, char *t) {
    int k, i, j, n, m, cost, *d, distance;
    n = strlen(s);
    m = strlen(t);
    if (n != 0 && m != 0) {
        d = (int*)malloc((sizeof(int)) * (m + 1) * (n + 1));
        m++;
        n++;
        //Step 2
        for (k = 0; k < n; k++)
            d[k] = k;
        for (k = 0; k < m; k++)
            d[k * n] = k;
        //Step 3 and 4
        for (i = 1; i < n; i++)
            for (j = 1; j < m; j++) {
                //Step 5
                if (s[i - 1] == t[j - 1])
                    cost = 0;
                else
                    cost = 1;
                //Step 6
                d[j * n + i] = min3(d[(j - 1) * n + i] + 1, d[j * n + i - 1] + 1, d[(j - 1) * n + i - 1] + cost);
            }
        distance = d[n * m - 1];
        free(d);
        return distance;
    }
    //a negative return value means that one or both strings are empty.
    else
        return -1;
}

VALUE fuzzy_jaro_winkler_distance(VALUE self, VALUE s1, VALUE s2) {
    return DBL2NUM(c_jaro_winkler_distance(CSTRING(s1), CSTRING(s2)));
}

VALUE fuzzy_levenstein_distance(VALUE self, VALUE s1, VALUE s2) {
    return INT2NUM(c_levenstein_distance(CSTRING(s1), CSTRING(s2)));
}

VALUE fuzzy_snowball(int argc, VALUE * argv, VALUE self) {
    VALUE word, language, result = Qnil;

    rb_scan_args(argc, argv, "11", &word, &language);
    if (NIL_P(language))
        language = fuzzy_default_language;

    if (TYPE(word) != T_STRING)
        rb_raise(rb_eArgError, "invalid word, expect string");

    struct sb_stemmer *stemmer = sb_stemmer_new(CSTRING(language), "UTF_8");
    if (stemmer) {
        const sb_symbol *stem = sb_stemmer_stem(stemmer, RSTRING_PTR(word), RSTRING_LEN(word));
        uint32_t stem_len = sb_stemmer_length(stemmer);
        result = rb_enc_str_new(stem, stem_len, rb_enc_get(word));
        sb_stemmer_delete(stemmer);
    }

    return result;
}

VALUE fuzzy_snowball_languages(VALUE self) {
    VALUE languages = rb_ary_new();
    const char **list = sb_stemmer_list();
    while (*list) {
        // ignore 'porter' - it's only for backwards compatibility.
        if (strcmp(*list, "porter"))
            rb_ary_push(languages, rb_str_new2(*list));
        list++;
    }

    return languages;
}

// adapted from http://en.literateprograms.org/Soundex_(C)
VALUE fuzzy_soundex(VALUE self, VALUE string) {
    if (TYPE(string) != T_STRING)
        rb_raise(rb_eArgError, "invalid argument, expect string");

    static int code[] = { 0, 1, 2, 3, 0, 1, 2, 0, 0, 2, 2, 4, 5, 5, 0, 1, 2, 6, 2, 3, 0, 1, 0, 2, 0, 2 };
                       /* a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z */
    static char key[5];
    register char ch;
    register int last;
    register int count;
    const char *cstring = RSTRING_PTR(string);

    /* Set up default key, complete with trailing '0's */
    strcpy(key, "Z000");

    /* Advance to the first letter.  If none present,
       return default key */
    while (*cstring && !isalpha(*cstring))
        ++cstring;
    if (*cstring == 0)
        return rb_str_new2(key);

    /* Pull out the first letter, uppercase it, and
       set up for main loop */
    key[0] = toupper(*cstring);
    last = code[key[0] - 'A'];
    ++cstring;

    /* Scan rest of string, stop at end of string or
       when the key is full */
    for (count = 1; count < 4 && *cstring; ++cstring) {
        /* If non-alpha, ignore the character altogether */
        if (isalpha(*cstring)) {
            ch = tolower(*cstring);
            /* Fold together adjacent letters sharing the same code */
            if (last != code[ch - 'a']) {
                last = code[ch - 'a'];
                /* Ignore code==0 letters except as separators */
                if (last != 0)
                    key[count++] = '0' + last;
            }
        }
    }

    return rb_str_new2(key);
}

void Init_fuzzy_string() {
    mFuzzy = rb_define_module("FuzzyString");

    fuzzy_default_language = rb_str_new2("en");
    rb_global_variable(&fuzzy_default_language);

    rb_define_module_function(mFuzzy, "jaro_winkler_distance", RUBY_METHOD_FUNC(fuzzy_jaro_winkler_distance), 2);
    rb_define_module_function(mFuzzy, "levenstein_distance", RUBY_METHOD_FUNC(fuzzy_levenstein_distance), 2);
    rb_define_module_function(mFuzzy, "stem", RUBY_METHOD_FUNC(fuzzy_snowball), -1);
    rb_define_module_function(mFuzzy, "stem_languages", RUBY_METHOD_FUNC(fuzzy_snowball_languages), 0);
    rb_define_module_function(mFuzzy, "soundex", RUBY_METHOD_FUNC(fuzzy_soundex), 1);

    rb_define_const(mFuzzy, "VERSION", rb_str_new2(RUBY_FUZZY_VERSION));
}
