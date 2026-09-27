
// Provided by course instructor.

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "hashmap.h"

using namespace std;
using namespace testing;

namespace {

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) {
    Random::rng.seed(s);
  }

  /**
   * Generate a random integer in the range 0 (inclusive) to `max` (exclusive)
   */
  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

std::mt19937 Random::rng;

  // If no tests exist, GoogleTest fails to compile with a fairly confusing
  // message. We have this empty test that does nothing to allow an empty
  // autograder submission to compile and regularly fail. Once you write your
  // own tests for the project, you can delete this one.

TEST(HashMapCore, DefaultConstructorStartsEmpty) {
  // make default map
  HashMap<int, int> hm;

  // should start empty
  EXPECT_THAT(hm.size(), Eq(0));
  EXPECT_THAT(hm.empty(), Eq(true));

  // default capacity should be 10
  EXPECT_THAT(hm.get_capacity(), Eq(10));
}

TEST(HashMapCore, CapacityConstructorStartsEmpty) {
  // make map with custom capacity
  HashMap<int, int> hm(7);

  // should start empty
  EXPECT_THAT(hm.size(), Eq(0));
  EXPECT_THAT(hm.empty(), Eq(true));

  // capacity should match
  EXPECT_THAT(hm.get_capacity(), Eq(7));
}

TEST(HashMapCore, EmptyChangesAfterInsert) {
  // make map
  HashMap<int, int> hm;

  // starts empty
  EXPECT_THAT(hm.empty(), Eq(true));

  // add one item
  hm.insert(1, 100);

  // no longer empty
  EXPECT_THAT(hm.empty(), Eq(false));
}

TEST(HashMapCore, SizeChangesWithInsertAndErase) {
  // make map
  HashMap<int, int> hm;

  // add items
  hm.insert(1, 10);
  hm.insert(2, 20);
  hm.insert(3, 30);

  // size should grow
  EXPECT_THAT(hm.size(), Eq(3));

  // remove one
  hm.erase(2);

  // size should shrink
  EXPECT_THAT(hm.size(), Eq(2));
}

TEST(HashMapCore, InsertActuallyAddsData) {
  // make map
  HashMap<int, int> hm;

  // insert item
  hm.insert(5, 50);

  // key should exist
  EXPECT_THAT(hm.contains(5), Eq(true));

  // value should match
  EXPECT_THAT(hm.at(5), Eq(50));
}

TEST(HashMapCore, InsertDoesNotOverwriteExistingKey) {
  // make map
  HashMap<int, int> hm;

  // insert same key twice
  hm.insert(8, 80);
  hm.insert(8, 999);

  // size should stay 1
  EXPECT_THAT(hm.size(), Eq(1));

  // original value should stay
  EXPECT_THAT(hm.at(8), Eq(80));
}

TEST(HashMapCore, InsertKeepsBothValuesOnCollision) {
  // make small map to force collision
  HashMap<int, int> hm(5);

  // 1 and 6 collide when capacity is 5
  hm.insert(1, 100);
  hm.insert(6, 600);

  // both should still exist
  EXPECT_THAT(hm.contains(1), Eq(true));
  EXPECT_THAT(hm.contains(6), Eq(true));
  EXPECT_THAT(hm.at(1), Eq(100));
  EXPECT_THAT(hm.at(6), Eq(600));
  EXPECT_THAT(hm.size(), Eq(2));
}

TEST(HashMapCore, ContainsFalseWhenMissingAndTrueWhenPresent) {
  // make map
  HashMap<int, int> hm(5);

  // add one item
  hm.insert(2, 22);

  // check present and missing
  EXPECT_THAT(hm.contains(2), Eq(true));
  EXPECT_THAT(hm.contains(7), Eq(false));
}

TEST(HashMapCore, ContainsWorksWithCollision) {
  // make small map
  HashMap<int, int> hm(5);

  // collide keys
  hm.insert(3, 30);
  hm.insert(8, 80);

  // should find both
  EXPECT_THAT(hm.contains(3), Eq(true));
  EXPECT_THAT(hm.contains(8), Eq(true));
}

TEST(HashMapCore, AtReturnsCorrectValueWithCollision) {
  // make small map
  HashMap<int, int> hm(5);

  // collide keys
  hm.insert(4, 40);
  hm.insert(9, 90);

  // each key should get its own value
  EXPECT_THAT(hm.at(4), Eq(40));
  EXPECT_THAT(hm.at(9), Eq(90));
}

TEST(HashMapCore, AtThrowsWhenKeyMissing) {
  // make map
  HashMap<int, int> hm;

  // missing key should throw
  EXPECT_THROW(hm.at(99), out_of_range);
}

