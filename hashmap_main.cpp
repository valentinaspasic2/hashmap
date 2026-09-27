#include <iostream>

#include "hashmap.h"

using namespace std;

// simple examples
int main() {
  HashMap<string, int> ages;

  // add some key-value pairs
  ages.insert("Alice", 20);
  ages.insert("Bob", 22);
  ages.insert("Charlie", 19);

  cout << "Alice's age: " << ages.at("Alice") << endl;
  cout << "Contains Bob: " << ages.contains("Bob") << endl;
  cout << "Number of entries: " << ages.size() << endl;

  // remove an entry
  ages.erase("Bob");

  cout << "Contains Bob after erase: " << ages.contains("Bob") << endl;

  return 0;
}
