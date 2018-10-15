#ifndef JsonParse_hpp
#define JsonParse_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"

//class Node; // forward
class Token; // forward
/* ********************************************************************************************************* */
class JsonParse {
public:
  enum TokenType { None, CommentStar, CommentSlash, Word, Whitespace, SingleChar, TextString };
  const String Environment_NewLine {"\r\n"};
  const String PhraseEnd={","};
  /* ********************************************************************************************************* */
  class Token {
  public:
    String* Text;
    TokenType* BlockType;
    String SpecificType = {"None"};
    String* DeQuoted() {
      return nullptr;
    }
  };
  /* ********************************************************************************************************* */
  class Node {// a value that is a hashtable, an array, a literal, or a pointer to a multiply-used item
  public:
    enum Types { None, Class, Interface, Method, Whatever };// Interface is not used yet.
    Types MyType = Types::None;
    String MyPhraseName = {"***Nothing***"};
    Node* Parent = null;
    int ChunkStart,ChunkEnd;
    String* ItemPtr=null;
    String* Literal=null;
    ArrayList<Node>* ChildrenArray = null;
    HashMap<String,Node>* ChildrenHash = null;
    Node(){}
    void AddSubPhrase(Node& ChildPhrase) {}
    void AddSubPhrase(String& Name, Node& ChildPhrase) {}
    String* ToJson() {
      return nullptr;
    }
    String* ToHash() {
      return nullptr;
    }
    String* ToArray() {
      return nullptr;
    }
  };
  /* ********************************************************************************************************* */
  class PhraseBase {// a value that is a hashtable, an array, a literal, or a pointer to a multiply-used item
  public:
    enum Types { None, Class, Interface, Method, Whatever };// Interface is not used yet.
    Types MyType = Types::None;
    String MyPhraseName = {"***Nothing***"};
    PhraseBase* Parent = null;
    int ChunkStart, ChunkEnd;
    String* ToJson() {
      return nullptr;  // virtual
    }
  };
  /* ********************************************************************************************************* */
  class LiteralPhrase: public PhraseBase {// a value that is a literal
  public:
    String* Literal = null;
    String* Get() {
      return nullptr;
    }
    String* ToJson() {
      return nullptr;
    }
  };
  /* ********************************************************************************************************* */
  class HashPhrase: public PhraseBase {// a value that is a hashtable, an array, a literal, or a pointer to a multiply-used item
  public:
    HashMap<String, PhraseBase> ChildrenHash;
    void AddSubPhrase(String& Name, PhraseBase& ChildPhrase){}
    PhraseBase* Get(String& Name) {
      return nullptr;
    }
    String* ToHash() {
      return nullptr;
    }
    String* ToJson() {
      return nullptr;
    }
  };
  /* ********************************************************************************************************* */
  class ArrayPhrase: public PhraseBase {// a value that is an array
  public:
    ArrayList<PhraseBase> ChildrenArray;
    void AddSubPhrase(PhraseBase& ChildPhrase) {}
    PhraseBase* Get(int Dex) {
      return nullptr;
    }
    String* ToArray() {
      return nullptr;
    }
    String* ToJson() {
      return nullptr;
    }
  };
  /* ********************************************************************************************************* */
  Node* Parse(String& JsonText) {
    return nullptr;
  }
  /* ********************************************************************************************************* */
  class Tokenizer {
    // <editor-fold defaultstate="collapsed" desc="Chunk Finders">
    /* ********************************************************************************************************* */
  public:
    int Chomp_CommentStar(String& txt, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    /* ********************************************************************************************************* */
    int Chomp_CommentSlash(String& txt, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    /* ********************************************************************************************************* */
    int Chomp_Word(String& txt, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    /* ********************************************************************************************************* */
    int Chomp_Whitespace(String& txt, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    /* ********************************************************************************************************* */
    int Chomp_TextString(String& txt,String& QuoteChar, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    /* ********************************************************************************************************* */
    int Chomp_TextStringDoubleQuoted(String& txt, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    /* ********************************************************************************************************* */
    int Chomp_TextStringSingleQuoted(String& txt, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    /* ********************************************************************************************************* */
    int Chomp_SingleChar(String& txt, int StartPlace, ArrayList<Token>& Tokens) { // more compact approach
      return 0;
    }
    // </editor-fold>
    // <editor-fold defaultstate="collapsed" desc="Literal finders, Is Word, Is Number, etc">
    /* ********************************************************************************************************* */
    boolean IsWordChar(char ch) {
      return 0;
    }
    /* ********************************************************************************************************* */
    boolean IsNumericChar(char ch) {
      return 0;
    }
    /* ********************************************************************************************************* */
    boolean IsNumericPunctuationChar(char ch) {
      return 0;
    }
    /* ********************************************************************************************************* */
    boolean IsNumericSuffixChar(char ch) {
      return 0;
    }
    /* ********************************************************************************************************* */
    boolean IsNumericString(String& txt) {
      return 0;
    }
    /* ********************************************************************************************************* */
    boolean IsBlankChar(char ch) {
      return 0;
    }
    // </editor-fold>
    /* ********************************************************************************************************* */
    int CompareStartStay(String& txt, int StartPlace, String& target) {
      return 0;
    }
    /* ********************************************************************************************************* */
    int CompareStart(String& txt, int StartPlace, String& target) {
      return 0;
    }
    /* ********************************************************************************************************* */
    int CompareStartAny(String& txt, int StartPlace, String* targets) {// targets is an array of strings
      return 0;
    }
    /* ********************************************************************************************************* */
    ArrayList<Token>* Tokenize(int StartPlace, String& txt) {
      return nullptr;
    }
    // <editor-fold defaultstate="collapsed" desc="Tree Creation">
    /* ********************************************************************************************************* */
    Node* MakeLiteral(String& Text, int ChunkStart, int ChunkEnd) {
      return nullptr;
    }
    /* ********************************************************************************************************* */
    String* DeQuote(String& Text) {
      return nullptr;
    }
    /* ********************************************************************************************************* */
    String* EnQuote(String& txt) {
      return nullptr;
    }
    /* ********************************************************************************************************* */
    Node* Chomp_NumberX(ArrayList<Token>& Chunks, int Marker, int RecurDepth) {
      return nullptr;
    }
    /* ********************************************************************************************************* */
    Node* Chomp_Number(ArrayList<Token>& Chunks, int Marker, int RecurDepth) {
      return nullptr;
    }
    /* ********************************************************************************************************* */
    Node* Chomp_HashMap(ArrayList<Token>& Chunks, int Marker, int RecurDepth) {
      return nullptr;
    }
    /* ********************************************************************************************************* */
    Node* Chomp_Array(ArrayList<Token>& Chunks, int Marker, int RecurDepth) {
      return nullptr;
    }
    // </editor-fold>
    /* ********************************************************************************************************* */
    Node* Fold(ArrayList<Token>& Chunks) {
      return nullptr;
    }
  };
};


#endif
