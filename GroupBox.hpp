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
    this->Add_SubSong(obox, TimeOffset, OctaveOffset, LoudnessFactor);
    return obox;
  }
  /* ********************************************************************************* */
  void Add_SubSong(IOffsetBox* obox, double TimeOffset, double OctaveOffset, double LoudnessFactor) {// Add a songlet with its offsetbox already created.
    obox->TimeX = (TimeOffset);
    obox->OctaveY = (OctaveOffset);
    obox->LoudnessFactor = (LoudnessFactor);
    this->Add_SubSong(obox);
  }
  /* ********************************************************************************* */
  void Add_SubSong(IOffsetBox* obox) {// Add a songlet with its offsetbox already created and filled out.
    obox->GetContent()->Set_Project(this->MyProject);// child inherits project from me
    obox->MyParentSong = this;
    int dex = this->Tree_Search(obox->TimeX, 0, SubSongs.size());
    SubSongs.insert(SubSongs.begin(), dex, obox);
    //this->Sort_Me();// overkill in the case we add a bunch of subsongs in a loop. should only sort once at end of loop.
    Refresh_Splines();// maybe sort_me and refresh_splines should be in update_guts instead?
  }
  /* ********************************************************************************* */
  void Remove_SubSong(IOffsetBox& obox) {
    std::vector<IOffsetBox*>::iterator iter;
    iter = std::find(this->SubSongs.begin(), this->SubSongs.end(), &obox);
    if (iter != this->SubSongs.end()){
      std::cout << "Element found in SubSongs: " << *iter << '\n';
      this->SubSongs.erase(iter);
      if (false){// to do: put this in calling code
        ISonglet *song = obox.GetContent();
        int RefCnt = song->UnRef_Songlet();
        if (RefCnt<=0){
          delete song;
        }
      }
      Refresh_Splines();
    } else {
      std::cout << "Element not found in SubSongs\n";
    }
  }
  /* ********************************************************************************* */
  void Refresh_Splines() {
      int SplineSize = (this->SubSongs.size()) * NumSubLines + 1;
      this->SplinePoints.resize(SplineSize);
      Splines::Cubic_Spline_Boxes(this->SubSongs, NumSubLines, SplinePoints);
  }
  /* ********************************************************************************* */
  double Get_Duration() override {
    return this->Duration;
  }
  /* ********************************************************************************* */
  double Get_Max_Amplitude() override {
    return this->MaxAmplitude;
  }
  /* ********************************************************************************* */
  void Update_Max_Amplitude() {
    int len = this->SubSongs.size();
    IOffsetBox *pnt;
    double MaxAmp = 0.0;
    for (int pcnt = 0; pcnt < len; pcnt++) {
      pnt = this->SubSongs.at(pcnt);
      double Amp = pnt->Get_Max_Amplitude();
      MaxAmp += Amp;// this is overkill, we need to only sum those subsongs that overlap.
    }
    this->MaxAmplitude = MaxAmp;
  }
  /* ********************************************************************************* */
  double Update_Durations() override {
    double MaxDuration = 0.0;
    double DurBuf = 0.0;
    int NumSubSongs = this->SubSongs.size();
    for (int cnt = 0; cnt < NumSubSongs; cnt++) {
      IOffsetBox *obox = this->SubSongs.at(cnt);
      ISonglet *child = obox->GetContent();
      //if (MaxDuration < (DurBuf = (ob.UnMapTime(vb.Update_Durations())))) {
      if (MaxDuration < (DurBuf = (obox->TimeX + child->Update_Durations()))) {
        MaxDuration = DurBuf;
      }
    }
    this->Duration = MaxDuration;
    return MaxDuration;
  }
  /* ********************************************************************************* */
  void Update_Guts(MetricsPacket& metrics) override {
    if (this->FreshnessTimeStamp < metrics.FreshnessTimeStamp) {// don't hit the same songlet twice on one update
      this->Set_Project(metrics.MyProject);
      //this->Sort_Me();
      this->Update_Max_Amplitude();
      metrics.MaxDuration = 0.0;// redundant
      double MyMaxDuration = 0.0;
      double DurBuf = 0.0;
      int NumSubSongs = this->SubSongs.size();
      for (int cnt = 0; cnt < NumSubSongs; cnt++) {
        IOffsetBox *obx = this->SubSongs.at(cnt);
        ISonglet *songlet = obx->GetContent();
        metrics.MaxDuration = 0.0;
        songlet->Update_Guts(metrics);
        if (MyMaxDuration < (DurBuf = (obx->TimeX + metrics.MaxDuration))) {
          MyMaxDuration = DurBuf;
        }
      }
      this->Duration = MyMaxDuration;
      this->FreshnessTimeStamp = metrics.FreshnessTimeStamp;
    }
    metrics.MaxDuration = this->Duration;
  }
  /* ********************************************************************************* */
  void Refresh_From_Beneath(IDrawable::IMoveable& mbox) override {
    //System.out.println("Refresh_From_Beneath");
    this->Sort_Me();
//    int Dex = this->Tree_Search(mbox.GetX(), 0, this->SubSongs.size());
//    this->Bubble_Right(Dex);// how do we get the right index? OffsetBoxes do not have that.
    if (UsingSplines) {
      Splines::Cubic_Spline_Boxes(this->SubSongs, NumSubLines, this->SplinePoints);
    }
  }
  /* ********************************************************************************* */
  void Bubble_Right(int Dex) {// when a point moves right in space, move it to the correct place in the collection.
    int len = this->SubSongs.size();
    IOffsetBox *mov = this->SubSongs.at(Dex);
    IOffsetBox *next;
    int PrevDex = Dex++;
    while (Dex < len) {
      next = this->SubSongs.at(Dex);
      if (mov->TimeX <= next->TimeX) {
        break;
      }
      this->SubSongs.at(PrevDex) = next;
      PrevDex = Dex++;
    }
    this->SubSongs.at(PrevDex) = mov;
  }
  /* ********************************************************************************* */
  void Sort_Me() {// sorting by RealTime
//    Collections.sort(this->SubSongs, new Comparator<IOffsetBox>() {
//      @Override public int compare(IOffsetBox voice0, IOffsetBox voice1) {
//        return Double.compare(voice0.TimeX, voice1.TimeX);
//      }
//    });
  }
  /* ********************************************************************************* */
  int Get_Sample_Count(int SampleRate) override {
    return SampleRate * (int) this->Get_Duration();
  }
  /* ********************************************************************************* */
  Config* Get_Project() override {
    return this->MyProject;
  }
  /* ********************************************************************************* */
  void Set_Project(Config* project) override {
    this->MyProject = project;
  }
  /* ********************************************************************************* */
  void SetSpineHighlight(boolean Highlight) {
    this->HighlightSpine = Highlight;
  }
  /* ********************************************************************************* */
  void Draw_Lines(IDrawingContext& ParentDC, int StartDex, int EndDex) {}
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) override {// IDrawable
  }
  CajaDelimitadora* GetBoundingBox() override {// IDrawable
    return this->MyBounds;
  }
  void UpdateBoundingBox() override {// IDrawable
    IOffsetBox *ChildOffsetBox;
//    CajaDelimitadora ChildBBoxUnMapped;
//    this->MyBounds.Reset();
    int len = this->SubSongs.size();
    for (int pcnt = 0; pcnt < len; pcnt++) {
      ChildOffsetBox = this->SubSongs.at(pcnt);
      ChildOffsetBox->UpdateBoundingBox();
//      ChildBBoxUnMapped = ChildOffsetBox.GetBoundingBox();// project child limits into parent (my) space
//      this->MyBounds.Include(ChildBBoxUnMapped);
    }
    this->UpdateBoundingBoxLocal();
  }
  void UpdateBoundingBoxLocal() override {// IDrawable
    IOffsetBox *ChildOffsetBox;
    CajaDelimitadora *ChildBBoxUnMapped;
    this->MyBounds->Reset();
    int len = this->SubSongs.size();
    if (len == 0) {
      this->MyBounds->ClearZero();
    } else {
      for (int pcnt = 0; pcnt < len; pcnt++) {
        ChildOffsetBox = this->SubSongs.at(pcnt);
        ChildBBoxUnMapped = ChildOffsetBox->GetBoundingBox();// project child limits into parent (my) space
        this->MyBounds->Include(*ChildBBoxUnMapped);
      }
    }
  }
  /* ********************************************************************************* */
  void GoFishing(IGrabber& Scoop) override {// IDrawable
    if (Scoop.Intersects(*MyBounds)) {// current search bounds are in parent coords
      int len = this->SubSongs.size();
      IOffsetBox *child;
      for (int pcnt = 0; pcnt < len; pcnt++) {
        child = this->SubSongs.at(pcnt);
        child->GoFishing(Scoop);
      }
    }
  }
  /* ********************************************************************************* */
  GroupBox* Clone_Me() override {// ICloneable
    GroupBox *child = new GroupBox();
    child->Copy_From(*this);
    return child;
  }
  /* ********************************************************************************* */
  GroupBox* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
    GroupBox *child;
    CollisionItem *ci = HitTable.GetItem(this);
    if (ci == null) {
      child = new GroupBox();
      ci = HitTable.InsertUniqueInstance(this);
      ci->Item = child;
      child->Copy_From(*this);
      IOffsetBox *SubSongHandle;
      int len = this->SubSongs.size();
      for (int cnt = 0; cnt < len; cnt++) {
        SubSongHandle = this->SubSongs.at(cnt);
        IOffsetBox *obox = SubSongHandle->Deep_Clone_Me(HitTable);
        child->Add_SubSong(obox);
      }
    } else {// pre exists
      child = (GroupBox*) ci->Item;// another cast!
    }
    return child;
  }
  /* ********************************************************************************* */
  GroupBox* Shallow_Clone_Me() {
    /*
     clone-me clones myself, clones all my children oboxes, but does NOT clone their songlets.
     */
    GroupBox *child;
    child = new GroupBox();
    //child->TraceText = "I am a shallow clone";
    child->Copy_From(*this);
    IOffsetBox *SubSongHandle, *ChildSubSongHandle;
    ISonglet *songlet;
    int len = this->SubSongs.size();
    for (int cnt = 0; cnt < len; cnt++) {
      SubSongHandle = this->SubSongs.at(cnt);
      songlet = SubSongHandle->GetContent();
      ChildSubSongHandle = songlet->Spawn_OffsetBox();
      ChildSubSongHandle->Copy_From(*SubSongHandle);
      child->Add_SubSong(ChildSubSongHandle);
    }
    return child;
  }
  /* ********************************************************************************* */
  void Copy_From(GroupBox& donor) {
    this->Duration = donor.Duration;
    this->Set_Project(donor.MyProject);
    this->MyName = donor.MyName;// for debugging
    this->MaxAmplitude = donor.MaxAmplitude;
    this->FreshnessTimeStamp = 0;// donor.FreshnessTimeStamp;
    this->MyBounds->Copy_From(*(donor.MyBounds));
  }
  /* ********************************************************************************* */
  void RescaleGroupTimeX(double Factor) {
    int len = this->SubSongs.size();
    for (int cnt = 0; cnt < len; cnt++) {
      IOffsetBox *obox = this->SubSongs.at(cnt);
      obox->TimeX *= Factor;
    }
  }
  /* ********************************************************************************* */
  static double DotProduct(double X0, double Y0, double X1, double Y1) {
    return X0 * X1 + Y0 * Y1;// length of projection from one vector onto another
  }
  /* ********************************************************************************* */
  void LineClosestPoint(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt, double YPnt, Point2D& Intersection) {// Find dnd destination using dot product on line segments.
    double Temp;
    if (LineX1 < LineX0 || (LineX1 == LineX0 && LineY1 < LineY0)) {// sort endpoints
      Temp = LineX0;// swap X
      LineX0 = LineX1;
      LineX1 = Temp;

      Temp = LineY0;// swap Y
      LineY0 = LineY1;
      LineY1 = Temp;
    }
    double XDif = LineX1 - LineX0, YDif = LineY1 - LineY0;
    double Shrink = Globals::Fudge;// shrink is a cheat so consecutive lines have slightly separate endpoints. this makes mouse distance from their endpoints unequal.
    double ShrinkX = XDif * Shrink, ShrinkY = YDif * Shrink;
    LineX0 += ShrinkX;
    LineY0 += ShrinkY;
    LineX1 -= ShrinkX;
    LineY1 -= ShrinkY;
    XDif -= ShrinkX * 2;
    YDif -= ShrinkY * 2;
    double Magnitude = Math::hypot(XDif, YDif);

    double DotProd = DotProduct(XPnt - LineX0, YPnt - LineY0, XDif, YDif);
    DotProd /= Magnitude;// now dotprod is the full length of the projection
    double XLoc = ((XDif / Magnitude) * DotProd);// scale separate dimensions to length of shadow
    double YLoc = ((YDif / Magnitude) * DotProd);
    if (XLoc > XDif) {// Test if the intersection is between the line's endpoints and cap them.
      //System.out.println("XLoc:" + XLoc + ", YLoc:" + YLoc);
      XLoc = XDif;
      YLoc = YDif;
    } else if (XLoc < 0) {
      XLoc = YLoc = 0;
    }
    XLoc += LineX0 + ShrinkX;
    YLoc += LineY0 + ShrinkY;
    Intersection.setLocation(XLoc, YLoc);
}
  /* ********************************************************************************* */
  static double DistanceFromLine(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt, double YPnt) {// work in progress for drag and drop support
    double XDif = LineX1 - LineX0, YDif = LineY1 - LineY0;
    double DotProd = DotProduct(XDif, YDif, XPnt - LineX0, YPnt - LineY0);
    double XLoc = LineX0 + (XDif * DotProd);// point of intersection
    double YLoc = LineY0 + (YDif * DotProd);
    // to do: at this point we would like to test if the intersection is between the line's endpoints.
//    double XLoc = (XDif * DotProd);// point of intersection
//    double YLoc = (YDif * DotProd);
//    double PntDX = (XLoc - XPnt); double PntDY = (YLoc - YPnt);
    double Distance = Math::hypot(XLoc - XPnt, YLoc - YPnt);
    return Distance;
  }
  /* ********************************************************************************* */
  double HeightFromLine(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt, double YPnt) {// work in progress for drag and drop support
    double YCross = LineYCross(LineX0, LineY0, LineX1, LineY1, XPnt);
    double Distance = YPnt - YCross;
    return Distance;
  }
  /* ********************************************************************************* */
  double LineYCross(double LineX0, double LineY0, double LineX1, double LineY1, double XPnt) {// work in progress for drag and drop support
    double XDif = LineX1 - LineX0;// given a line and an X point, return the Y location of the intersect along that line
    double YCross;
    if (XDif == 0) {// If slope is infinite, just return the halfway point between the top and bottom of the line.
      YCross = (LineY0 + LineY1) / 2.0;
    } else {
      double YDif = LineY1 - LineY0;
      double Slope = YDif / XDif;
      double PointXOffset = (XPnt - LineX0);
      YCross = LineY0 + (PointXOffset * Slope);
    }
    return YCross;
  }
  /* ********************************************************************************* */
  double HitsMyVineSpline(double XPnt, double YPnt) {// work in progress for drag and drop support
    double Limit = 0.1;// octaves.  hardcoded hack, need something better
    int len = this->SubSongs.size();
    double Dist;
    IOffsetBox *LastBox = this->SubSongs.at(len - 1);
    Point2D Intersection;// = new Point2D();
    if (0.0 <= XPnt && XPnt <= LastBox->TimeX) {// or this->MyBounds.Max.x) {
      int FoundDex = Tree_Search(XPnt, 0, len);// to do: tree search with buffer around click point
      int LinesPerSubSong = this->SplinePoints.size() / this->SubSongs.size();

      // Splines start to the left of subsong[0], so their indexes are higher than the subsongs they correspond to.
      int StartSplineDex = FoundDex * LinesPerSubSong;
      int EndSplineDex = (FoundDex + 1) * LinesPerSubSong;
      EndSplineDex = Math::min(EndSplineDex, this->SplinePoints.size());

      Point2D prevpnt, pnt(0, 0);// = Point2D.Zero;
      double MinDist = Double_POSITIVE_INFINITY;
      for (int scnt = StartSplineDex; scnt < EndSplineDex; scnt++) {// roll through all segments in this subrange of this spline. look for closest segment.
        prevpnt = pnt;
        pnt = this->SplinePoints[scnt];
        LineClosestPoint(prevpnt.x, prevpnt.y, pnt.x, pnt.y, XPnt, YPnt, Intersection);
        Dist = Math::hypot(XPnt - Intersection.x, YPnt - Intersection.y);
        //System.out.println("Dist:" + Dist + ", Intersection.x:" + Intersection.x + ", Intersection.y:" + Intersection.y);
        if (MinDist > Dist) {
          MinDist = Dist;
        }
      }
      if (MinDist < Limit) {// then we found one
        return MinDist;
      }
      // to do: need condition if FoundDex is greater than len. beyond-end insertion would be nice.
    }
    return Double_POSITIVE_INFINITY;// infinite if not found
  }
  /* ********************************************************************************* */
  double HitsMyVine(double XPnt, double YPnt) {// work in progress for drag and drop support
    double Limit = 0.1;// octaves.  hardcoded hack, need something better
    int len = this->SubSongs.size();
    IOffsetBox *OBox, *ClosestPoint = null;
    double XPrev = 0, YPrev = 0, YCross, YDist, Dist;
    IOffsetBox *LastBox = this->SubSongs.at(len - 1);
    Point2D Intersection;// = new Point2D();
    if (0.0 <= XPnt && XPnt <= LastBox->TimeX) {// or this->MyBounds.Max.x) {
//      int FoundDex = Tree_Search(XPnt - Limit, 0, len);
      int FoundDex = Tree_Search(XPnt, 0, len);
      if (FoundDex == 0) {// X point equals first element in subsong array
        XPrev = YPrev = 0;
      } else {
        IOffsetBox *PrevBox = this->SubSongs.at(FoundDex - 1);
        XPrev = PrevBox->TimeX;
        YPrev = PrevBox->OctaveY;
      }
      // to do: need condition if FoundDex is greater than len. beyond-end insertion would be nice.
      OBox = this->SubSongs.at(FoundDex);

      if (true) {
        LineClosestPoint(XPrev, YPrev, OBox->TimeX, OBox->OctaveY, XPnt, YPnt, Intersection);
        Dist = Math::hypot(XPnt - Intersection.x, YPnt - Intersection.y);
        //System.out.println("Dist:" + Dist + ", Intersection.x:" + Intersection.x + ", Intersection.y:" + Intersection.y);
        if (Dist < Limit) {// then we found one
          ClosestPoint = this->SubSongs.at(FoundDex);
          return Dist;
        }
      } else {
        YCross = LineYCross(XPrev, YPrev, OBox->TimeX, OBox->OctaveY, XPnt);
        YDist = Math::abs(YPnt - YCross);
        if (YDist < Limit) {// then we found one
          ClosestPoint = this->SubSongs.at(FoundDex);
          return YDist;
        }
      }
    }
    return Double_POSITIVE_INFINITY;// infinite if not found
  }
  /* ************************************************************************************************************************ */
  int Tree_Search(double Time, int minloc, int maxloc) {// finds place where time would be inserted or replaced
    int medloc;
    while (minloc < maxloc) {
      medloc = (minloc + maxloc) >> 1; // >>1 is same as div 2, only faster.
      if (Time <= this->SubSongs.at(medloc)->TimeX) {
        maxloc = medloc;
      } else {
        minloc = medloc + 1;/* has to go through here to be found. */
      }
    }
    return minloc;
  }
  /* ********************************************************************************* */
  void Space_Evenly() {
    int NumKids = this->SubSongs.size();
    if (NumKids == 0) {
      return;
    }
    int FinalKid = NumKids - 1;
    IOffsetBox *obox = this->SubSongs.at(FinalKid);
    double XLimit = obox->TimeX;
    double Spacing = XLimit / NumKids;
    double FractAlong;
    for (int cnt = 0; cnt <= FinalKid; cnt++) {// do we want it to be  ---.---.---.  or  .---.---.  or  .---.---.---  ?
      obox = this->SubSongs.at(cnt);
      FractAlong = (((double) cnt) / (double) FinalKid);//  .---.---.  spacing
      obox->TimeX = XLimit * FractAlong;
    }
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {// IDeletable
    return true;
  }
  void Delete_Me() override {// IDeletable
    this->MyBounds->Delete_Me();
    delete this->MyBounds; // wreck everything to prevent accidental re-use
    this->MyBounds = null;
    this->MyProject = null;
    this->Wipe_SubSongs();
    this->Duration = Double_NEGATIVE_INFINITY;
    this->FreshnessTimeStamp = Integer_MAX_VALUE;
  }
  void Wipe_SubSongs() {
    int len = this->SubSongs.size();
    IOffsetBox *obox;
    for (int cnt = 0; cnt < len; cnt++) {
      obox = this->SubSongs.at(cnt);
      obox->Delete_Me();
      delete obox;
    }
    this->SubSongs.clear();
  }
  /* ********************************************************************************* */
  int Ref_Songlet() override {// ISonglet Reference Counting: increment ref counter and return new value just for kicks
    return ++this->RefCount;
  }
  int UnRef_Songlet() override {// ISonglet Reference Counting: decrement ref counter and return new value just for kicks
    return --this->RefCount;
  }
  int GetRefCount() {// ISonglet Reference Counting: get number of references for serialization
    return this->RefCount;
  }
  /* ********************************************************************************* */
  JsonParse::Node* Export(CollisionLibrary& HitTable) override {// ITextable
    JsonParse::Node *phrase = new JsonParse::Node();
//    phrase->ChildrenHash = new HashMap<String, JsonParse::Node>();
//    phrase->AddSubPhrase("MyName", IFactory.Utils.PackField(this->MyName));
//
//    if (false) {
//      phrase->AddSubPhrase("MaxAmplitude", IFactory.Utils.PackField(this->MaxAmplitude));// can be calculated
//      phrase->AddSubPhrase("MyBounds", MyBounds.Export(HitTable));// can be calculated
//    }
//
//    // Save my array of songlets.
//    JsonParse.Node CPointsPhrase = new JsonParse.Node();
//    CPointsPhrase.ChildrenArray = IFactory.Utils.MakeArray(HitTable, this->SubSongs);
//    phrase->AddSubPhrase(this->SubSongsName, CPointsPhrase);

    return phrase;
  }
  void ShallowLoad(JsonParse::Node& phrase) {}
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
  /* ********************************************************************************* */
  class Group_Singer: public ISinger {
  public:
    GroupBox* MySonglet;
    ArrayList<ISinger*> NowPlaying;// pool of currently playing voices
    int Current_Dex = 0;
    double Prev_Time = 0;
    IOffsetBox MyOffsetBox;// private
    /* ********************************************************************************* */
    Group_Singer(){}
    ~Group_Singer(){ this->Delete_Me(); }
    /* ********************************************************************************* */
    void Start() override {
      IsFinished = false;
      Current_Dex = 0;
      Prev_Time = 0;
      NowPlaying.clear();
    }
    /* ********************************************************************************* */
    void Skip_To(double EndTime) override {
      if (this->IsFinished) {
        return;
      }
      EndTime = this->MyOffsetBox.MapTime(EndTime);// EndTime is now time internal to GroupBox's own coordinate system
      if (this->MySonglet->SubSongs.size() <= 0) {
        this->IsFinished = true;
        this->Prev_Time = EndTime;
        return;
      }
      double Clipped_EndTime = this->Tee_Up(EndTime);
      int NumPlaying = NowPlaying.size();
      ISinger *player = null;
      int cnt = 0;
      while (cnt < NumPlaying) {// then play the whole pool
        player = this->NowPlaying.at(cnt);
        player->Skip_To(Clipped_EndTime);
        cnt++;
      }
      cnt = 0;// now pack down the finished ones
      while (cnt < this->NowPlaying.size()) {
        player = this->NowPlaying.at(cnt);
        if (player->IsFinished) {
          this->NowPlaying.remove(player);
          player->Delete_Me();
          delete player;
        } else {
          cnt++;
        }
      }
      this->Prev_Time = EndTime;
    }
    /* ********************************************************************************* */
    void Render_To(double EndTime, Wave& wave) override {
      if (this->IsFinished) { return; }
      if (this->InheritedMap.LoudnessFactor == 0.0) {// muted, so don't waste time rendering
        return;
      }
      EndTime = this->MyOffsetBox.MapTime(EndTime);// EndTime is now time internal to GroupBox's own coordinate system
      double UnMapped_Prev_Time = this->InheritedMap.UnMapTime(this->Prev_Time);// get start time in parent coordinates
      if (this->MySonglet->SubSongs.size() <= 0) {
        this->IsFinished = true;
        wave.Init(UnMapped_Prev_Time, UnMapped_Prev_Time, this->MyProject->SampleRate);// wave times are in parent coordinates because the parent will be reading the wave data.
        this->Prev_Time = EndTime;
        return;
      }
      double Clipped_EndTime = this->Tee_Up(EndTime);
      double UnMapped_EndTime = this->InheritedMap.UnMapTime(Clipped_EndTime);
      wave.Init(UnMapped_Prev_Time, UnMapped_EndTime, this->MyProject->SampleRate);// wave times are in parent coordinates because the parent will be reading the wave data.
      Wave ChildWave;
      int NumPlaying = NowPlaying.size();
      ISinger *player = null;
      int cnt = 0;
      while (cnt < NumPlaying) {// then play the whole pool
        player = this->NowPlaying.at(cnt);
        player->Render_To(Clipped_EndTime, ChildWave);
        wave.Overdub(ChildWave);// sum/overdub the waves
        cnt++;
      }
      cnt = 0;// now pack down the finished ones
      while (cnt < this->NowPlaying.size()) {
        player = this->NowPlaying.at(cnt);
        if (player->IsFinished) {
          this->NowPlaying.remove(player);
          // player->Delete_Me();// redundant, should be handled internally by player/singer
          delete player;
        } else { cnt++; }
      }
      wave.Amplify(this->MyOffsetBox.LoudnessFactor);
      this->Prev_Time = EndTime;
    }
    /* ********************************************************************************* */
    void Inherit(ISinger& parent) override {}
    /* ********************************************************************************* */
    void Compound() override {}
    /* ********************************************************************************* */
    void Compound(MonkeyBox& donor) override {}
    /* ********************************************************************************* */
    IOffsetBox* Get_OffsetBox() override {
      return &(this->MyOffsetBox);
    }
    /* ********************************************************************************* */
    boolean Create_Me() override {// IDeletable
      return ISinger::Create_Me();
    }
    void Delete_Me() override {// IDeletable
      ISinger::Delete_Me();
      int len = this->NowPlaying.size();
      ISinger *singer;
      for (int cnt = 0; cnt < len; cnt++) {
        singer = this->NowPlaying.at(cnt);
        //singer->Delete_Me();// redundant
        delete singer;
      }
      this->NowPlaying.clear();
    }
    /* ********************************************************************************* */
    double Tee_Up(double EndTime) {// consolidating identical code
      if (EndTime < 0) {
        EndTime = 0;// clip time
      }
      int NumSonglets = MySonglet->SubSongs.size();
      int FinalSongletDex = NumSonglets - 1;
      double Final_Start = this->MySonglet->SubSongs.at(FinalSongletDex)->TimeX;
      Final_Start = Math::min(Final_Start, EndTime);
      double Final_Time = this->MySonglet->Get_Duration();
      if (EndTime > Final_Time) {
        this->IsFinished = true;
        EndTime = Final_Time;// clip time
      }
      IOffsetBox *obox;
      while (this->Current_Dex < NumSonglets) {// first find new songlets in this time range and add them to pool
        obox = MySonglet->SubSongs.at(this->Current_Dex);
        if (Final_Start < obox->TimeX) { break; }// repeat until obox start time overtakes EndTime
        ISinger *singer = obox->Spawn_Singer();
        singer->Inherit(*this);
        this->NowPlaying.add(singer);
        singer->Start();
        this->Current_Dex++;
      }
      return EndTime;
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
