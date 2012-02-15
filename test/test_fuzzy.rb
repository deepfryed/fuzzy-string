# encoding: utf-8
require 'helper'

FS = FuzzyString

describe 'fuzzy' do
  describe 'soundex' do
    it 'should raise exception for nil' do
      assert_raises(ArgumentError) { FS.soundex(nil) }
    end

    it 'should default to Z000 for empty string' do
      assert_equal 'Z000', FS.soundex('')
    end

    it 'should generate valid soundex codes' do
      assert_equal "A140", FS.soundex("apple")
      assert_equal "A140", FS.soundex("appel")
      assert_equal "A142", FS.soundex("apples")
      assert_equal "P200", FS.soundex("peach")
    end
  end

  describe 'stem' do
    it 'should stem english words' do
      assert_equal "appl", FS.stem("apples")
      assert_equal "appl", FS.stem("apple")
    end

    it 'should stem spanish words' do
      assert_equal "manz", FS.stem("manza", "es")
    end

    it 'should return a list of languages' do
      assert_kind_of Array, FS.stem_languages
    end
  end

  describe 'jaro winkler distance' do
    it 'should work' do
      assert_equal 1, FS.jaro_winkler_distance("dean", "dean")
      assert_equal 0, FS.jaro_winkler_distance("dean", "mike")
      assert_in_delta 0.8333, FS.jaro_winkler_distance("dean", "sean")
      assert_in_delta 0.6666, FS.jaro_winkler_distance("dean", "teen")
    end
  end

  describe 'levenstein distance' do
    it 'should work' do
      assert_equal 0, FS.levenstein_distance("apple", "apple")
      assert_equal 1, FS.levenstein_distance("apple", "apples")
      assert_equal 5, FS.levenstein_distance("apple", "orange")
    end
  end
end