TEST(HashMapCore, ClearResetsSizeAndEmptiesMap) {
  // make map
  HashMap<int, int> hm(5);

  // add items
  hm.insert(1, 10);
  hm.insert(6, 60);
  hm.insert(11, 110);

  // clear map
  hm.clear();

  // should be empty now
  EXPECT_THAT(hm.size(), Eq(0));
  EXPECT_THAT(hm.empty(), Eq(true));
  EXPECT_THAT(hm.contains(1), Eq(false));
  EXPECT_THAT(hm.contains(6), Eq(false));
  EXPECT_THAT(hm.contains(11), Eq(false));
}

TEST(HashMapCore, ClearFreesNodesSoMapCanBeReused) {
  // make map
  HashMap<int, int> hm(5);

  // add items
  hm.insert(1, 10);
  hm.insert(6, 60);

  // clear
  hm.clear();

  // reuse map
  hm.insert(2, 20);

  // new data should work
  EXPECT_THAT(hm.size(), Eq(1));
  EXPECT_THAT(hm.contains(2), Eq(true));
  EXPECT_THAT(hm.at(2), Eq(20));
}

TEST(HashMapCore, DestructorRunsAfterHavingData) {
  // make heap map
  HashMap<int, int>* hm = new HashMap<int, int>(5);

  // add items
  hm->insert(1, 10);
  hm->insert(6, 60);
  hm->insert(11, 110);

  // delete map
  delete hm;

  // reaching here is good
  SUCCEED();
}

TEST(HashMapCore, EraseReturnsValueAndRemovesKey) {
  // make map
  HashMap<int, int> hm;

  // add item
  hm.insert(7, 70);

  // erase it
  int value = hm.erase(7);

  // returned value should match
  EXPECT_THAT(value, Eq(70));

  // key should be gone
  EXPECT_THAT(hm.contains(7), Eq(false));
  EXPECT_THAT(hm.size(), Eq(0));
}

TEST(HashMapCore, EraseThrowsWhenKeyMissing) {
  // make map
  HashMap<int, int> hm;

  // erase missing key
  EXPECT_THROW(hm.erase(123), out_of_range);
}

TEST(HashMapCore, EraseDoesNotRemoveOtherCollisionValues) {
  // make small map
  HashMap<int, int> hm(5);

  // collide keys
  hm.insert(1, 10);
  hm.insert(6, 60);
  hm.insert(11, 110);

  // erase middle collision key
  int value = hm.erase(6);

  // removed value should match
  EXPECT_THAT(value, Eq(60));

  // other keys should still exist
  EXPECT_THAT(hm.contains(1), Eq(true));
  EXPECT_THAT(hm.contains(11), Eq(true));
  EXPECT_THAT(hm.at(1), Eq(10));
  EXPECT_THAT(hm.at(11), Eq(110));
  EXPECT_THAT(hm.size(), Eq(2));
}

TEST(HashMapCore, CopyConstructorCopiesEmptyMap) {
  // make empty map
  HashMap<int, int> hm;

  // copy it
  HashMap<int, int> copy(hm);

  // copy should also be empty
  EXPECT_THAT(copy.size(), Eq(0));
  EXPECT_THAT(copy.empty(), Eq(true));
}

TEST(HashMapCore, CopyConstructorCopiesAllData) {
  // make original map
  HashMap<int, int> hm(5);

  // add items
  hm.insert(1, 10);
  hm.insert(6, 60);
  hm.insert(2, 20);

  // copy map
  HashMap<int, int> copy(hm);

  // copy should have same data
  EXPECT_THAT(copy.size(), Eq(3));
  EXPECT_THAT(copy.contains(1), Eq(true));
  EXPECT_THAT(copy.contains(6), Eq(true));
  EXPECT_THAT(copy.contains(2), Eq(true));
  EXPECT_THAT(copy.at(1), Eq(10));
  EXPECT_THAT(copy.at(6), Eq(60));
  EXPECT_THAT(copy.at(2), Eq(20));
}

TEST(HashMapCore, CopyConstructorMakesDeepCopy) {
  // make original map
  HashMap<int, int> hm(5);

  // add data
  hm.insert(1, 10);
  hm.insert(6, 60);

  // copy map
  HashMap<int, int> copy(hm);

  // change original
  hm.erase(1);

  // copy should stay unchanged
  EXPECT_THAT(copy.contains(1), Eq(true));
  EXPECT_THAT(copy.at(1), Eq(10));
  EXPECT_THAT(copy.size(), Eq(2));
}

TEST(HashMapCore, AssignmentCopiesEmptyMap) {
  // make source empty
  HashMap<int, int> source;

  // make target with data
  HashMap<int, int> target;
  target.insert(1, 10);
  target.insert(2, 20);

  // assign
  target = source;

  // target should become empty
  EXPECT_THAT(target.size(), Eq(0));
  EXPECT_THAT(target.empty(), Eq(true));
}

