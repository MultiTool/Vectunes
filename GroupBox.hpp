#ifndef GroupBox_hpp
#define GroupBox_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "IDrawable.hpp"
#include "ISonglet.hpp"

/**
 *
 * @author MultiTool
*/

// class AudProject;// forward

class GroupBox: public ISonglet {
public:
  ArrayList<IOffsetBox*> SubSongs;
  String SubSongsName = "SubSongs";
  double Duration = 0.0;
  Config* MyProject;
  String MyName;// for debugging
  double MaxAmplitude = 0.0;
  CajaDelimitadora* MyBounds;
  int FreshnessTimeStamp;
  int RefCount = 0;

  // to do: These will belong to the Artist object after we code for that separation
  boolean HighlightSpine = false;
  Color* LineColor;
  boolean UsingSplines = true;
  int NumSubLines = 10;
  ArrayList<Point2D> SplinePoints;
  //Point2D[] SplinePoints = new Splines.PointX[0];
  /* ********************************************************************************* */
  GroupBox() {
    MyBounds = new CajaDelimitadora();
    RefCount = 0;
    //this->LineColor = Globals::ToColorWheel(Math::frand());
  }
  ~GroupBox() {this->Delete_Me();}
  /* ********************************************************************************* */
  IOffsetBox* Add_SubSong(ISonglet& songlet, double TimeOffset, double OctaveOffset, double LoudnessFactor) {
    IOffsetBox *obox = songlet.Spawn_OffsetBox();
    this->Add_SubSong(*obox, TimeOffset, OctaveOffset, LoudnessFactor);
    return obox;
  }
  /* ********************************************************************************* */
  void Add_SubSong(IOffsetBox& obox, double TimeOffset, double OctaveOffset, double LoudnessFactor) {// Add a songlet with its offsetbox already created.
    obox.TimeX = (TimeOffset);
    obox.OctaveY = (OctaveOffset);
    obox.LoudnessFactor = (LoudnessFactor);
    this->Add_SubSong(obox);
  }
  /* ********************************************************************************* */
  void Add_SubSong(IOffsetBox& obox) {// Add a songlet with its offsetbox already created and filled out.
    obox.GetContent()->Set_Project(this->MyProject);// child inherits project from me
    obox.MyParentSong = this;
    SubSongs.push_back(&obox);

    if (UsingSplines) {
      int SplineSize = (this->SubSongs.size()) * NumSubLines + 1;
      this->SplinePoints.resize(SplineSize);
//      this->SplinePoints = new Point2D[SplineSize];
//      for (int pcnt = 0; pcnt < this->SplinePoints.length; pcnt++) {
//        this->SplinePoints[pcnt] = new Point2D();
//      }
      //Cubic_Spline_Boxes(ArrayList<IOffsetBox>& raw, int NumSubLines, ArrayList<Point2D>& SplinePoints)
      Splines::Cubic_Spline_Boxes(this->SubSongs, NumSubLines, SplinePoints);
    }
  }
  /* ********************************************************************************* */
  void Remove_SubSong(IOffsetBox& obox) {
    std::vector<IOffsetBox*>::iterator it;
    it = std::find(this->SubSongs.begin(), this->SubSongs.end(), &obox);
    if (it != this->SubSongs.end()){
      //std::cout << "Element found in SubSongs: " << *it << '\n';
      this->SubSongs.erase(it);
    } else {
      std::cout << "Element not found in SubSongs\n";
    }
  }
  /* ********************************************************************************* */
  void Refresh_Splines() {}
  /* ********************************************************************************* */
  double Get_Duration() override {
    return this->Duration;
  }
  /* ********************************************************************************* */
  double Get_Max_Amplitude() {
    return 0;
  }
  /* ********************************************************************************* */
  void Update_Max_Amplitude() {}
  /* ********************************************************************************* */
  double Update_Durations() {
    return 0;
  }
  /* ********************************************************************************* */
  void Update_Guts(MetricsPacket& metrics) override {}
  /* ********************************************************************************* */
  void Refresh_From_Beneath(IDrawable::IMoveable& mbox) override {}
  /* ********************************************************************************* */
  void Bubble_Right(int Dex) {}
  /* ********************************************************************************* */
  void Sort_Me() {}
  /* ********************************************************************************* */
  int Get_Sample_Count(int SampleRate) override {
    return 0;
  }
  /* ********************************************************************************* */
  Config* Get_Project() override {
    return nullptr;
  }
  /* ********************************************************************************* */
  void Set_Project(Config* project) override {}
  /* ********************************************************************************* */
  void SetSpineHighlight(boolean Highlight) {}
  /* ********************************************************************************* */
  void Draw_Lines(IDrawingContext& ParentDC, int StartDex, int EndDex) {}
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) override {}
  CajaDelimitadora* GetBoundingBox() override {
    return nullptr;
  }
  void UpdateBoundingBox() override {}
  void UpdateBoundingBoxLocal() override {}
  /* ********************************************************************************* */
  void GoFishing(IGrabber& Scoop) override {}
  /* ********************************************************************************* */
  GroupBox* Clone_Me() override {
    return nullptr;
  }
  /* ********************************************************************************* */
  GroupBox* Deep_Clone_Me(CollisionLibrary& HitTable) override {
    return nullptr;
  }
  /* ********************************************************************************* */
  GroupBox* Shallow_Clone_Me() {
    return nullptr;
  }
  /* ********************************************************************************* */
  void Copy_From(GroupBox& donor) {}
  /* ********************************************************************************* */
  void RescaleGroupTimeX(double Factor) {}
  /* ********************************************************************************* */
  static double DotProduct(double X0, double Y0, double X1, double Y1) {
    return 0;
  }
  /* ********************************************************************************* */
  void LineClosestPoint(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt, double YPnt, Point2D& Intersection) {}
  /* ********************************************************************************* */
  static double DistanceFromLine(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt, double YPnt) {
    return 0;
  }
  /* ********************************************************************************* */
  double HeightFromLine(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt, double YPnt) {
    return 0;
  }
  /* ********************************************************************************* */
  double LineYCross(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt) {
    return 0;
  }
  /* ********************************************************************************* */
  double HitsMyVineSpline(double XPnt, double YPnt) {
    return 0;
  }
  /* ********************************************************************************* */
  double HitsMyVine(double XPnt, double YPnt) {
    return 0;
  }
  /* ************************************************************************************************************************ */
  int Tree_Search(double Time, int minloc, int maxloc) {
    return 0;
  }
  /* ********************************************************************************* */
  void Space_Evenly() {}
  /* ********************************************************************************* */
  boolean Create_Me() override {
    return 0;
  }
  void Delete_Me() override {}
  void Wipe_SubSongs() {}
  /* ********************************************************************************* */
  int Ref_Songlet() override {
    return 0;
  }
  int UnRef_Songlet() override {
    return 0;
  }
  int GetRefCount() {
    return 0;
  }
  /* ********************************************************************************* */
  JsonParse::Node* Export(CollisionLibrary& HitTable) override {
    return nullptr;
  }
  void ShallowLoad(JsonParse::Node& phrase) {}
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
  /* ********************************************************************************* */
  class Group_Singer: public ISinger {
  public:
    GroupBox* MySonglet;
    ArrayList<ISinger> NowPlaying;// pool of currently playing voices
    int Current_Dex = 0;
    double Prev_Time = 0;
    //private Group_OffsetBox MyOffsetBox;
    /* ********************************************************************************* */
    void Start() override {}
    /* ********************************************************************************* */
    void Skip_To(double EndTime) override {}
    /* ********************************************************************************* */
    void Render_To(double EndTime, Wave& wave) override {}
    /* ********************************************************************************* */
    void Inherit(ISinger& parent) override {}
    /* ********************************************************************************* */
    void Compound() override {}
    /* ********************************************************************************* */
    void Compound(MonkeyBox& donor) override {}
    /* ********************************************************************************* */
    IOffsetBox* Get_OffsetBox() override {
      return nullptr;
    }
    /* ********************************************************************************* */
    boolean Create_Me() override {
      return 0;
    }
    void Delete_Me() override {}
    /* ********************************************************************************* */
    double Tee_Up(double EndTime) {
      return 0;
    }
  };
  /* ********************************************************************************* */
  class Group_OffsetBox: public IOffsetBox {// location box to transpose in pitch, move in time, etc.
  public:
    GroupBox* Content = null;
    double GroupScaleX = 1.0;
    String GroupScaleXName = "GroupScaleX";// for serialization
    String ObjectTypeName = "Group_OffsetBox";
    /* ********************************************************************************* */
    Group_OffsetBox() {}
    /* ********************************************************************************* */
    GroupBox* GetContent() {
      return nullptr;
    }
    /* ********************************************************************************* */
    void Attach_Songlet(GroupBox& songlet) {}
    /* ********************************************************************************* */
    void RescaleGroupTimeX(double Factor) {}
    /* ********************************************************************************* */
    void Draw_Me(IDrawingContext& ParentDC) override {}
    /* ********************************************************************************* */
    Group_Singer* Spawn_Singer() override {
      return nullptr;
    }
    /* ********************************************************************************* */
    Group_OffsetBox* Clone_Me() override {
      return nullptr;
    }
    /* ********************************************************************************* */
    Group_OffsetBox* Deep_Clone_Me(CollisionLibrary& HitTable) override {
      return nullptr;
    }
    /* ********************************************************************************* */
    void BreakFromHerd(CollisionLibrary& HitTable) {}
    /* ********************************************************************************* */
    void BreakFromHerd_Shallow() {}
    /* ********************************************************************************* */
    boolean Create_Me() {
      return 0;
    }
    void Delete_Me() {}
    /* ********************************************************************************* */
    JsonParse::Node* Export(CollisionLibrary& HitTable) {
      return nullptr;
    }
    void ShallowLoad(JsonParse::Node& phrase) {}
    void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
    ISonglet* Spawn_And_Attach_Songlet() {
      return nullptr;
    }
    /* ********************************************************************************* */
//    class Factory: public  IFactory {// for serialization
//    public:
//      Group_OffsetBox* Create(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {
//        return nullptr;
//      }
//    };
  };
  /* ********************************************************************************* */
  Group_OffsetBox* Spawn_OffsetBox() override {
    Group_OffsetBox *lbox = new Group_OffsetBox();// Deliver an OffsetBox specific to this type of songlet.
    lbox->Attach_Songlet(*this);
    return lbox;
  }
  /* ********************************************************************************* */
  Group_Singer* Spawn_Singer() override {
    Group_Singer *GroupPlayer = new Group_Singer();
    GroupPlayer->MySonglet = this;
    GroupPlayer->MyProject = this->MyProject;// inherit project
    return GroupPlayer;
  }
};


#endif
