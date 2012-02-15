# encoding: utf-8
require 'helper'

describe 'fuzzy' do
  describe 'soundex' do
    it 'should raise exception for nil' do
      assert_raises(ArgumentError) { Fuzzy.soundex(nil) }
    end

    it 'should default to Z000 for empty string' do
      assert_equal 'Z000', Fuzzy.soundex('')
    end

    it 'should generate valid soundex codes' do
      assert_equal "A140", Fuzzy.soundex("apple")
      assert_equal "A140", Fuzzy.soundex("appel")
      assert_equal "A142", Fuzzy.soundex("apples")
      assert_equal "P200", Fuzzy.soundex("peach")
    end
  end

  describe 'stem' do
    it 'should stem english words' do
      assert_equal "appl", Fuzzy.stem("apples")
      assert_equal "appl", Fuzzy.stem("apple")
    end

    it 'should stem spanish words' do
      assert_equal "manz", Fuzzy.stem("manza", "es")
    end

    it 'should return a list of languages' do
      assert_kind_of Array, Fuzzy.stem_languages
    end
  end

  describe 'jaro winkler distance' do
    it 'should work' do
      assert_equal 1, Fuzzy.jaro_winkler_distance("dean", "dean")
      assert_equal 0, Fuzzy.jaro_winkler_distance("dean", "mike")
      assert_in_delta 0.8333, Fuzzy.jaro_winkler_distance("dean", "sean")
      assert_in_delta 0.6666, Fuzzy.jaro_winkler_distance("dean", "teen")
    end
  end
end