TEST(HashMapCore, AssignmentCopiesAllDataAndSize) {
  // make source
  HashMap<int, int> source(5);

  // add source data
  source.insert(1, 10);
  source.insert(6, 60);
  source.insert(3, 30);

  // make target
  HashMap<int, int> target;
  target.insert(100, 1000);

  // assign
  target = source;

  // target should match source
  EXPECT_THAT(target.size(), Eq(3));
  EXPECT_THAT(target.contains(1), Eq(true));
  EXPECT_THAT(target.contains(6), Eq(true));
  EXPECT_THAT(target.contains(3), Eq(true));
  EXPECT_THAT(target.at(1), Eq(10));
  EXPECT_THAT(target.at(6), Eq(60));
  EXPECT_THAT(target.at(3), Eq(30));
}

TEST(HashMapCore, AssignmentProtectsSelfAssignment) {
  // make map
  HashMap<int, int> hm(5);

  // add data
  hm.insert(1, 10);
  hm.insert(6, 60);

  // create reference to same object
  HashMap<int, int>& ref = hm;

  // self assign through reference (avoids compiler error)
  hm = ref;

  // data should stay valid
  EXPECT_THAT(hm.size(), Eq(2));
  EXPECT_THAT(hm.contains(1), Eq(true));
  EXPECT_THAT(hm.contains(6), Eq(true));
  EXPECT_THAT(hm.at(1), Eq(10));
  EXPECT_THAT(hm.at(6), Eq(60));
}

TEST(HashMapCore, AssignmentMakesDeepCopy) {
  // make source
  HashMap<int, int> source(5);

  // add source data
  source.insert(1, 10);
  source.insert(6, 60);

  // make target
  HashMap<int, int> target;

  // assign
  target = source;

  // change source
  source.erase(1);

  // target should stay unchanged
  EXPECT_THAT(target.contains(1), Eq(true));
  EXPECT_THAT(target.at(1), Eq(10));
  EXPECT_THAT(target.size(), Eq(2));
}

TEST(HashMapAugmented, EqualityFalseWhenSizesDiffer) {
  // make two maps
  HashMap<int, int> a(5);
  HashMap<int, int> b(5);

  // add different number of items
  a.insert(1, 10);
  a.insert(2, 20);
  b.insert(1, 10);

  // sizes differ so not equal
  EXPECT_THAT(a == b, Eq(false));
}

TEST(HashMapAugmented, EqualityChecksValuesNotJustKeys) {
  // make two maps
  HashMap<int, int> a(5);
  HashMap<int, int> b(5);

  // same keys
  a.insert(1, 10);
  a.insert(6, 60);

  b.insert(1, 999);
  b.insert(6, 60);

  // one value differs
  EXPECT_THAT(a == b, Eq(false));
}

TEST(HashMapAugmented, EqualityTrueWithSameMappingsDifferentOrder) {
  // make two maps
  HashMap<int, int> a(5);
  HashMap<int, int> b(10);

  // insert same mappings in different order
  a.insert(1, 10);
  a.insert(6, 60);
  a.insert(2, 20);

  b.insert(2, 20);
  b.insert(1, 10);
  b.insert(6, 60);

  // should still be equal
  EXPECT_THAT(a == b, Eq(true));
}

TEST(HashMapAugmented, BeginNextWorksOnEmptyMap) {
  // make empty map
  HashMap<int, int> hm;

  // start iteration
  hm.begin();

  int key;
  int value;

  // should return false right away
  EXPECT_THAT(hm.next(key, value), Eq(false));
}

TEST(HashMapAugmented, BeginNextVisitsAllCollisionData) {
  // make small map
  HashMap<int, int> hm(5);

  // add colliding keys
  hm.insert(1, 10);
  hm.insert(6, 60);
  hm.insert(11, 110);

  // start iteration
  hm.begin();

  vector<pair<int, int>> seen;

  int key;
  int value;

  // collect all pairs
  while (hm.next(key, value)) {
    seen.push_back({key, value});
  }

  // should get every mapping
  vector<pair<int, int>> expected = {
      {1, 10},
      {6, 60},
      {11, 110},
  };

  EXPECT_THAT(seen, UnorderedElementsAreArray(expected));
}

TEST(HashMapAugmented, NextReturnsFalseWhenDone) {
  // make map
  HashMap<int, int> hm(5);

  // add one item
  hm.insert(3, 30);

  // start iteration
  hm.begin();

  int key;
  int value;

  // first call gets item
  EXPECT_THAT(hm.next(key, value), Eq(true));

  // second call should be done
  EXPECT_THAT(hm.next(key, value), Eq(false));

  // should keep returning false
  EXPECT_THAT(hm.next(key, value), Eq(false));
}

TEST(HashMapAugmented, EqualityFalseWhenLeftSideIsSmaller) {
  // make two maps
  HashMap<int, int> small(5);
  HashMap<int, int> big(5);

  // small is subset of big
  small.insert(1, 10);

  big.insert(1, 10);
  big.insert(2, 20);

  // should not be equal
  EXPECT_THAT(small == big, Eq(false));
}

}  // namespace
