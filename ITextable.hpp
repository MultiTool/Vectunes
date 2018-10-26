#ifndef ITextable_hpp
#define ITextable_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "JsonParse.hpp"

/**
 *
 * @author MultiTool
*/
class CollisionLibrary;// forward
class ITextable {// DIY Json ISerializable - more control
public:
  virtual JsonParse::Node* Export(CollisionLibrary& HitTable) { return nullptr; };// pass a collision table parameter
  virtual void ShallowLoad(JsonParse::Node& phrase){};// just fill in primitive fields that belong to this object, don't follow up pointers.
  virtual void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances){};// Fill in all the values of an already-created object, including deep pointers.
//  IFactory GetMyFactory();// this always returns a singleton of IFactory, one for each class declaration.
//
};

/* ********************************************************************************* */
class CollisionItem {// do we really need this?
public:
  String *ItemTxtPtr;// Key, usually
  ITextable *Item = null;//, ClonedItem = null;
  JsonParse::Node *JsonPhrase = null;// serialization of the ITextable Item
};

class CollisionLibrary {// contains twice-indexed list of instances of (usually) songlet/phrase pairs for serialization, DEserialization, and cloning
public:
  int ItemIdNum = 0;
  HashMap<ITextable*, CollisionItem*> Instances;// serialization and cloning
  HashMap<String, CollisionItem> Items;// DEserialization
  CollisionItem *InsertUniqueInstance(ITextable *KeyObj) {// for serialization
    CollisionItem *ci = new CollisionItem();
//    ci.ItemTxtPtr = Globals.PtrPrefix + ItemIdNum;
//    ci.Item = KeyObj;
//    this.Instances.put(KeyObj, ci);// object is key
//    this.Items.put(ci.ItemTxtPtr, ci);// string is key
//    ItemIdNum++;
    return ci;
  }
  void InsertTextifiedItem(String& KeyTxt, JsonParse::Node& Item) {// for deserialization, only on load
//    CollisionItem *ci = new CollisionItem();
//    ci.Item = null;
//    ci.ItemTxtPtr = KeyTxt;
//    ci.JsonPhrase = Item;
//    this.Items.put(KeyTxt, ci);// string is key
  }
  CollisionItem* GetItem(ITextable* KeyObj) {// for serialization
    return nullptr;
    //return this.Instances.get(KeyObj);
  }
  CollisionItem* GetItem(String& KeyTxt) {
    return nullptr;
    //return this.Items.get(KeyTxt);
  }
  JsonParse::Node* ExportJson() {
    JsonParse::Node *MainPhrase = new JsonParse::Node();
//    MainPhrase.ChildrenHash = new HashMap<String, JsonParse.Node>();
//    JsonParse.Node ChildPhrase;
//    CollisionItem ci;
//    for (Map.Entry<String, CollisionItem> entry : this.Items.entrySet()) {
//      ci = entry.getValue();
//      if (ci.JsonPhrase != null) {
//        //ChildPhrase = new JsonParse.Node();// should we clone the child phrase?
//        MainPhrase.AddSubPhrase(ci.ItemTxtPtr, ci.JsonPhrase);
//      }
//    }
    return MainPhrase;
  }
  void ConsumePhrase(JsonParse::Node& LibraryPhrase) {
//    this.Items.clear();
//    this.Instances.clear();
//    for (Map.Entry<String, JsonParse.Node> entry : LibraryPhrase.ChildrenHash.entrySet()) {
//      this.InsertTextifiedItem(entry.getKey(), entry.getValue());
//    }
  }
  void Wipe_Songlets() {// for testing
    CollisionItem *ci;
//    IDeletable deletable;
//    for (Map.Entry<String, CollisionItem> entry : this.Items.entrySet()) {
//      ci = entry.getValue();
//      if ((deletable = (IDeletable) ci.Item) != null) {
//        //deletable.Delete_Me();
//        ci.Item = null;
//      }
//    }
  }
};

#endif
