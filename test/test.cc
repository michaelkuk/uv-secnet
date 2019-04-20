/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <string>
#include <iostream>

using namespace std;

class Parent {
  public:
    Parent(string s) {
      data = s;
    };

  virtual string& getData() {
    return data;
  }
  virtual void setData(string s) {
    data = s;
  }

  virtual void callOverride(string s) {
    setData(s);
  }

    string data;
};

class Child : public Parent {
  public:
    Child(string s, string d): Parent("") {
      this->data = s;
      data1 = d;
    };

    void setData(string s) {
      cout << "Overriden method" << endl;
      data = s;
    }

    void setData(string s, string d) {
      Parent::setData(s);
      data1 = d;
    }

    string data;
    string data1;
};

int main() {
  Child c("hello", "dolly");

  Parent p = static_cast<Parent>(c);

  p.setData("hello again");

  cout << "Child has data property of: " << p.data << endl;
  return 0;
}